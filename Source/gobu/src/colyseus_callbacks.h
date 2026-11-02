#ifndef COLYSEUS_CALLBACKS_H
#define COLYSEUS_CALLBACKS_H

#include "gobu_colyseus.h"
#include <colyseus/schema/callbacks.h>
#include <colyseus/schema/decoder.h>
#include <colyseus/schema/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maximum number of callback entries per callbacks instance
#define COLYSEUS_MAX_CALLBACK_ENTRIES 256

// Callback types
typedef enum {
    COLYSEUS_CB_LISTEN,
    COLYSEUS_CB_ON_ADD,
    COLYSEUS_CB_ON_REMOVE,
    COLYSEUS_CB_ON_CHANGE
} ColyseusGobuCallbackType;

// Callback entry - stores the script's callable and related info
typedef struct {
    colyseus_callback_handle_t handle;  // Native callback handle
    GobuVariant callable;                // The script's Callable
    int ref_id;                          // Schema reference ID (0 for root state)
    char* property;                      // Property name (owned)
    ColyseusGobuCallbackType type;      // Callback type
    bool active;                         // Whether this entry is in use
    int field_type;                      // Field type from colyseus_field_type_t
    const colyseus_schema_vtable_t* item_vtable;  // For collections: vtable of items
} GobuCallbackEntry;

// The data of a _ColyseusCallbacks
typedef struct {
    colyseus_callbacks_t* native_callbacks;  // Native callbacks manager
    ColyseusRoomWrapper* room_wrapper;       // Reference to room
    GobuCallbackEntry entries[COLYSEUS_MAX_CALLBACK_ENTRIES];
    int entry_count;
} ColyseusCallbacksWrapper;

void* gobuext_colyseus_callbacks_create_instance(void* p_class_userdata, GobuObject* p_object);
void gobuext_colyseus_callbacks_free_instance(void* p_class_userdata, void* p_instance);

// Static get(room) -> _ColyseusCallbacks: the callbacks of a joined room's state
GOBUEXT_METHOD(gobuext_colyseus_callbacks_get);

// Each returns a handle for remove(), or -1.

// listen(target, property_or_callback, [callback]) -> int
// - listen("property", callback) - root state
// - listen(schema_dict, "property", callback) - nested schema
GOBUEXT_METHOD(gobuext_colyseus_callbacks_listen);

// on_add(target, property_or_callback, [callback]) -> int: as listen()
GOBUEXT_METHOD(gobuext_colyseus_callbacks_on_add);

// on_remove(target, property_or_callback, [callback]) -> int: as listen()
GOBUEXT_METHOD(gobuext_colyseus_callbacks_on_remove);

// on_change(target, property_or_callback, [callback]) -> int
// - on_change(callback) - the root state changed
// - on_change("property", callback) - a collection of the root state changed
// - on_change(schema_dict, callback) - a nested schema changed
// - on_change(schema_dict, "property", callback) - a collection of a nested schema changed
GOBUEXT_METHOD(gobuext_colyseus_callbacks_on_change);

// remove(handle)
GOBUEXT_METHOD(gobuext_colyseus_callbacks_remove);

#ifdef __cplusplus
}
#endif

#endif /* COLYSEUS_CALLBACKS_H */
