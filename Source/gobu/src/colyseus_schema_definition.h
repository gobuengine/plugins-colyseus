#ifndef COLYSEUS_SCHEMA_DEFINITION_H
#define COLYSEUS_SCHEMA_DEFINITION_H

#include "gobu_colyseus.h"
#include <colyseus/schema/dynamic_schema.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * A room's state described from Grost (Room.set_state_type()): the Dictionaries of Colyseus.schema() and
 * Colyseus.field() (colyseus.gro),
 *
 *     { "fields": [ { "name": "x", "type": "number", "child_type": null, "key_type": "string" }, ... ] }
 *
 * or just the Array of fields. "child_type" is a nested schema of the same kind (a "ref", or the items of a
 * "map" or an "array") or the name of a primitive type. It becomes a dynamic vtable for the decoder, and the
 * decoded state comes back as Dictionaries, as when the server's reflection describes it.
 */
typedef struct colyseus_schema_definition colyseus_schema_definition_t;

struct colyseus_schema_definition {
    colyseus_dynamic_vtable_t* vtable;
    /* The nested schemas: their vtables are the fields' child_vtable, which the parent's vtable does not own. */
    colyseus_schema_definition_t** children;
    int child_count;
};

/* NULL when the value is not a schema definition. */
colyseus_schema_definition_t* colyseus_schema_definition_create(const GobuVariant* definition);
/* With its nested schemas. */
void colyseus_schema_definition_free(colyseus_schema_definition_t* definition);

#ifdef __cplusplus
}
#endif

#endif /* COLYSEUS_SCHEMA_DEFINITION_H */
