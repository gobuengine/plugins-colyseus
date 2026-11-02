#include "colyseus_callbacks.h"
#include "colyseus_state.h"
#include <colyseus/room.h>
#include <colyseus/schema.h>
#include <colyseus/schema/ref_tracker.h>
#include <colyseus/schema/dynamic_schema.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// Helper functions
// ============================================================================

// Find a free entry slot
static GobuCallbackEntry* find_free_entry(ColyseusCallbacksWrapper* wrapper) {
    for (int i = 0; i < COLYSEUS_MAX_CALLBACK_ENTRIES; i++) {
        if (!wrapper->entries[i].active) {
            return &wrapper->entries[i];
        }
    }
    return NULL;
}

// Find entry by handle
static GobuCallbackEntry* find_entry_by_handle(ColyseusCallbacksWrapper* wrapper, int handle) {
    for (int i = 0; i < COLYSEUS_MAX_CALLBACK_ENTRIES; i++) {
        if (wrapper->entries[i].active && wrapper->entries[i].handle == handle) {
            return &wrapper->entries[i];
        }
    }
    return NULL;
}

static void set_call_error(GobuCallError* r_error, int32_t error, int32_t argument, int32_t expected) {
    r_error->error = error;
    r_error->argument = argument;
    r_error->expected = expected;
}

// ============================================================================
// Trampoline functions - Called by native SDK, invoke the script's Callables
// ============================================================================

// A schema instance as a Dictionary (dynamic or static vtable); nil without a vtable.
static void schema_to_variant(void* value, const colyseus_schema_vtable_t* vtable, GobuVariant* out_variant) {
    if (!vtable) {
        gobu->variant_new_nil(out_variant);
        return;
    }
    gobu->dictionary_new(out_variant);
    if (colyseus_vtable_is_dynamic(vtable)) {
        // A dynamic schema (the server's reflection, or a Colyseus.schema() definition)
        colyseus_dynamic_schema_to_dictionary((colyseus_dynamic_schema_t*)value, out_variant);
    } else {
        colyseus_schema_to_dictionary((colyseus_schema_t*)value, vtable, out_variant);
    }
}

// An item of a collection (a schema): the entry's item vtable, or the schema's own.
static void item_to_variant(void* value, const GobuCallbackEntry* entry, GobuVariant* out_variant) {
    if (!value) {
        gobu->variant_new_nil(out_variant);
        return;
    }
    colyseus_schema_t* schema = (colyseus_schema_t*)value;
    schema_to_variant(value, entry->item_vtable ? entry->item_vtable : schema->__vtable, out_variant);
}

// The key of a collection's item: an array's index (int*), or a map's key (char*).
static void key_to_variant(void* key, const GobuCallbackEntry* entry, GobuVariant* out_variant) {
    if (!key) {
        gobu->variant_new_nil(out_variant);
    } else if (entry->field_type == COLYSEUS_FIELD_ARRAY) {
        gobu->variant_new_int(out_variant, *(int*)key);
    } else {
        gobuext_new_string(out_variant, (const char*)key);
    }
}

// Helper to convert a native value to Variant based on field type
static void native_value_to_variant(void* value, int field_type, const colyseus_schema_vtable_t* item_vtable, GobuVariant* out_variant) {
    if (!value) {
        gobu->variant_new_nil(out_variant);
        return;
    }

    switch (field_type) {
        case COLYSEUS_FIELD_STRING:
            // value is char* directly
            gobu->variant_new_string(out_variant, (const char*)value, -1);
            break;
        case COLYSEUS_FIELD_INT8:  gobu->variant_new_int(out_variant, *(int8_t*)value); break;
        case COLYSEUS_FIELD_INT16: gobu->variant_new_int(out_variant, *(int16_t*)value); break;
        case COLYSEUS_FIELD_INT32: gobu->variant_new_int(out_variant, *(int32_t*)value); break;
        case COLYSEUS_FIELD_INT64: gobu->variant_new_int(out_variant, *(int64_t*)value); break;
        case COLYSEUS_FIELD_UINT8:  gobu->variant_new_int(out_variant, *(uint8_t*)value); break;
        case COLYSEUS_FIELD_UINT16: gobu->variant_new_int(out_variant, *(uint16_t*)value); break;
        case COLYSEUS_FIELD_UINT32: gobu->variant_new_int(out_variant, *(uint32_t*)value); break;
        case COLYSEUS_FIELD_UINT64: gobu->variant_new_int(out_variant, (int64_t)*(uint64_t*)value); break;
        case COLYSEUS_FIELD_FLOAT32: gobu->variant_new_float(out_variant, *(float*)value); break;
        case COLYSEUS_FIELD_FLOAT64: gobu->variant_new_float(out_variant, *(double*)value); break;
        case COLYSEUS_FIELD_BOOLEAN: gobu->variant_new_bool(out_variant, *(bool*)value); break;
        case COLYSEUS_FIELD_REF: {
            // value is schema pointer
            colyseus_schema_t* schema = (colyseus_schema_t*)value;
            schema_to_variant(value, item_vtable ? item_vtable : schema->__vtable, out_variant);
            break;
        }
        default:
            gobu->variant_new_nil(out_variant);
            break;
    }
}

