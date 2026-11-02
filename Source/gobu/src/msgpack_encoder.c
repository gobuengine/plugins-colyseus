/**
 * Gobu Variant to Msgpack encoding
 *
 * This module provides conversion from native Gobu Variant types to
 * msgpack-encoded binary data.
 */

#include "msgpack_encoder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// Internal types and constants
// ============================================================================

// Initial buffer size and growth factor
#define INITIAL_BUFFER_SIZE 256
#define BUFFER_GROWTH_FACTOR 2

// Msgpack format bytes
#define MSGPACK_NIL         0xc0
#define MSGPACK_FALSE       0xc2
#define MSGPACK_TRUE        0xc3
#define MSGPACK_BIN8        0xc4
#define MSGPACK_BIN16       0xc5
#define MSGPACK_BIN32       0xc6
#define MSGPACK_FLOAT32     0xca
#define MSGPACK_FLOAT64     0xcb
#define MSGPACK_UINT8       0xcc
#define MSGPACK_UINT16      0xcd
#define MSGPACK_UINT32      0xce
#define MSGPACK_UINT64      0xcf
#define MSGPACK_INT8        0xd0
#define MSGPACK_INT16       0xd1
#define MSGPACK_INT32       0xd2
#define MSGPACK_INT64       0xd3
#define MSGPACK_STR8        0xd9
#define MSGPACK_STR16       0xda
#define MSGPACK_STR32       0xdb
#define MSGPACK_ARRAY16     0xdc
#define MSGPACK_ARRAY32     0xdd
#define MSGPACK_MAP16       0xde
#define MSGPACK_MAP32       0xdf

// Encoder buffer context
typedef struct {
    uint8_t* data;
    size_t size;
    size_t capacity;
} EncoderBuffer;

// ============================================================================
// Buffer management
// ============================================================================

static bool buffer_init(EncoderBuffer* buf) {
    buf->data = (uint8_t*)malloc(INITIAL_BUFFER_SIZE);
    if (!buf->data) return false;
    buf->size = 0;
    buf->capacity = INITIAL_BUFFER_SIZE;
    return true;
}

static void buffer_free(EncoderBuffer* buf) {
    if (buf->data) {
        free(buf->data);
        buf->data = NULL;
    }
    buf->size = 0;
    buf->capacity = 0;
}

static bool buffer_ensure_capacity(EncoderBuffer* buf, size_t additional) {
    size_t required = buf->size + additional;
    if (required <= buf->capacity) return true;

    size_t new_capacity = buf->capacity;
    while (new_capacity < required) {
        new_capacity *= BUFFER_GROWTH_FACTOR;
    }

    uint8_t* new_data = (uint8_t*)realloc(buf->data, new_capacity);
    if (!new_data) return false;

    buf->data = new_data;
    buf->capacity = new_capacity;
    return true;
}

static bool buffer_write_byte(EncoderBuffer* buf, uint8_t byte) {
    if (!buffer_ensure_capacity(buf, 1)) return false;
    buf->data[buf->size++] = byte;
    return true;
}

static bool buffer_write_bytes(EncoderBuffer* buf, const uint8_t* bytes, size_t len) {
    if (!buffer_ensure_capacity(buf, len)) return false;
    memcpy(buf->data + buf->size, bytes, len);
    buf->size += len;
    return true;
}

static bool buffer_write_uint16_be(EncoderBuffer* buf, uint16_t value) {
    uint8_t bytes[2] = {
        (uint8_t)(value >> 8),
        (uint8_t)(value & 0xff)
    };
    return buffer_write_bytes(buf, bytes, 2);
}

static bool buffer_write_uint32_be(EncoderBuffer* buf, uint32_t value) {
    uint8_t bytes[4] = {
        (uint8_t)(value >> 24),
        (uint8_t)((value >> 16) & 0xff),
        (uint8_t)((value >> 8) & 0xff),
        (uint8_t)(value & 0xff)
    };
    return buffer_write_bytes(buf, bytes, 4);
}

static bool buffer_write_uint64_be(EncoderBuffer* buf, uint64_t value) {
    uint8_t bytes[8] = {
        (uint8_t)(value >> 56),
        (uint8_t)((value >> 48) & 0xff),
        (uint8_t)((value >> 40) & 0xff),
        (uint8_t)((value >> 32) & 0xff),
        (uint8_t)((value >> 24) & 0xff),
        (uint8_t)((value >> 16) & 0xff),
        (uint8_t)((value >> 8) & 0xff),
        (uint8_t)(value & 0xff)
    };
    return buffer_write_bytes(buf, bytes, 8);
}

// ============================================================================
// Msgpack encoding helpers
// ============================================================================

static bool encode_nil(EncoderBuffer* buf) {
    return buffer_write_byte(buf, MSGPACK_NIL);
}

static bool encode_bool(EncoderBuffer* buf, bool value) {
    return buffer_write_byte(buf, value ? MSGPACK_TRUE : MSGPACK_FALSE);
}

