/**
 * Msgpack to Gobu Variant conversion
 *
 * This module provides direct conversion from msgpack-encoded data to
 * native Gobu Variant types (Dictionary, Array, String, int, float, etc.)
 * without intermediate representations.
 */

#include "msgpack_variant.h"
#include "msgpack_decoder.h"
#include <stdlib.h>
#include <string.h>

// ============================================================================
// Internal types and constants
// ============================================================================

#define MSGPACK_STACK_MAX_DEPTH 64

typedef enum {
    STACK_TYPE_NONE = 0,
    STACK_TYPE_ARRAY,
    STACK_TYPE_DICTIONARY,
} StackItemType;

typedef struct {
    StackItemType type;
    GobuVariant variant;     // The Array or Dictionary
    size_t expected_count;   // Expected number of elements
    size_t current_count;    // Current number of elements added
    bool awaiting_key;       // For dictionaries: true if next value is a key
    GobuVariant pending_key; // For dictionaries: the key waiting for its value
} StackItem;

typedef struct {
    StackItem stack[MSGPACK_STACK_MAX_DEPTH];
    int stack_depth;
    GobuVariant* result;     // Pointer to store the final result
    bool success;
} MsgpackBuilderContext;

// ============================================================================
// Helper functions
// ============================================================================

// Adds a value to the current container, or makes it the result. Takes `value`: it is destroyed.
static void builder_add_value(MsgpackBuilderContext* ctx, GobuVariant* value) {
    if (ctx->stack_depth == 0) {
        // No container on stack, this is the root value
        gobu->variant_destroy(ctx->result);
        gobu->variant_new_copy(ctx->result, value);
        gobu->variant_destroy(value);
        return;
    }

    StackItem* current = &ctx->stack[ctx->stack_depth - 1];

    if (current->type == STACK_TYPE_ARRAY) {
        gobuext_array_append(&current->variant, value);
        current->current_count++;
    } else if (current->type == STACK_TYPE_DICTIONARY) {
        if (current->awaiting_key) {
            // This value is a key, store it for later
            gobu->variant_destroy(&current->pending_key);
            gobu->variant_new_copy(&current->pending_key, value);
            gobu->variant_destroy(value);
            current->awaiting_key = false;
        } else {
            // This value is the value for the pending key
            gobu->variant_set(&current->variant, &current->pending_key, value);
            gobu->variant_destroy(value);
            gobu->variant_destroy(&current->pending_key);
            gobu->variant_new_nil(&current->pending_key);

            current->current_count++;
            current->awaiting_key = true;  // Next value will be a key again
        }
    }
}

// ============================================================================
// Msgpack decoder callbacks
// ============================================================================

static void msgpack_value_cb(
    MsgpackValueType value_type,
    int64_t int_value,
    double float_value,
    bool bool_value,
    const uint8_t* data_ptr,
    size_t data_len,
    size_t container_len,
    void* userdata
) {
    (void)container_len;
    MsgpackBuilderContext* ctx = (MsgpackBuilderContext*)userdata;
    if (!ctx->success) return;

    GobuVariant value;

    switch (value_type) {
        case MSGPACK_VALUE_NIL:
            gobu->variant_new_nil(&value);
            break;

        case MSGPACK_VALUE_BOOL:
            gobu->variant_new_bool(&value, bool_value);
            break;

        case MSGPACK_VALUE_INT:
            gobu->variant_new_int(&value, int_value);
            break;

        case MSGPACK_VALUE_FLOAT:
            gobu->variant_new_float(&value, float_value);
            break;

        case MSGPACK_VALUE_STRING:
            gobu->variant_new_string(&value, data_ptr ? (const char*)data_ptr : "", data_ptr ? (int64_t)data_len : 0);
            break;

        case MSGPACK_VALUE_BINARY:
            gobu->variant_new_packed_byte_array(&value, data_ptr, data_ptr ? (int64_t)data_len : 0);
            break;

        case MSGPACK_VALUE_ARRAY:
        case MSGPACK_VALUE_DICTIONARY:
            // These are handled by container_callback
            return;

        default:
            gobu->variant_new_nil(&value);
            break;
    }

    builder_add_value(ctx, &value);
}

static void msgpack_container_cb(
    bool is_start,
    bool is_array,
    size_t length,
    void* userdata
) {
    MsgpackBuilderContext* ctx = (MsgpackBuilderContext*)userdata;
    if (!ctx->success) return;

    if (is_start) {
        // Starting a new container
        if (ctx->stack_depth >= MSGPACK_STACK_MAX_DEPTH) {
            ctx->success = false;
            return;
        }

        StackItem* item = &ctx->stack[ctx->stack_depth];
        item->expected_count = length;
        item->current_count = 0;
        gobu->variant_new_nil(&item->pending_key);

        if (is_array) {
            item->type = STACK_TYPE_ARRAY;
            item->awaiting_key = false;
            gobu->array_new(&item->variant);
        } else {
            item->type = STACK_TYPE_DICTIONARY;
            item->awaiting_key = true;  // First value in a map is a key
            gobu->dictionary_new(&item->variant);
        }

        ctx->stack_depth++;
    } else {
        // Ending a container
        if (ctx->stack_depth <= 0) {
            ctx->success = false;
            return;
        }

        ctx->stack_depth--;
        StackItem* finished = &ctx->stack[ctx->stack_depth];
        gobu->variant_destroy(&finished->pending_key);

        // Add the finished container to its parent (or set as result)
        builder_add_value(ctx, &finished->variant);

        // Reset the stack item
        finished->type = STACK_TYPE_NONE;
    }
}

// ============================================================================
// Public API
// ============================================================================

bool msgpack_to_gobu_variant(const uint8_t* data, size_t length, GobuVariant* out_variant) {
    if (!out_variant) {
        return false;
    }
    // Initialize result to nil
    gobu->variant_new_nil(out_variant);
    if (!data || length == 0) {
        return length == 0;  // Empty data is valid, returns nil
    }

    MsgpackBuilderContext ctx = {0};
    ctx.result = out_variant;
    ctx.success = true;
    ctx.stack_depth = 0;

    MsgpackDecoderContext decoder_ctx = {
        .value_callback = msgpack_value_cb,
        .container_callback = msgpack_container_cb,
        .userdata = &ctx
    };

    bool decode_success = msgpack_decode_to_gobu(data, length, &decoder_ctx);

    // Containers left open by a payload that ended early or failed
    while (ctx.stack_depth > 0) {
        ctx.stack_depth--;
        gobu->variant_destroy(&ctx.stack[ctx.stack_depth].variant);
        gobu->variant_destroy(&ctx.stack[ctx.stack_depth].pending_key);
    }

    return decode_success && ctx.success;
}