// Calls the entry's Callable with call_deferred(), so that it runs on the main thread, and destroys the arguments.
static void call_deferred(GobuCallbackEntry* entry, GobuVariant* args, int64_t arg_count) {
    const GobuVariant* arg_ptrs[2] = { NULL, NULL };
    for (int64_t i = 0; i < arg_count; i++) {
        arg_ptrs[i] = &args[i];
    }
    gobuext_call(&entry->callable, "call_deferred", arg_ptrs, arg_count);
    for (int64_t i = 0; i < arg_count; i++) {
        gobu->variant_destroy(&args[i]);
    }
}

static void property_change_trampoline(void* value, void* previous_value, void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;

    GobuVariant args[2];  // (current, previous)
    native_value_to_variant(value, entry->field_type, entry->item_vtable, &args[0]);
    native_value_to_variant(previous_value, entry->field_type, entry->item_vtable, &args[1]);
    call_deferred(entry, args, 2);
}

// on_add and on_remove: (value, key)
static void item_trampoline(void* value, void* key, void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;

    GobuVariant args[2];
    item_to_variant(value, entry, &args[0]);
    key_to_variant(key, entry, &args[1]);
    call_deferred(entry, args, 2);
}

static void instance_change_trampoline(void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;

    call_deferred(entry, NULL, 0);
}

// on_change of a collection: (key, value)
static void collection_change_trampoline(void* key, void* value, void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;

    GobuVariant args[2];
    key_to_variant(key, entry, &args[0]);
    item_to_variant(value, entry, &args[1]);
    call_deferred(entry, args, 2);
}

// ============================================================================
// Instances
// ============================================================================

void* gobuext_colyseus_callbacks_create_instance(void* p_class_userdata, GobuObject* p_object) {
    (void)p_class_userdata;
    (void)p_object;
    // All entries start inactive
    return calloc(1, sizeof(ColyseusCallbacksWrapper));
}

void gobuext_colyseus_callbacks_free_instance(void* p_class_userdata, void* p_instance) {
    (void)p_class_userdata;

    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) return;

    // Free all callback entries
    for (int i = 0; i < COLYSEUS_MAX_CALLBACK_ENTRIES; i++) {
        if (wrapper->entries[i].active) {
            free(wrapper->entries[i].property);
            gobu->variant_destroy(&wrapper->entries[i].callable);
        }
    }

    // Free native callbacks (must be done before room is freed)
    if (wrapper->native_callbacks) {
        colyseus_callbacks_free(wrapper->native_callbacks);
    }

    free(wrapper);
}

// The callbacks of the room's decoder, once it is joined.
static void init_with_room(ColyseusCallbacksWrapper* wrapper, ColyseusRoomWrapper* room_wrapper) {
    wrapper->room_wrapper = room_wrapper;
    if (room_wrapper->native_room && room_wrapper->native_room->serializer && room_wrapper->native_room->serializer->decoder) {
        wrapper->native_callbacks = colyseus_callbacks_create(room_wrapper->native_room->serializer->decoder);
    }
}

// ============================================================================
// Static factory method: get(room)
// ============================================================================