static bool encode_int(EncoderBuffer* buf, int64_t value) {
    if (value >= 0) {
        // Positive integer
        if (value <= 127) {
            // Positive fixint: single byte 0x00-0x7f
            return buffer_write_byte(buf, (uint8_t)value);
        } else if (value <= 0xff) {
            // uint8
            return buffer_write_byte(buf, MSGPACK_UINT8) &&
                   buffer_write_byte(buf, (uint8_t)value);
        } else if (value <= 0xffff) {
            // uint16
            return buffer_write_byte(buf, MSGPACK_UINT16) &&
                   buffer_write_uint16_be(buf, (uint16_t)value);
        } else if (value <= 0xffffffff) {
            // uint32
            return buffer_write_byte(buf, MSGPACK_UINT32) &&
                   buffer_write_uint32_be(buf, (uint32_t)value);
        } else {
            // uint64
            return buffer_write_byte(buf, MSGPACK_UINT64) &&
                   buffer_write_uint64_be(buf, (uint64_t)value);
        }
    } else {
        // Negative integer
        if (value >= -32) {
            // Negative fixint: 0xe0-0xff
            return buffer_write_byte(buf, (uint8_t)(0xe0 | (value & 0x1f)));
        } else if (value >= -128) {
            // int8
            return buffer_write_byte(buf, MSGPACK_INT8) &&
                   buffer_write_byte(buf, (uint8_t)(int8_t)value);
        } else if (value >= -32768) {
            // int16
            return buffer_write_byte(buf, MSGPACK_INT16) &&
                   buffer_write_uint16_be(buf, (uint16_t)(int16_t)value);
        } else if (value >= -2147483648LL) {
            // int32
            return buffer_write_byte(buf, MSGPACK_INT32) &&
                   buffer_write_uint32_be(buf, (uint32_t)(int32_t)value);
        } else {
            // int64
            return buffer_write_byte(buf, MSGPACK_INT64) &&
                   buffer_write_uint64_be(buf, (uint64_t)value);
        }
    }
}

static bool encode_float(EncoderBuffer* buf, double value) {
    // Always use float64 for precision
    if (!buffer_write_byte(buf, MSGPACK_FLOAT64)) return false;

    // Convert to big-endian bytes
    union {
        double d;
        uint64_t u;
    } conv;
    conv.d = value;

    return buffer_write_uint64_be(buf, conv.u);
}

static bool encode_string(EncoderBuffer* buf, const char* str, size_t len) {
    if (len <= 31) {
        // fixstr: 0xa0-0xbf
        if (!buffer_write_byte(buf, (uint8_t)(0xa0 | len))) return false;
    } else if (len <= 0xff) {
        // str8
        if (!buffer_write_byte(buf, MSGPACK_STR8)) return false;
        if (!buffer_write_byte(buf, (uint8_t)len)) return false;
    } else if (len <= 0xffff) {
        // str16
        if (!buffer_write_byte(buf, MSGPACK_STR16)) return false;
        if (!buffer_write_uint16_be(buf, (uint16_t)len)) return false;
    } else {
        // str32
        if (!buffer_write_byte(buf, MSGPACK_STR32)) return false;
        if (!buffer_write_uint32_be(buf, (uint32_t)len)) return false;
    }

    return buffer_write_bytes(buf, (const uint8_t*)str, len);
}

static bool encode_binary(EncoderBuffer* buf, const uint8_t* data, size_t len) {
    if (len <= 0xff) {
        // bin8
        if (!buffer_write_byte(buf, MSGPACK_BIN8)) return false;
        if (!buffer_write_byte(buf, (uint8_t)len)) return false;
    } else if (len <= 0xffff) {
        // bin16
        if (!buffer_write_byte(buf, MSGPACK_BIN16)) return false;
        if (!buffer_write_uint16_be(buf, (uint16_t)len)) return false;
    } else {
        // bin32
        if (!buffer_write_byte(buf, MSGPACK_BIN32)) return false;
        if (!buffer_write_uint32_be(buf, (uint32_t)len)) return false;
    }

    return buffer_write_bytes(buf, data, len);
}

static bool encode_array_header(EncoderBuffer* buf, size_t len) {
    if (len <= 15) {
        // fixarray: 0x90-0x9f
        return buffer_write_byte(buf, (uint8_t)(0x90 | len));
    } else if (len <= 0xffff) {
        // array16
        return buffer_write_byte(buf, MSGPACK_ARRAY16) &&
               buffer_write_uint16_be(buf, (uint16_t)len);
    } else {
        // array32
        return buffer_write_byte(buf, MSGPACK_ARRAY32) &&
               buffer_write_uint32_be(buf, (uint32_t)len);
    }
}

