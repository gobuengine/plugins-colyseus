#include "colyseus_schema_definition.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * Helpers
 * ============================================================================ */

/* Calls a method of a value; true when the call succeeded (then *result must be destroyed). */
static bool variant_call_method(Variant* variant, const char* method_name,
    NativeExtensionConstVariantPtr* args, int arg_count, Variant* result) {
    StringName method;
    constructors.string_name_new_with_latin1_chars(&method, method_name, false);
    NativeExtensionCallError error;
    api.variant_call(variant, &method, args, arg_count, result, &error);
    destructors.string_name_destructor(&method);
    return error.error == NATIVEEXTENSION_CALL_OK;
}

/* The UTF-8 of a String value (the caller frees it). */
static char* variant_to_cstr(Variant* variant) {
    String str;
    constructors.string_from_variant_constructor(&str, variant);
    char* buffer = NULL;
    int32_t length = api.string_to_utf8_chars(&str, NULL, 0);
    if (length > 0) {
        buffer = (char*)malloc(length + 1);
        if (buffer) {
            api.string_to_utf8_chars(&str, buffer, length);
            buffer[length] = '\0';
        }
    }
    destructors.string_destructor(&str);
    return buffer ? buffer : strdup("");
}

static int64_t array_size(Variant* array) {
    Variant size;
    if (!variant_call_method(array, "size", NULL, 0, &size)) {
        return 0;
    }
    int64_t value = 0;
    constructors.int_from_variant_constructor(&value, &size);
    destructors.variant_destroy(&size);
    return value;
}

/* dictionary.get(key) or array.get(index): true when it could be read (then *out must be destroyed). */
static bool get_key(Variant* container, const char* key, Variant* out) {
    String key_string;
    constructors.string_new_with_utf8_chars(&key_string, key);
    Variant key_variant;
    constructors.variant_from_string_constructor(&key_variant, &key_string);
    NativeExtensionConstVariantPtr args[1] = { &key_variant };
    bool success = variant_call_method(container, "get", args, 1, out);
    destructors.variant_destroy(&key_variant);
    destructors.string_destructor(&key_string);
    return success;
}

static bool get_index(Variant* array, int64_t index, Variant* out) {
    Variant index_variant;
    constructors.variant_from_int_constructor(&index_variant, &index);
    NativeExtensionConstVariantPtr args[1] = { &index_variant };
    bool success = variant_call_method(array, "get", args, 1, out);
    destructors.variant_destroy(&index_variant);
    return success;
}

/* ============================================================================
 * Definitions
 * ============================================================================ */

static void add_child(colyseus_schema_definition_t* definition, colyseus_schema_definition_t* child) {
    colyseus_schema_definition_t** children = realloc(definition->children,
        (definition->child_count + 1) * sizeof(colyseus_schema_definition_t*));
    if (!children) {
        colyseus_schema_definition_free(child);
        return;
    }
    definition->children = children;
    definition->children[definition->child_count++] = child;
}

/* A field: { "name", "type", "child_type", "key_type" } (Colyseus.field()). NULL when it has no name or type. */
static colyseus_dynamic_field_t* parse_field(Variant* field_variant, int field_index, colyseus_schema_definition_t* definition) {
    if (api.variant_get_type(field_variant) != NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) {
        fprintf(stderr, "[colyseus] schema field %d is not a Dictionary (Colyseus.field()); skipping\n", field_index);
        return NULL;
    }
    Variant name_variant;
    if (!get_key(field_variant, "name", &name_variant)) {
        fprintf(stderr, "[colyseus] schema field %d: no 'name'; skipping\n", field_index);
        return NULL;
    }
    char* field_name = variant_to_cstr(&name_variant);
    destructors.variant_destroy(&name_variant);

    Variant type_variant;
    if (!get_key(field_variant, "type", &type_variant)) {
        fprintf(stderr, "[colyseus] schema field '%s': no 'type'; skipping\n", field_name);
        free(field_name);
        return NULL;
    }
    char* field_type_str = variant_to_cstr(&type_variant);
    destructors.variant_destroy(&type_variant);

    colyseus_field_type_t field_type = colyseus_field_type_from_string(field_type_str);
    colyseus_dynamic_field_t* field = colyseus_dynamic_field_create(field_index, field_name, field_type, field_type_str);

    /* A ref, or the items of a map or an array: a nested schema, or a primitive type's name. */
    if (field && (field_type == COLYSEUS_FIELD_MAP || field_type == COLYSEUS_FIELD_ARRAY || field_type == COLYSEUS_FIELD_REF)) {
        Variant child_variant;
        if (get_key(field_variant, "child_type", &child_variant)) {
            NativeExtensionVariantType child_type = api.variant_get_type(&child_variant);
            if (child_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY || child_type == NATIVEEXTENSION_VARIANT_TYPE_ARRAY) {
                colyseus_schema_definition_t* child = colyseus_schema_definition_create(&child_variant);
                if (child) {
                    field->child_vtable = child->vtable;
                    add_child(definition, child);
                }
            } else if (child_type == NATIVEEXTENSION_VARIANT_TYPE_STRING) {
                field->child_primitive_type = variant_to_cstr(&child_variant);
            }
            destructors.variant_destroy(&child_variant);
        }
    }

    free(field_name);
    free(field_type_str);
    return field;
}

colyseus_schema_definition_t* colyseus_schema_definition_create(NativeExtensionConstVariantPtr definition_variant) {
    if (!definition_variant) {
        return NULL;
    }
    /* Colyseus.schema(): { "fields": [...] }; or the Array of fields itself. */
    Variant fields;
    NativeExtensionVariantType type = api.variant_get_type((NativeExtensionVariantPtr)definition_variant);
    if (type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) {
        if (!get_key((Variant*)definition_variant, "fields", &fields)) {
            return NULL;
        }
    } else if (type == NATIVEEXTENSION_VARIANT_TYPE_ARRAY) {
        api.variant_new_copy(&fields, (NativeExtensionConstVariantPtr)definition_variant);
    } else {
        return NULL;
    }
    if (api.variant_get_type(&fields) != NATIVEEXTENSION_VARIANT_TYPE_ARRAY) {
        fprintf(stderr, "[colyseus] a schema needs its fields (Colyseus.schema([Colyseus.field(...), ...]))\n");
        destructors.variant_destroy(&fields);
        return NULL;
    }

    colyseus_schema_definition_t* definition = calloc(1, sizeof(colyseus_schema_definition_t));
    if (!definition) {
        destructors.variant_destroy(&fields);
        return NULL;
    }
    definition->vtable = colyseus_dynamic_vtable_create("GrostSchema");
    if (!definition->vtable) {
        destructors.variant_destroy(&fields);
        free(definition);
        return NULL;
    }

    int64_t field_count = array_size(&fields);
    for (int64_t i = 0; i < field_count; i++) {
        Variant field_variant;
        if (get_index(&fields, i, &field_variant)) {
            colyseus_dynamic_field_t* field = parse_field(&field_variant, (int)i, definition);
            if (field) {
                colyseus_dynamic_vtable_add_field(definition->vtable, field);
            }
            destructors.variant_destroy(&field_variant);
        }
    }
    destructors.variant_destroy(&fields);
    return definition;
}

void colyseus_schema_definition_free(colyseus_schema_definition_t* definition) {
    if (!definition) {
        return;
    }
    for (int i = 0; i < definition->child_count; i++) {
        colyseus_schema_definition_free(definition->children[i]);
    }
    free(definition->children);
    colyseus_dynamic_vtable_free(definition->vtable);
    free(definition);
}