GOBUEXT_METHOD(gobuext_colyseus_callbacks_get) {
    (void)p_method_userdata;
    (void)p_instance;  // NULL for static methods
    (void)p_argument_count;

    ColyseusRoomWrapper* room_wrapper = gobuext_colyseus_room_get_wrapper(gobu->variant_to_object(p_args[0]));
    if (!room_wrapper) {
        set_call_error(r_error, GOBU_CALL_ERROR_INVALID_ARGUMENT, 0, GOBU_TYPE_OBJECT);
        return;
    }

    if (!gobu->classdb_instantiate("_ColyseusCallbacks", r_return)) {
        set_call_error(r_error, GOBU_CALL_ERROR_INVALID_METHOD, 0, 0);
        return;
    }
    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)gobu->object_get_plugin_instance(gobu_plugin, gobu->variant_to_object(r_return));
    if (wrapper) {
        init_with_room(wrapper, room_wrapper);
    }
}

// ============================================================================
// Instance methods
// ============================================================================

static void* root_state(ColyseusCallbacksWrapper* wrapper) {
    if (wrapper->room_wrapper && wrapper->room_wrapper->native_room) {
        return colyseus_room_get_state(wrapper->room_wrapper->native_room);
    }
    return NULL;
}

// A nested schema of the state, by the "__ref_id" of its Dictionary.
static void* schema_by_ref_id(ColyseusCallbacksWrapper* wrapper, int ref_id) {
    if (wrapper->room_wrapper && wrapper->room_wrapper->native_room &&
        wrapper->room_wrapper->native_room->serializer &&
        wrapper->room_wrapper->native_room->serializer->decoder) {
        colyseus_ref_tracker_t* refs = wrapper->room_wrapper->native_room->serializer->decoder->refs;
        if (refs) {
            return colyseus_ref_tracker_get(refs, ref_id);
        }
    }
    return NULL;
}

// The type of `property` in the instance's vtable, and its items' vtable (collections).
static void find_field(void* instance, const char* property, GobuCallbackEntry* entry) {
    if (!instance || !property) return;
    colyseus_schema_t* schema = (colyseus_schema_t*)instance;
    if (!schema->__vtable) return;

    if (colyseus_vtable_is_dynamic(schema->__vtable)) {
        // Dynamic vtable - use dynamic field lookup
        const colyseus_dynamic_vtable_t* dyn_vtable = colyseus_vtable_as_dynamic(schema->__vtable);
        const colyseus_dynamic_field_t* dyn_field = colyseus_dynamic_vtable_find_field_by_name(dyn_vtable, property);
        if (dyn_field) {
            entry->field_type = dyn_field->type;
            entry->item_vtable = dyn_field->child_vtable ? &dyn_field->child_vtable->base : NULL;
        }
    } else {
        // Static vtable
        for (int i = 0; i < schema->__vtable->field_count; i++) {
            if (schema->__vtable->fields[i].name && strcmp(schema->__vtable->fields[i].name, property) == 0) {
                entry->field_type = schema->__vtable->fields[i].type;
                entry->item_vtable = schema->__vtable->fields[i].child_vtable;
                break;
            }
        }
    }
}

// The target of listen(), on_add() and on_remove(): ("property", callback) on the root state, or
// (schema_dict, "property", callback) on a nested schema. False, with r_error set, for other arguments.
static bool parse_property_target(ColyseusCallbacksWrapper* wrapper, const GobuVariant* const* p_args, int64_t p_argument_count,
    void** r_instance, char** r_property, const GobuVariant** r_callable, int* r_ref_id, GobuCallError* r_error) {
    if (p_argument_count < 2) {
        set_call_error(r_error, GOBU_CALL_ERROR_TOO_FEW_ARGUMENTS, 0, 2);
        return false;
    }
    const int32_t first_arg_type = gobu->variant_get_type(p_args[0]);
    if (first_arg_type == GOBU_TYPE_STRING) {
        *r_property = gobuext_to_utf8(p_args[0]);
        *r_callable = p_args[1];
        *r_ref_id = 0;
        *r_instance = root_state(wrapper);
        return true;
    }
    if (first_arg_type == GOBU_TYPE_DICTIONARY) {
        if (p_argument_count < 3) {
            set_call_error(r_error, GOBU_CALL_ERROR_TOO_FEW_ARGUMENTS, 0, 3);
            return false;
        }
        *r_ref_id = colyseus_extract_ref_id(p_args[0]);
        *r_property = gobuext_to_utf8(p_args[1]);
        *r_callable = p_args[2];
        *r_instance = schema_by_ref_id(wrapper, *r_ref_id);
        return true;
    }
    set_call_error(r_error, GOBU_CALL_ERROR_INVALID_ARGUMENT, 0, GOBU_TYPE_STRING);
    return false;
}

