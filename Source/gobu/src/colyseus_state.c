#include "colyseus_state.h"
#include <colyseus/schema/collections.h>
#include <colyseus/schema/dynamic_schema.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Magic marker used to identify dynamic vtables (from dynamic_schema.c) */
#define DYNAMIC_VTABLE_MAGIC_SIZE ((size_t)0xDEADBEEF)

/* Check if a vtable is a dynamic vtable */
static inline bool is_dynamic_vtable(const colyseus_schema_vtable_t* vtable) {
    return vtable && vtable->size == DYNAMIC_VTABLE_MAGIC_SIZE;
}

// Helper to create a Variant from an int64
static void variant_from_int(GobuVariant* result, int64_t value) {
    gobu->variant_new_int(result, value);
}

// Helper to create a Variant from a double
static void variant_from_float(GobuVariant* result, double value) {
    gobu->variant_new_float(result, value);
}

// Helper to create a Variant from a bool
static void variant_from_bool(GobuVariant* result, bool value) {
    gobu->variant_new_bool(result, value);
}

// Helper to create a Variant from a C string (NULL: nil)
static void variant_from_string(GobuVariant* result, const char* str) {
    if (str == NULL) {
        gobu->variant_new_nil(result);
        return;
    }
    gobu->variant_new_string(result, str, -1);
}

// A schema as a Dictionary: dynamic (from reflection auto-detection, or Colyseus.schema()) or static.
static void schema_to_variant(const void* schema, const colyseus_schema_vtable_t* vtable, GobuVariant* result) {
    gobu->dictionary_new(result);
    if (is_dynamic_vtable(vtable)) {
        colyseus_dynamic_schema_to_dictionary((const colyseus_dynamic_schema_t*)schema, result);
    } else {
        colyseus_schema_to_dictionary((const colyseus_schema_t*)schema, vtable, result);
    }
}

void colyseus_field_to_variant(
    const void* ptr,
    colyseus_field_type_t field_type,
    const colyseus_schema_vtable_t* child_vtable,
    GobuVariant* result
) {
    if (ptr == NULL) {
        gobu->variant_new_nil(result);
        return;
    }

    switch (field_type) {
        case COLYSEUS_FIELD_STRING: {
            const char* str = *(const char**)ptr;
            variant_from_string(result, str);
            break;
        }

        case COLYSEUS_FIELD_NUMBER:
        case COLYSEUS_FIELD_FLOAT32:
        case COLYSEUS_FIELD_FLOAT64: {
            // Numbers are stored as float in the schema
            float value = *(const float*)ptr;
            variant_from_float(result, (double)value);
            break;
        }

        case COLYSEUS_FIELD_BOOLEAN: {
            bool value = *(const bool*)ptr;
            variant_from_bool(result, value);
            break;
        }

        case COLYSEUS_FIELD_INT8: {
            int8_t value = *(const int8_t*)ptr;
            variant_from_int(result, (int64_t)value);
            break;
        }

        case COLYSEUS_FIELD_UINT8: {
            uint8_t value = *(const uint8_t*)ptr;
            variant_from_int(result, (int64_t)value);
            break;
        }

        case COLYSEUS_FIELD_INT16: {
            int16_t value = *(const int16_t*)ptr;
            variant_from_int(result, (int64_t)value);
            break;
        }

        case COLYSEUS_FIELD_UINT16: {
            uint16_t value = *(const uint16_t*)ptr;
            variant_from_int(result, (int64_t)value);
            break;
        }

        case COLYSEUS_FIELD_INT32: {
            int32_t value = *(const int32_t*)ptr;
            variant_from_int(result, (int64_t)value);
            break;
        }

        case COLYSEUS_FIELD_UINT32: {
            uint32_t value = *(const uint32_t*)ptr;
            variant_from_int(result, (int64_t)value);
            break;
        }

        case COLYSEUS_FIELD_INT64: {
            int64_t value = *(const int64_t*)ptr;
            variant_from_int(result, value);
            break;
        }

        case COLYSEUS_FIELD_UINT64: {
            uint64_t value = *(const uint64_t*)ptr;
            variant_from_int(result, (int64_t)value);
            break;
        }

        case COLYSEUS_FIELD_REF: {
            const colyseus_schema_t* child = *(const colyseus_schema_t**)ptr;
            if (child && child_vtable) {
                schema_to_variant(child, child_vtable, result);
            } else {
                gobu->variant_new_nil(result);
            }
            break;
        }

        case COLYSEUS_FIELD_ARRAY: {
            const colyseus_array_schema_t* arr = *(const colyseus_array_schema_t**)ptr;
            if (arr) {
                gobu->array_new(result);
                colyseus_array_to_gobu_array(arr, result);
            } else {
                gobu->variant_new_nil(result);
            }
            break;
        }

        case COLYSEUS_FIELD_MAP: {
            const colyseus_map_schema_t* map = *(const colyseus_map_schema_t**)ptr;
            if (map) {
                gobu->dictionary_new(result);
                colyseus_map_to_dictionary(map, result);
            } else {
                gobu->variant_new_nil(result);
            }
            break;
        }

        default:
            gobu->variant_new_nil(result);
            break;
    }
}

