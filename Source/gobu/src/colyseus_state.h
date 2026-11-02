#ifndef COLYSEUS_STATE_H
#define COLYSEUS_STATE_H

#include "gobu_colyseus.h"
#include <colyseus/schema/types.h>
#include <colyseus/schema/collections.h>
#include <colyseus/schema/ref_tracker.h>
#include <colyseus/schema/dynamic_schema.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert a static schema instance to a Gobu Dictionary
 *
 * @param schema The schema instance to convert
 * @param vtable The vtable describing the schema structure
 * @param result A Dictionary: its fields are added to it
 */
void colyseus_schema_to_dictionary(
    const colyseus_schema_t* schema,
    const colyseus_schema_vtable_t* vtable,
    GobuVariant* result
);

/**
 * Convert a dynamic schema instance to a Gobu Dictionary
 *
 * @param schema The dynamic schema instance to convert
 * @param result A Dictionary: its fields are added to it
 */
void colyseus_dynamic_schema_to_dictionary(
    const colyseus_dynamic_schema_t* schema,
    GobuVariant* result
);

/**
 * Convert an ArraySchema to a Gobu Array
 *
 * @param array The array schema to convert
 * @param result An Array: its items are appended to it
 */
void colyseus_array_to_gobu_array(
    const colyseus_array_schema_t* array,
    GobuVariant* result
);

/**
 * Convert a MapSchema to a Gobu Dictionary
 *
 * @param map The map schema to convert
 * @param result A Dictionary: its items are added to it
 */
void colyseus_map_to_dictionary(
    const colyseus_map_schema_t* map,
    GobuVariant* result
);

/**
 * Convert a schema field value to a Gobu Variant
 *
 * @param ptr Pointer to the field value
 * @param field_type The field type enum
 * @param child_vtable For ref/array/map types, the child schema vtable (may be NULL)
 * @param result The Variant to write (uninitialized)
 */
void colyseus_field_to_variant(
    const void* ptr,
    colyseus_field_type_t field_type,
    const colyseus_schema_vtable_t* child_vtable,
    GobuVariant* result
);

/**
 * Extract __ref_id from a Dictionary of the state
 *
 * @param dict_variant The Dictionary
 * @return The ref_id, or -1 if not found
 */
int colyseus_extract_ref_id(const GobuVariant* dict_variant);

#ifdef __cplusplus
}
#endif

#endif /* COLYSEUS_STATE_H */