// A new entry for `callable` (copied); NULL, with -1 returned and `property` freed, when all are in use.
static GobuCallbackEntry* add_entry(ColyseusCallbacksWrapper* wrapper, ColyseusGobuCallbackType type, char* property,
    int ref_id, int default_field_type, const GobuVariant* callable, GobuVariant* r_return) {
    GobuCallbackEntry* entry = find_free_entry(wrapper);
    if (!entry) {
        free(property);
        gobu->variant_new_int(r_return, -1);
        return NULL;
    }
    entry->active = true;
    entry->type = type;
    entry->property = property;
    entry->ref_id = ref_id;
    entry->field_type = default_field_type;
    entry->item_vtable = NULL;
    gobu->variant_new_copy(&entry->callable, callable);
    return entry;
}

// The entry's handle: the native one, or a placeholder when it could not be registered.
static void finish_entry(ColyseusCallbacksWrapper* wrapper, GobuCallbackEntry* entry, bool registered, colyseus_callback_handle_t handle, GobuVariant* r_return) {
    entry->handle = registered ? handle : wrapper->entry_count;
    wrapper->entry_count++;
    gobu->variant_new_int(r_return, entry->handle);
}

GOBUEXT_METHOD(gobuext_colyseus_callbacks_listen) {
    (void)p_method_userdata;

    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        set_call_error(r_error, GOBU_CALL_ERROR_INSTANCE_IS_NULL, 0, 0);
        return;
    }

    void* instance = NULL;
    char* property_name = NULL;
    const GobuVariant* callable_arg = NULL;
    int ref_id = 0;
    if (!parse_property_target(wrapper, p_args, p_argument_count, &instance, &property_name, &callable_arg, &ref_id, r_error)) {
        return;
    }

    GobuCallbackEntry* entry = add_entry(wrapper, COLYSEUS_CB_LISTEN, property_name, ref_id, COLYSEUS_FIELD_STRING, callable_arg, r_return);
    if (!entry) return;
    find_field(instance, property_name, entry);

    const bool can_register = wrapper->native_callbacks && instance && property_name;
    finish_entry(wrapper, entry, can_register, can_register ? colyseus_callbacks_listen(
        wrapper->native_callbacks,
        instance,
        property_name,
        property_change_trampoline,
        entry,  // userdata
        true    // immediate
    ) : 0, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_callbacks_on_add) {
    (void)p_method_userdata;

    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        set_call_error(r_error, GOBU_CALL_ERROR_INSTANCE_IS_NULL, 0, 0);
        return;
    }

    void* instance = NULL;
    char* property_name = NULL;
    const GobuVariant* callable_arg = NULL;
    int ref_id = 0;
    if (!parse_property_target(wrapper, p_args, p_argument_count, &instance, &property_name, &callable_arg, &ref_id, r_error)) {
        return;
    }

    GobuCallbackEntry* entry = add_entry(wrapper, COLYSEUS_CB_ON_ADD, property_name, ref_id, COLYSEUS_FIELD_MAP, callable_arg, r_return);
    if (!entry) return;
    find_field(instance, property_name, entry);

    const bool can_register = wrapper->native_callbacks && instance && property_name;
    finish_entry(wrapper, entry, can_register, can_register ? colyseus_callbacks_on_add(
        wrapper->native_callbacks,
        instance,
        property_name,
        item_trampoline,
        entry,
        true  // immediate
    ) : 0, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_callbacks_on_remove) {
    (void)p_method_userdata;

    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        set_call_error(r_error, GOBU_CALL_ERROR_INSTANCE_IS_NULL, 0, 0);
        return;
    }

    void* instance = NULL;
    char* property_name = NULL;
    const GobuVariant* callable_arg = NULL;
    int ref_id = 0;
    if (!parse_property_target(wrapper, p_args, p_argument_count, &instance, &property_name, &callable_arg, &ref_id, r_error)) {
        return;
    }

    GobuCallbackEntry* entry = add_entry(wrapper, COLYSEUS_CB_ON_REMOVE, property_name, ref_id, COLYSEUS_FIELD_MAP, callable_arg, r_return);
    if (!entry) return;
    find_field(instance, property_name, entry);

    const bool can_register = wrapper->native_callbacks && instance && property_name;
    finish_entry(wrapper, entry, can_register, can_register ? colyseus_callbacks_on_remove(
        wrapper->native_callbacks,
        instance,
        property_name,
        item_trampoline,
        entry
    ) : 0, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_callbacks_on_change) {
    (void)p_method_userdata;

    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        set_call_error(r_error, GOBU_CALL_ERROR_INSTANCE_IS_NULL, 0, 0);
        return;
    }

    if (p_argument_count < 1) {
        set_call_error(r_error, GOBU_CALL_ERROR_TOO_FEW_ARGUMENTS, 0, 1);
        return;
    }

    const int32_t first_arg_type = gobu->variant_get_type(p_args[0]);

    void* instance = NULL;
    char* property_name = NULL;
    const GobuVariant* callable_arg = NULL;
    int ref_id = 0;
    bool is_instance_change = false;  // true = instance on_change (no property), false = collection on_change

    if (first_arg_type == GOBU_TYPE_CALLABLE) {
        // on_change(callback) — instance change on root state
        callable_arg = p_args[0];
        is_instance_change = true;
        instance = root_state(wrapper);

    } else if (first_arg_type == GOBU_TYPE_STRING) {
        // on_change("property", callback) — collection change on root state
        if (p_argument_count < 2) {
            set_call_error(r_error, GOBU_CALL_ERROR_TOO_FEW_ARGUMENTS, 0, 2);
            return;
        }
        property_name = gobuext_to_utf8(p_args[0]);
        callable_arg = p_args[1];
        instance = root_state(wrapper);

    } else if (first_arg_type == GOBU_TYPE_DICTIONARY) {
        if (p_argument_count < 2) {
            set_call_error(r_error, GOBU_CALL_ERROR_TOO_FEW_ARGUMENTS, 0, 2);
            return;
        }

        ref_id = colyseus_extract_ref_id(p_args[0]);
        if (gobu->variant_get_type(p_args[1]) == GOBU_TYPE_CALLABLE) {
            // on_change(schema_dict, callback) — instance change
            callable_arg = p_args[1];
            is_instance_change = true;
        } else {
            // on_change(schema_dict, "property", callback) — collection change
            if (p_argument_count < 3) {
                set_call_error(r_error, GOBU_CALL_ERROR_TOO_FEW_ARGUMENTS, 0, 3);
                return;
            }
            property_name = gobuext_to_utf8(p_args[1]);
            callable_arg = p_args[2];
        }
        instance = schema_by_ref_id(wrapper, ref_id);

    } else {
        set_call_error(r_error, GOBU_CALL_ERROR_INVALID_ARGUMENT, 0, GOBU_TYPE_CALLABLE);
        return;
    }

    GobuCallbackEntry* entry = add_entry(wrapper, COLYSEUS_CB_ON_CHANGE, property_name, ref_id, COLYSEUS_FIELD_MAP, callable_arg, r_return);
    if (!entry) return;

    // Look up field info for collection changes
    if (!is_instance_change) {
        find_field(instance, property_name, entry);
    }

    colyseus_callback_handle_t handle = 0;
    bool registered = false;
    if (wrapper->native_callbacks && instance) {
        if (is_instance_change) {
            handle = colyseus_callbacks_on_change_instance(
                wrapper->native_callbacks,
                instance,
                instance_change_trampoline,
                entry
            );
            registered = true;
        } else if (property_name) {
            handle = colyseus_callbacks_on_change_collection(
                wrapper->native_callbacks,
                instance,
                property_name,
                collection_change_trampoline,
                entry
            );
            registered = true;
        }
    }
    finish_entry(wrapper, entry, registered, handle, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_callbacks_remove) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return;

    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        set_call_error(r_error, GOBU_CALL_ERROR_INSTANCE_IS_NULL, 0, 0);
        return;
    }

    GobuCallbackEntry* entry = find_entry_by_handle(wrapper, (int)gobu->variant_to_int(p_args[0]));
    if (entry) {
        // Remove from native callbacks
        if (wrapper->native_callbacks) {
            colyseus_callbacks_remove(wrapper->native_callbacks, entry->handle);
        }

        // Clean up entry
        free(entry->property);
        entry->property = NULL;
        gobu->variant_destroy(&entry->callable);
        entry->active = false;
    }
}