/* Forward declaration */
static void colyseus_dynamic_value_to_variant(
    const colyseus_dynamic_value_t* value,
    GobuVariant* result
);

/* Internal function to convert dynamic schema with explicit vtable */
static void colyseus_dynamic_schema_to_dictionary_with_vtable(
    const colyseus_dynamic_schema_t* schema,
    const colyseus_dynamic_vtable_t* dyn_vtable,
    GobuVariant* result
) {
    if (!schema || !result) return;

    // Add __ref_id field for nested callback support
    GobuVariant ref_id_variant;
    variant_from_int(&ref_id_variant, (int64_t)schema->__refId);
    gobuext_dictionary_set(result, "__ref_id", &ref_id_variant);

    // Use provided vtable or fall back to schema's vtable
    if (!dyn_vtable) {
        dyn_vtable = schema->__dyn_vtable;
    }

    if (!dyn_vtable) {
        // No vtable available - can't iterate fields
        return;
    }

    // Iterate over all fields defined in the vtable
    for (int i = 0; i < dyn_vtable->dyn_field_count; i++) {
        const colyseus_dynamic_field_t* field = dyn_vtable->dyn_fields[i];
        if (!field || !field->name) continue;

        // Look up the value by field index
        colyseus_dynamic_value_t* value = colyseus_dynamic_schema_get(
            (colyseus_dynamic_schema_t*)schema, field->index);

        if (!value) continue;  // Field not set yet

        GobuVariant field_variant;
        colyseus_dynamic_value_to_variant(value, &field_variant);
        gobuext_dictionary_set(result, field->name, &field_variant);
    }
}

void colyseus_schema_to_dictionary(
    const colyseus_schema_t* schema,
    const colyseus_schema_vtable_t* vtable,
    GobuVariant* result
) {
    if (!schema || !vtable || !result) return;

    // Check if this is a dynamic vtable - if so, forward to dynamic handler
    if (is_dynamic_vtable(vtable)) {
        const colyseus_dynamic_vtable_t* dyn_vtable = (const colyseus_dynamic_vtable_t*)vtable;
        colyseus_dynamic_schema_to_dictionary_with_vtable(
            (const colyseus_dynamic_schema_t*)schema, dyn_vtable, result);
        return;
    }

    // Add __ref_id field for nested callback support
    GobuVariant ref_id_variant;
    variant_from_int(&ref_id_variant, (int64_t)schema->__refId);
    gobuext_dictionary_set(result, "__ref_id", &ref_id_variant);

    // Convert each field (static vtable)
    for (int i = 0; i < vtable->field_count; i++) {
        const colyseus_field_t* field = &vtable->fields[i];
        const void* field_ptr = (const char*)schema + field->offset;

        GobuVariant field_variant;
        colyseus_field_to_variant(field_ptr, field->type, field->child_vtable, &field_variant);
        gobuext_dictionary_set(result, field->name, &field_variant);
    }
}

