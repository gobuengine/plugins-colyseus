#include "colyseus_schema_definition.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
static colyseus_dynamic_field_t* parse_field(const GobuVariant* field_variant, int field_index, colyseus_schema_definition_t* definition) {
    if (gobu->variant_get_type(field_variant) != GOBU_TYPE_DICTIONARY) {
        fprintf(stderr, "[colyseus] schema field %d is not a Dictionary (Colyseus.field()); skipping\n", field_index);
        return NULL;
    }
    const GobuVariant* name_variant = gobuext_dictionary_find(field_variant, "name");
    if (!name_variant) {
        fprintf(stderr, "[colyseus] schema field %d: no 'name'; skipping\n", field_index);
        return NULL;
    }
    char* field_name = gobuext_to_utf8(name_variant);

    const GobuVariant* type_variant = gobuext_dictionary_find(field_variant, "type");
    if (!type_variant) {
        fprintf(stderr, "[colyseus] schema field '%s': no 'type'; skipping\n", field_name);
        free(field_name);
        return NULL;
    }
    char* field_type_str = gobuext_to_utf8(type_variant);

    colyseus_field_type_t field_type = colyseus_field_type_from_string(field_type_str);
    colyseus_dynamic_field_t* field = colyseus_dynamic_field_create(field_index, field_name, field_type, field_type_str);

    /* A ref, or the items of a map or an array: a nested schema, or a primitive type's name. */
    if (field && (field_type == COLYSEUS_FIELD_MAP || field_type == COLYSEUS_FIELD_ARRAY || field_type == COLYSEUS_FIELD_REF)) {
        const GobuVariant* child_variant = gobuext_dictionary_find(field_variant, "child_type");
        if (child_variant) {
            const int32_t child_type = gobu->variant_get_type(child_variant);
            if (child_type == GOBU_TYPE_DICTIONARY || child_type == GOBU_TYPE_ARRAY) {
                colyseus_schema_definition_t* child = colyseus_schema_definition_create(child_variant);
                if (child) {
                    field->child_vtable = child->vtable;
                    add_child(definition, child);
                }
            } else if (child_type == GOBU_TYPE_STRING) {
                field->child_primitive_type = gobuext_to_utf8(child_variant);
            }
        }
    }

    free(field_name);
    free(field_type_str);
    return field;
}

colyseus_schema_definition_t* colyseus_schema_definition_create(const GobuVariant* definition_variant) {
    if (!definition_variant) {
        return NULL;
    }
    /* Colyseus.schema(): { "fields": [...] }; or the Array of fields itself. */
    const GobuVariant* fields = NULL;
    const int32_t type = gobu->variant_get_type(definition_variant);
    if (type == GOBU_TYPE_DICTIONARY) {
        fields = gobuext_dictionary_find(definition_variant, "fields");
    } else if (type == GOBU_TYPE_ARRAY) {
        fields = definition_variant;
    }
    if (!fields) {
        return NULL;
    }
    if (gobu->variant_get_type(fields) != GOBU_TYPE_ARRAY) {
        fprintf(stderr, "[colyseus] a schema needs its fields (Colyseus.schema([Colyseus.field(...), ...]))\n");
        return NULL;
    }

    colyseus_schema_definition_t* definition = calloc(1, sizeof(colyseus_schema_definition_t));
    if (!definition) {
        return NULL;
    }
    definition->vtable = colyseus_dynamic_vtable_create("GrostSchema");
    if (!definition->vtable) {
        free(definition);
        return NULL;
    }

    const int64_t field_count = gobu->variant_size(fields);
    for (int64_t i = 0; i < field_count; i++) {
        GobuVariant field_variant;
        if (gobu->variant_get_indexed(fields, i, &field_variant)) {
            colyseus_dynamic_field_t* field = parse_field(&field_variant, (int)i, definition);
            if (field) {
                colyseus_dynamic_vtable_add_field(definition->vtable, field);
            }
        }
        gobu->variant_destroy(&field_variant);
    }
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
