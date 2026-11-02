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
    Variant callable;                    // The script's Callable, stored as a Variant
    int ref_id;                          // Schema reference ID (0 for root state)
    char* property;                      // Property name (owned)
    ColyseusGobuCallbackType type;      // Callback type
    bool active;                         // Whether this entry is in use
    int field_type;                      // Field type from colyseus_field_type_t
    const colyseus_schema_vtable_t* item_vtable;  // For collections: vtable of items
} GobuCallbackEntry;

// ColyseusCallbacks wrapper
typedef struct {
    colyseus_callbacks_t* native_callbacks;  // Native callbacks manager
    ColyseusRoomWrapper* room_wrapper;       // Reference to room
    NativeExtensionObjectPtr gobu_object;       // Gobu object pointer
    GobuCallbackEntry entries[COLYSEUS_MAX_CALLBACK_ENTRIES];
    int entry_count;
} ColyseusCallbacksWrapper;

// Constructor/destructor
NativeExtensionObjectPtr gobuext_colyseus_callbacks_constructor(void* p_class_userdata);
void gobuext_colyseus_callbacks_destructor(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance);

// Static factory method: ColyseusCallbacks.get(room)
void gobuext_colyseus_callbacks_get(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
);

// Instance methods

// listen(target, property_or_callback, [callback]) -> int
// - listen("property", callback) - root state
// - listen(schema_dict, "property", callback) - nested schema
void gobuext_colyseus_callbacks_listen(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
);

// on_add(target, property_or_callback, [callback]) -> int
void gobuext_colyseus_callbacks_on_add(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
);

// on_remove(target, property_or_callback, [callback]) -> int
void gobuext_colyseus_callbacks_on_remove(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
);

// on_change(target, property_or_callback, [callback]) -> int
void gobuext_colyseus_callbacks_on_change(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
);

// remove(handle) -> void
void gobuext_colyseus_callbacks_remove(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
);

// Internal: Initialize a callbacks wrapper with a room
void gobuext_colyseus_callbacks_init_with_room(
    ColyseusCallbacksWrapper* wrapper,
    ColyseusRoomWrapper* room_wrapper
);

// Internal: Get the last created wrapper (for factory method)
ColyseusCallbacksWrapper* gobuext_colyseus_callbacks_get_last_wrapper(void);

#ifdef __cplusplus
}
#endif

#endif /* COLYSEUS_CALLBACKS_H */