/* Convert a dynamic value to a Gobu Variant */
static void colyseus_dynamic_value_to_variant(
    const colyseus_dynamic_value_t* value,
    GobuVariant* result
) {
    if (!value || !result) {
        gobu->variant_new_nil(result);
        return;
    }

    switch (value->type) {
        case COLYSEUS_FIELD_STRING:
            variant_from_string(result, value->data.str);
            break;

        case COLYSEUS_FIELD_NUMBER:
        case COLYSEUS_FIELD_FLOAT64:
            variant_from_float(result, value->data.num);
            break;

        case COLYSEUS_FIELD_FLOAT32:
            variant_from_float(result, (double)value->data.f32);
            break;

        case COLYSEUS_FIELD_BOOLEAN:
            variant_from_bool(result, value->data.boolean);
            break;

        case COLYSEUS_FIELD_INT8:
            variant_from_int(result, (int64_t)value->data.i8);
            break;

        case COLYSEUS_FIELD_UINT8:
            variant_from_int(result, (int64_t)value->data.u8);
            break;

        case COLYSEUS_FIELD_INT16:
            variant_from_int(result, (int64_t)value->data.i16);
            break;

        case COLYSEUS_FIELD_UINT16:
            variant_from_int(result, (int64_t)value->data.u16);
            break;

        case COLYSEUS_FIELD_INT32:
            variant_from_int(result, (int64_t)value->data.i32);
            break;

        case COLYSEUS_FIELD_UINT32:
            variant_from_int(result, (int64_t)value->data.u32);
            break;

        case COLYSEUS_FIELD_INT64:
            variant_from_int(result, value->data.i64);
            break;

        case COLYSEUS_FIELD_UINT64:
            variant_from_int(result, (int64_t)value->data.u64);
            break;

        case COLYSEUS_FIELD_REF:
            if (value->data.ref) {
                gobu->dictionary_new(result);
                colyseus_dynamic_schema_to_dictionary(value->data.ref, result);
            } else {
                gobu->variant_new_nil(result);
            }
            break;

        case COLYSEUS_FIELD_ARRAY:
            if (value->data.array) {
                gobu->array_new(result);
                colyseus_array_to_gobu_array(value->data.array, result);
            } else {
                gobu->variant_new_nil(result);
            }
            break;

        case COLYSEUS_FIELD_MAP:
            if (value->data.map) {
                gobu->dictionary_new(result);
                colyseus_map_to_dictionary(value->data.map, result);
            } else {
                gobu->variant_new_nil(result);
            }
            break;

        default:
            gobu->variant_new_nil(result);
            break;
    }
}

void colyseus_dynamic_schema_to_dictionary(
    const colyseus_dynamic_schema_t* schema,
    GobuVariant* result
) {
    colyseus_dynamic_schema_to_dictionary_with_vtable(schema, NULL, result);
}

// Helper to convert a primitive void* value (from colyseus_decode_primitive) to a Variant
static void primitive_value_to_variant(void* value, const char* primitive_type, GobuVariant* result) {
    if (!value || !primitive_type) {
        gobu->variant_new_nil(result);
        return;
    }

    colyseus_field_type_t ft = colyseus_field_type_from_string(primitive_type);
    switch (ft) {
        case COLYSEUS_FIELD_STRING:
            // decode_primitive returns char* directly for strings
            variant_from_string(result, (const char*)value);
            break;
        case COLYSEUS_FIELD_NUMBER:
        case COLYSEUS_FIELD_FLOAT64:
            variant_from_float(result, *(double*)value);
            break;
        case COLYSEUS_FIELD_FLOAT32:
            variant_from_float(result, (double)*(float*)value);
            break;
        case COLYSEUS_FIELD_BOOLEAN:
            variant_from_bool(result, *(bool*)value);
            break;
        case COLYSEUS_FIELD_INT8:
            variant_from_int(result, (int64_t)*(int8_t*)value);
            break;
        case COLYSEUS_FIELD_UINT8:
            variant_from_int(result, (int64_t)*(uint8_t*)value);
            break;
        case COLYSEUS_FIELD_INT16:
            variant_from_int(result, (int64_t)*(int16_t*)value);
            break;
        case COLYSEUS_FIELD_UINT16:
            variant_from_int(result, (int64_t)*(uint16_t*)value);
            break;
        case COLYSEUS_FIELD_INT32:
            variant_from_int(result, (int64_t)*(int32_t*)value);
            break;
        case COLYSEUS_FIELD_UINT32:
            variant_from_int(result, (int64_t)*(uint32_t*)value);
            break;
        case COLYSEUS_FIELD_INT64:
            variant_from_int(result, *(int64_t*)value);
            break;
        case COLYSEUS_FIELD_UINT64:
            variant_from_int(result, (int64_t)*(uint64_t*)value);
            break;
        default:
            gobu->variant_new_nil(result);
            break;
    }
}