static bool encode_map_header(EncoderBuffer* buf, size_t len) {
    if (len <= 15) {
        // fixmap: 0x80-0x8f
        return buffer_write_byte(buf, (uint8_t)(0x80 | len));
    } else if (len <= 0xffff) {
        // map16
        return buffer_write_byte(buf, MSGPACK_MAP16) &&
               buffer_write_uint16_be(buf, (uint16_t)len);
    } else {
        // map32
        return buffer_write_byte(buf, MSGPACK_MAP32) &&
               buffer_write_uint32_be(buf, (uint32_t)len);
    }
}

// ============================================================================
// Variant encoding (forward declaration for recursion)
// ============================================================================

static bool encode_variant(EncoderBuffer* buf, const GobuVariant* variant);

// ============================================================================
// Complex type encoding
// ============================================================================

// A String or StringName, as its UTF-8
static bool encode_gobu_string(EncoderBuffer* buf, const GobuVariant* str_variant) {
    char* text = gobuext_to_utf8(str_variant);
    if (!text) return false;
    bool result = encode_string(buf, text, strlen(text));
    free(text);
    return result;
}

static bool encode_packed_byte_array(EncoderBuffer* buf, const GobuVariant* pba_variant) {
    // A copy to read: the engine gives its bytes writable, which would detach the game's own array.
    GobuVariant bytes;
    gobu->variant_new_copy(&bytes, pba_variant);
    const int64_t size = gobu->variant_size(&bytes);
    const uint8_t* data = size > 0 ? gobu->packed_byte_array_data(&bytes) : NULL;
    bool result = encode_binary(buf, data, data ? (size_t)size : 0);
    gobu->variant_destroy(&bytes);
    return result;
}

static bool encode_array(EncoderBuffer* buf, const GobuVariant* arr_variant) {
    int64_t size = gobu->variant_size(arr_variant);

    if (!encode_array_header(buf, (size_t)size)) return false;

    for (int64_t i = 0; i < size; i++) {
        GobuVariant element;
        bool result = gobu->variant_get_indexed(arr_variant, i, &element) && encode_variant(buf, &element);
        gobu->variant_destroy(&element);

        if (!result) return false;
    }

    return true;
}

static bool encode_dictionary(EncoderBuffer* buf, const GobuVariant* dict_variant) {
    // Get keys array
    GobuVariant keys_variant;
    gobu->dictionary_keys(dict_variant, &keys_variant);

    int64_t size = gobu->variant_size(&keys_variant);
    bool result = encode_map_header(buf, (size_t)size);

    for (int64_t i = 0; result && i < size; i++) {
        GobuVariant key;
        result = gobu->variant_get_indexed(&keys_variant, i, &key);

        // Encode key, then its value
        const GobuVariant* value = result ? gobu->dictionary_find(dict_variant, &key) : NULL;
        result = value && encode_variant(buf, &key) && encode_variant(buf, value);
        gobu->variant_destroy(&key);
    }

    gobu->variant_destroy(&keys_variant);
    return result;
}

// ============================================================================
// Main variant encoding
// ============================================================================

static bool encode_variant(EncoderBuffer* buf, const GobuVariant* variant) {
    const int32_t type = gobu->variant_get_type(variant);

    switch (type) {
        case GOBU_TYPE_NIL:
            return encode_nil(buf);

        case GOBU_TYPE_BOOL:
            return encode_bool(buf, gobu->variant_to_bool(variant));

        case GOBU_TYPE_INT:
            return encode_int(buf, gobu->variant_to_int(variant));

        case GOBU_TYPE_FLOAT:
            return encode_float(buf, gobu->variant_to_float(variant));

        case GOBU_TYPE_STRING:
        case GOBU_TYPE_STRING_NAME:  // A StringName key of a Dictionary (as a script may write one)
            return encode_gobu_string(buf, variant);

        case GOBU_TYPE_PACKED_BYTE_ARRAY:
            return encode_packed_byte_array(buf, variant);

        case GOBU_TYPE_ARRAY:
            return encode_array(buf, variant);

        case GOBU_TYPE_DICTIONARY:
            return encode_dictionary(buf, variant);

        default:
            // Unsupported type - encode as nil
            printf("[msgpack_encoder] Warning: unsupported variant type %d, encoding as nil\n", (int)type);
            fflush(stdout);
            return encode_nil(buf);
    }
}

// ============================================================================
// Public API
// ============================================================================

uint8_t* gobu_variant_to_msgpack(const GobuVariant* variant, size_t* out_length) {
    if (!variant || !out_length) {
        if (out_length) *out_length = 0;
        return NULL;
    }

    EncoderBuffer buf;
    if (!buffer_init(&buf)) {
        *out_length = 0;
        return NULL;
    }

    if (!encode_variant(&buf, variant)) {
        buffer_free(&buf);
        *out_length = 0;
        return NULL;
    }

    *out_length = buf.size;

    // Transfer ownership of buffer to caller
    // Shrink to exact size to avoid waste
    if (buf.size < buf.capacity) {
        uint8_t* result = (uint8_t*)realloc(buf.data, buf.size);
        if (result) {
            return result;
        }
    }

    return buf.data;
}