// Callback context for array iteration
typedef struct {
    GobuVariant* result;
    const colyseus_schema_vtable_t* child_vtable;
    bool has_schema_child;
    const char* child_primitive_type;
} ArrayConvertContext;

static void array_item_to_gobu(int index, void* value, void* userdata) {
    (void)index;
    ArrayConvertContext* ctx = (ArrayConvertContext*)userdata;

    GobuVariant item_variant;

    if (ctx->has_schema_child && ctx->child_vtable && value) {
        schema_to_variant(value, ctx->child_vtable, &item_variant);
    } else if (value) {
        primitive_value_to_variant(value, ctx->child_primitive_type, &item_variant);
    } else {
        gobu->variant_new_nil(&item_variant);
    }

    gobuext_array_append(ctx->result, &item_variant);
}

void colyseus_array_to_gobu_array(
    const colyseus_array_schema_t* array,
    GobuVariant* result
) {
    if (!array || !result) return;

    ArrayConvertContext ctx = {
        .result = result,
        .child_vtable = array->child_vtable,
        .has_schema_child = array->has_schema_child,
        .child_primitive_type = array->child_primitive_type
    };

    // Iterate through array items
    colyseus_array_schema_foreach((colyseus_array_schema_t*)array, array_item_to_gobu, &ctx);
}

// Callback context for map iteration
typedef struct {
    GobuVariant* result;
    const colyseus_schema_vtable_t* child_vtable;
    bool has_schema_child;
    const char* child_primitive_type;
} MapConvertContext;

static void map_item_to_gobu(const char* key, void* value, void* userdata) {
    MapConvertContext* ctx = (MapConvertContext*)userdata;

    GobuVariant item_variant;

    if (ctx->has_schema_child && ctx->child_vtable && value) {
        schema_to_variant(value, ctx->child_vtable, &item_variant);
    } else if (value) {
        primitive_value_to_variant(value, ctx->child_primitive_type, &item_variant);
    } else {
        gobu->variant_new_nil(&item_variant);
    }

    gobuext_dictionary_set(ctx->result, key, &item_variant);
}

void colyseus_map_to_dictionary(
    const colyseus_map_schema_t* map,
    GobuVariant* result
) {
    if (!map || !result) return;

    MapConvertContext ctx = {
        .result = result,
        .child_vtable = map->child_vtable,
        .has_schema_child = map->has_schema_child,
        .child_primitive_type = map->child_primitive_type
    };

    // Iterate through map items
    colyseus_map_schema_foreach((colyseus_map_schema_t*)map, map_item_to_gobu, &ctx);
}

int colyseus_extract_ref_id(const GobuVariant* dict_variant) {
    if (!dict_variant || gobu->variant_get_type(dict_variant) != GOBU_TYPE_DICTIONARY) return -1;

    const GobuVariant* ref_id = gobuext_dictionary_find(dict_variant, "__ref_id");
    return ref_id ? (int)gobu->variant_to_int(ref_id) : -1;
}
