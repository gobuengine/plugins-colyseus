#include "colyseus_callbacks.h"
#include "colyseus_state.h"
#include <colyseus/room.h>
#include <colyseus/schema.h>
#include <colyseus/schema/ref_tracker.h>
#include <colyseus/schema/dynamic_schema.h>
#include <stdlib.h>
#include <string.h>

// Storage for the last created wrapper (for factory method)
static ColyseusCallbacksWrapper* g_last_created_callbacks_wrapper = NULL;

// ============================================================================
// Helper functions
// ============================================================================

static char* strdup_safe(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* dup = (char*)malloc(len);
    if (dup) memcpy(dup, str, len);
    return dup;
}

// Helper to extract C string from a Gobu String variant
static char* variant_string_to_c_str(NativeExtensionConstVariantPtr var) {
    // First extract String from Variant
    String str;
    constructors.string_from_variant_constructor(&str, (NativeExtensionVariantPtr)var);
    
    // Then convert to C string
    int32_t length = api.string_to_utf8_chars(&str, NULL, 0);
    if (length <= 0) {
        destructors.string_destructor(&str);
        return strdup_safe("");
    }
    
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        destructors.string_destructor(&str);
        return strdup_safe("");
    }
    
    api.string_to_utf8_chars(&str, buffer, length);
    buffer[length] = '\0';
    
    destructors.string_destructor(&str);
    return buffer;
}

// Helper to extract int64 from a Variant
static int64_t variant_to_int64(NativeExtensionConstVariantPtr var) {
    int64_t result = 0;
    constructors.int_from_variant_constructor(&result, (NativeExtensionVariantPtr)var);
    return result;
}

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

// ============================================================================
// Trampoline functions - Called by native SDK, invoke the script's Callables
// ============================================================================

// Helper to convert a native value to Variant based on field type
static void native_value_to_variant(void* value, int field_type, const colyseus_schema_vtable_t* item_vtable, Variant* out_variant) {
    if (!value) {
        gobuext_variant_new_nil(out_variant);
        return;
    }
    
    switch (field_type) {
        case COLYSEUS_FIELD_STRING: {
            // value is char* directly
            const char* str = (const char*)value;
            if (str) {
                String gobu_str;
                constructors.string_new_with_utf8_chars(&gobu_str, str);
                constructors.variant_from_string_constructor(out_variant, &gobu_str);
                destructors.string_destructor(&gobu_str);
            } else {
                gobuext_variant_new_nil(out_variant);
            }
            break;
        }
        case COLYSEUS_FIELD_INT8:
        case COLYSEUS_FIELD_INT16:
        case COLYSEUS_FIELD_INT32:
        case COLYSEUS_FIELD_INT64: {
            // value points to the integer
            int64_t int_val = 0;
            switch (field_type) {
                case COLYSEUS_FIELD_INT8:  int_val = *(int8_t*)value; break;
                case COLYSEUS_FIELD_INT16: int_val = *(int16_t*)value; break;
                case COLYSEUS_FIELD_INT32: int_val = *(int32_t*)value; break;
                case COLYSEUS_FIELD_INT64: int_val = *(int64_t*)value; break;
            }
            constructors.variant_from_int_constructor(out_variant, &int_val);
            break;
        }
        case COLYSEUS_FIELD_UINT8:
        case COLYSEUS_FIELD_UINT16:
        case COLYSEUS_FIELD_UINT32:
        case COLYSEUS_FIELD_UINT64: {
            int64_t int_val = 0;
            switch (field_type) {
                case COLYSEUS_FIELD_UINT8:  int_val = *(uint8_t*)value; break;
                case COLYSEUS_FIELD_UINT16: int_val = *(uint16_t*)value; break;
                case COLYSEUS_FIELD_UINT32: int_val = *(uint32_t*)value; break;
                case COLYSEUS_FIELD_UINT64: int_val = (int64_t)*(uint64_t*)value; break;
            }
            constructors.variant_from_int_constructor(out_variant, &int_val);
            break;
        }
        case COLYSEUS_FIELD_FLOAT32: {
            double float_val = *(float*)value;
            constructors.variant_from_float_constructor(out_variant, &float_val);
            break;
        }
        case COLYSEUS_FIELD_FLOAT64: {
            double float_val = *(double*)value;
            constructors.variant_from_float_constructor(out_variant, &float_val);
            break;
        }
        case COLYSEUS_FIELD_BOOLEAN: {
            NativeExtensionBool bool_val = *(bool*)value ? 1 : 0;
            constructors.variant_from_bool_constructor(out_variant, &bool_val);
            break;
        }
        case COLYSEUS_FIELD_REF: {
            // value is schema pointer
            colyseus_schema_t* schema = (colyseus_schema_t*)value;
            const colyseus_schema_vtable_t* vtable = item_vtable ? item_vtable : schema->__vtable;
            if (schema && vtable) {
                // A dynamic schema (the server's reflection, or a Colyseus.schema() definition): a Dictionary
                if (colyseus_vtable_is_dynamic(vtable)) {
                    colyseus_dynamic_schema_t* dyn_schema = (colyseus_dynamic_schema_t*)value;
                    Dictionary dict;
                    constructors.dictionary_constructor(&dict, NULL);
                    colyseus_dynamic_schema_to_dictionary(dyn_schema, &dict);
                    constructors.variant_from_dictionary_constructor(out_variant, &dict);
                } else {
                    // Static vtable - convert to Dictionary
                    Dictionary dict;
                    constructors.dictionary_constructor(&dict, NULL);
                    colyseus_schema_to_dictionary(schema, vtable, &dict);
                    constructors.variant_from_dictionary_constructor(out_variant, &dict);
                }
            } else {
                gobuext_variant_new_nil(out_variant);
            }
            break;
        }
        default:
            gobuext_variant_new_nil(out_variant);
            break;
    }
}

static void property_change_trampoline(void* value, void* previous_value, void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;

    // Create Variant arguments for the callable
    Variant current_variant;
    Variant previous_variant;

    native_value_to_variant(value, entry->field_type, entry->item_vtable, &current_variant);
    native_value_to_variant(previous_value, entry->field_type, entry->item_vtable, &previous_variant);

    // Use call_deferred to ensure the callable runs on the main thread.
    // WebSocket callbacks fire from a background thread, so direct calls crash Gobu.
    StringName call_method;
    constructors.string_name_new_with_latin1_chars(&call_method, "call_deferred", false);

    NativeExtensionConstVariantPtr arg_ptrs[2] = { &current_variant, &previous_variant };
    Variant return_value;
    NativeExtensionCallError error;

    api.variant_call((NativeExtensionVariantPtr)&entry->callable, &call_method, arg_ptrs, 2, &return_value, &error);

    destructors.string_name_destructor(&call_method);

    destructors.variant_destroy(&return_value);
    destructors.variant_destroy(&current_variant);
    destructors.variant_destroy(&previous_variant);
}

static void item_add_trampoline(void* value, void* key, void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;
    
    // Create Variant arguments
    Variant value_variant;
    Variant key_variant;
    
    // Convert value (schema) to a Dictionary
    if (value) {
        colyseus_schema_t* schema = (colyseus_schema_t*)value;
        const colyseus_schema_vtable_t* vtable = entry->item_vtable ? entry->item_vtable : schema->__vtable;

        if (vtable && colyseus_vtable_is_dynamic(vtable)) {
            colyseus_dynamic_schema_t* dyn_schema = (colyseus_dynamic_schema_t*)value;
            Dictionary dict;
            constructors.dictionary_constructor(&dict, NULL);
            colyseus_dynamic_schema_to_dictionary(dyn_schema, &dict);
            constructors.variant_from_dictionary_constructor(&value_variant, &dict);
        } else if (vtable) {
            // Static vtable - convert to Dictionary
            Dictionary dict;
            constructors.dictionary_constructor(&dict, NULL);
            colyseus_schema_to_dictionary(schema, vtable, &dict);
            constructors.variant_from_dictionary_constructor(&value_variant, &dict);
        } else {
            // No vtable, create nil variant
            gobuext_variant_new_nil(&value_variant);
        }
    } else {
        gobuext_variant_new_nil(&value_variant);
    }
    
    // Convert key based on collection type
    if (key) {
        if (entry->field_type == COLYSEUS_FIELD_ARRAY) {
            // For arrays, key is int* (pointer to index)
            int64_t index = *(int*)key;
            constructors.variant_from_int_constructor(&key_variant, &index);
        } else {
            // For maps, key is char* string
            const char* key_str = (const char*)key;
            String gobu_key;
            constructors.string_new_with_utf8_chars(&gobu_key, key_str);
            constructors.variant_from_string_constructor(&key_variant, &gobu_key);
            destructors.string_destructor(&gobu_key);
        }
    } else {
        gobuext_variant_new_nil(&key_variant);
    }
    
    // Use call_deferred to ensure the callable runs on the main thread.
    // WebSocket callbacks fire from a background thread, so direct calls crash Gobu.
    StringName call_method;
    constructors.string_name_new_with_latin1_chars(&call_method, "call_deferred", false);

    NativeExtensionConstVariantPtr arg_ptrs[2] = { &value_variant, &key_variant };
    Variant return_value;
    NativeExtensionCallError error;

    api.variant_call((NativeExtensionVariantPtr)&entry->callable, &call_method, arg_ptrs, 2, &return_value, &error);

    destructors.string_name_destructor(&call_method);

    destructors.variant_destroy(&return_value);
    destructors.variant_destroy(&value_variant);
    destructors.variant_destroy(&key_variant);
}

static void item_remove_trampoline(void* value, void* key, void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;
    
    // Create Variant arguments
    Variant value_variant;
    Variant key_variant;
    
    // Convert value (schema) to a Dictionary
    if (value) {
        colyseus_schema_t* schema = (colyseus_schema_t*)value;
        const colyseus_schema_vtable_t* vtable = entry->item_vtable ? entry->item_vtable : schema->__vtable;

        if (vtable && colyseus_vtable_is_dynamic(vtable)) {
            colyseus_dynamic_schema_t* dyn_schema = (colyseus_dynamic_schema_t*)value;
            Dictionary dict;
            constructors.dictionary_constructor(&dict, NULL);
            colyseus_dynamic_schema_to_dictionary(dyn_schema, &dict);
            constructors.variant_from_dictionary_constructor(&value_variant, &dict);
        } else if (vtable) {
            // Static vtable - convert to Dictionary
            Dictionary dict;
            constructors.dictionary_constructor(&dict, NULL);
            colyseus_schema_to_dictionary(schema, vtable, &dict);
            constructors.variant_from_dictionary_constructor(&value_variant, &dict);
        } else {
            gobuext_variant_new_nil(&value_variant);
        }
    } else {
        gobuext_variant_new_nil(&value_variant);
    }
    
    // Convert key based on collection type
    if (key) {
        if (entry->field_type == COLYSEUS_FIELD_ARRAY) {
            // For arrays, key is int* (pointer to index)
            int64_t index = *(int*)key;
            constructors.variant_from_int_constructor(&key_variant, &index);
        } else {
            // For maps, key is char* string
            const char* key_str = (const char*)key;
            String gobu_key;
            constructors.string_new_with_utf8_chars(&gobu_key, key_str);
            constructors.variant_from_string_constructor(&key_variant, &gobu_key);
            destructors.string_destructor(&gobu_key);
        }
    } else {
        gobuext_variant_new_nil(&key_variant);
    }
    
    // Use call_deferred to ensure the callable runs on the main thread.
    // WebSocket callbacks fire from a background thread, so direct calls crash Gobu.
    StringName call_method;
    constructors.string_name_new_with_latin1_chars(&call_method, "call_deferred", false);

    NativeExtensionConstVariantPtr arg_ptrs[2] = { &value_variant, &key_variant };
    Variant return_value;
    NativeExtensionCallError error;

    api.variant_call((NativeExtensionVariantPtr)&entry->callable, &call_method, arg_ptrs, 2, &return_value, &error);

    destructors.string_name_destructor(&call_method);

    destructors.variant_destroy(&return_value);
    destructors.variant_destroy(&value_variant);
    destructors.variant_destroy(&key_variant);
}

static void instance_change_trampoline(void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;

    // Use call_deferred with no arguments
    StringName call_method;
    constructors.string_name_new_with_latin1_chars(&call_method, "call_deferred", false);

    Variant return_value;
    NativeExtensionCallError error;

    api.variant_call((NativeExtensionVariantPtr)&entry->callable, &call_method, NULL, 0, &return_value, &error);

    destructors.string_name_destructor(&call_method);
    destructors.variant_destroy(&return_value);
}

static void collection_change_trampoline(void* key, void* value, void* userdata) {
    GobuCallbackEntry* entry = (GobuCallbackEntry*)userdata;
    if (!entry || !entry->active) return;

    Variant key_variant;
    Variant value_variant;

    // Convert key
    if (key) {
        if (entry->field_type == COLYSEUS_FIELD_ARRAY) {
            int64_t index = *(int*)key;
            constructors.variant_from_int_constructor(&key_variant, &index);
        } else {
            const char* key_str = (const char*)key;
            String gobu_key;
            constructors.string_new_with_utf8_chars(&gobu_key, key_str);
            constructors.variant_from_string_constructor(&key_variant, &gobu_key);
            destructors.string_destructor(&gobu_key);
        }
    } else {
        gobuext_variant_new_nil(&key_variant);
    }

    // Convert value
    if (value) {
        colyseus_schema_t* schema = (colyseus_schema_t*)value;
        const colyseus_schema_vtable_t* vtable = entry->item_vtable ? entry->item_vtable : schema->__vtable;

        if (vtable && colyseus_vtable_is_dynamic(vtable)) {
            colyseus_dynamic_schema_t* dyn_schema = (colyseus_dynamic_schema_t*)value;
            Dictionary dict;
            constructors.dictionary_constructor(&dict, NULL);
            colyseus_dynamic_schema_to_dictionary(dyn_schema, &dict);
            constructors.variant_from_dictionary_constructor(&value_variant, &dict);
        } else if (vtable) {
            Dictionary dict;
            constructors.dictionary_constructor(&dict, NULL);
            colyseus_schema_to_dictionary(schema, vtable, &dict);
            constructors.variant_from_dictionary_constructor(&value_variant, &dict);
        } else {
            gobuext_variant_new_nil(&value_variant);
        }
    } else {
        gobuext_variant_new_nil(&value_variant);
    }

    StringName call_method;
    constructors.string_name_new_with_latin1_chars(&call_method, "call_deferred", false);

    NativeExtensionConstVariantPtr arg_ptrs[2] = { &key_variant, &value_variant };
    Variant return_value;
    NativeExtensionCallError error;

    api.variant_call((NativeExtensionVariantPtr)&entry->callable, &call_method, arg_ptrs, 2, &return_value, &error);

    destructors.string_name_destructor(&call_method);
    destructors.variant_destroy(&return_value);
    destructors.variant_destroy(&key_variant);
    destructors.variant_destroy(&value_variant);
}

// ============================================================================
// Constructor/Destructor
// ============================================================================

NativeExtensionObjectPtr gobuext_colyseus_callbacks_constructor(void* p_class_userdata) {
    (void)p_class_userdata;
    
    // Create the Gobu Object (construct parent RefCounted class)
    StringName parent_class_name;
    constructors.string_name_new_with_latin1_chars(&parent_class_name, "RefCounted", false);
    
    NativeExtensionObjectPtr object = api.classdb_construct_object(&parent_class_name);
    destructors.string_name_destructor(&parent_class_name);
    
    if (!object) return NULL;
    
    // Create our wrapper instance data
    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)calloc(1, sizeof(ColyseusCallbacksWrapper));
    if (!wrapper) return NULL;
    
    wrapper->native_callbacks = NULL;
    wrapper->room_wrapper = NULL;
    wrapper->gobu_object = object;
    wrapper->entry_count = 0;
    
    // Initialize all entries as inactive
    for (int i = 0; i < COLYSEUS_MAX_CALLBACK_ENTRIES; i++) {
        wrapper->entries[i].active = false;
        wrapper->entries[i].property = NULL;
    }
    
    // Attach our wrapper to the Gobu object
    StringName class_name;
    constructors.string_name_new_with_latin1_chars(&class_name, "_ColyseusCallbacks", false);
    api.object_set_instance(object, &class_name, wrapper);
    destructors.string_name_destructor(&class_name);
    
    // Store for retrieval by factory method
    g_last_created_callbacks_wrapper = wrapper;
    
    return object;
}

void gobuext_colyseus_callbacks_destructor(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance) {
    (void)p_class_userdata;
    
    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) return;
    
    // Free all callback entries
    for (int i = 0; i < COLYSEUS_MAX_CALLBACK_ENTRIES; i++) {
        if (wrapper->entries[i].active) {
            if (wrapper->entries[i].property) {
                free(wrapper->entries[i].property);
            }
            // Destroy the callable variant
            destructors.variant_destroy(&wrapper->entries[i].callable);
        }
    }
    
    // Free native callbacks (must be done before room is freed)
    if (wrapper->native_callbacks) {
        colyseus_callbacks_free(wrapper->native_callbacks);
    }
    
    free(wrapper);
}

ColyseusCallbacksWrapper* gobuext_colyseus_callbacks_get_last_wrapper(void) {
    ColyseusCallbacksWrapper* wrapper = g_last_created_callbacks_wrapper;
    g_last_created_callbacks_wrapper = NULL;
    return wrapper;
}

void gobuext_colyseus_callbacks_init_with_room(
    ColyseusCallbacksWrapper* wrapper,
    ColyseusRoomWrapper* room_wrapper
) {
    if (!wrapper || !room_wrapper) return;
    
    wrapper->room_wrapper = room_wrapper;
    
    // Get the decoder from the room's serializer
    if (room_wrapper->native_room) {
        if (room_wrapper->native_room->serializer) {
            if (room_wrapper->native_room->serializer->decoder) {
                wrapper->native_callbacks = colyseus_callbacks_create(
                    room_wrapper->native_room->serializer->decoder
                );
                return;
            }
        }
    }
}

// ============================================================================
// Static factory method: ColyseusCallbacks.get(room)
// ============================================================================

void gobuext_colyseus_callbacks_get(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
) {
    (void)p_method_userdata;
    (void)p_instance;  // NULL for static methods
    
    if (p_argument_count < 1) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
        return;
    }
    
    // Extract the room object from the variant
    NativeExtensionObjectPtr room_obj = NULL;
    constructors.object_from_variant_constructor(&room_obj, (NativeExtensionVariantPtr)p_args[0]);
    
    if (!room_obj) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INVALID_ARGUMENT;
        return;
    }
    
    // Get the room's instance ID and look up the wrapper from our registry
    NativeExtensionObjectInstanceID room_instance_id = api.object_get_instance_id(room_obj);
    
    // Look up the room wrapper from our global registry
    ColyseusRoomWrapper* room_wrapper = gobuext_colyseus_room_get_wrapper_by_id(room_instance_id);
    
    // Create a new ColyseusCallbacks instance
    StringName class_name;
    constructors.string_name_new_with_latin1_chars(&class_name, "_ColyseusCallbacks", false);
    NativeExtensionObjectPtr callbacks_obj = api.classdb_construct_object(&class_name);
    destructors.string_name_destructor(&class_name);
    
    if (!callbacks_obj) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INVALID_METHOD;
        return;
    }
    
    // Get the wrapper that was just created
    ColyseusCallbacksWrapper* wrapper = gobuext_colyseus_callbacks_get_last_wrapper();
    
    // Initialize with the room - try to get room_wrapper from the object directly
    // Since object_get_instance_binding may not work for our custom instance data,
    // we need a different approach. Let's store a global mapping or use a different method.
    
    // For now, let's try to initialize if we have room info
    if (wrapper && room_wrapper) {
        gobuext_colyseus_callbacks_init_with_room(wrapper, room_wrapper);
    }
    
    // Return the callbacks object as a variant
    if (r_return) {
        constructors.variant_from_object_constructor(r_return, &callbacks_obj);
    }
}

// ============================================================================
// Instance methods
// ============================================================================

void gobuext_colyseus_callbacks_listen(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
) {
    (void)p_method_userdata;
    
    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
        return;
    }
    
    if (p_argument_count < 2) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
        return;
    }
    
    // Determine argument pattern based on first arg type
    NativeExtensionVariantType first_arg_type = api.variant_get_type(p_args[0]);
    
    void* instance = NULL;
    char* property_name = NULL;
    NativeExtensionConstVariantPtr callable_arg = NULL;
    
    if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_STRING) {
        // listen("property", callback) - root state
        property_name = variant_string_to_c_str(p_args[0]);
        callable_arg = p_args[1];
        
        // Get root state
        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room) {
            instance = colyseus_room_get_state(wrapper->room_wrapper->native_room);
        }
        
    } else if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) {
        // listen(schema_dict, "property", callback) - nested schema
        if (p_argument_count < 3) {
            if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
            return;
        }
        
        // Extract ref_id from dictionary
        int ref_id = colyseus_extract_ref_id(p_args[0]);
        property_name = variant_string_to_c_str(p_args[1]);
        callable_arg = p_args[2];
        
        // Look up instance by ref_id
        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room &&
            wrapper->room_wrapper->native_room->serializer &&
            wrapper->room_wrapper->native_room->serializer->decoder) {
            
            colyseus_ref_tracker_t* refs = 
                wrapper->room_wrapper->native_room->serializer->decoder->refs;
            if (refs) {
                instance = colyseus_ref_tracker_get(refs, ref_id);
            }
        }
        
    } else {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INVALID_ARGUMENT;
        return;
    }
    
    // Find a free entry
    GobuCallbackEntry* entry = find_free_entry(wrapper);
    if (!entry) {
        if (property_name) free(property_name);
        int64_t result = -1;
        if (r_return) constructors.variant_from_int_constructor(r_return, &result);
        return;
    }
    
    // Set up entry
    entry->active = true;
    entry->type = COLYSEUS_CB_LISTEN;
    entry->property = property_name;
    entry->ref_id = (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) ? colyseus_extract_ref_id(p_args[0]) : 0;
    entry->field_type = COLYSEUS_FIELD_STRING;  // Default
    entry->item_vtable = NULL;
    
    // Look up field type from instance's vtable
    if (instance && property_name) {
        colyseus_schema_t* schema = (colyseus_schema_t*)instance;
        if (schema->__vtable) {
            if (colyseus_vtable_is_dynamic(schema->__vtable)) {
                // Dynamic vtable - use dynamic field lookup
                const colyseus_dynamic_vtable_t* dyn_vtable = colyseus_vtable_as_dynamic(schema->__vtable);
                const colyseus_dynamic_field_t* dyn_field = colyseus_dynamic_vtable_find_field_by_name(dyn_vtable, property_name);
                if (dyn_field) {
                    entry->field_type = dyn_field->type;
                    entry->item_vtable = dyn_field->child_vtable ? &dyn_field->child_vtable->base : NULL;
                }
            } else {
                // Static vtable
                for (int i = 0; i < schema->__vtable->field_count; i++) {
                    if (schema->__vtable->fields[i].name && strcmp(schema->__vtable->fields[i].name, property_name) == 0) {
                        entry->field_type = schema->__vtable->fields[i].type;
                        entry->item_vtable = schema->__vtable->fields[i].child_vtable;
                        break;
                    }
                }
            }
        }
    }
    
    // Copy the callable variant (properly increments reference count)
    api.variant_new_copy(&entry->callable, callable_arg);
    
    // Register with native callbacks if available
    if (wrapper->native_callbacks && instance && property_name) {
        entry->handle = colyseus_callbacks_listen(
            wrapper->native_callbacks,
            instance,
            property_name,
            property_change_trampoline,
            entry,  // userdata
            true    // immediate
        );
    } else {
        entry->handle = wrapper->entry_count;
    }
    
    wrapper->entry_count++;
    
    // Return handle
    int64_t handle = entry->handle;
    if (r_return) constructors.variant_from_int_constructor(r_return, &handle);
}

void gobuext_colyseus_callbacks_on_add(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
) {
    (void)p_method_userdata;
    
    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
        return;
    }
    
    if (p_argument_count < 2) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
        return;
    }
    
    // Similar logic to listen()
    NativeExtensionVariantType first_arg_type = api.variant_get_type(p_args[0]);
    
    void* instance = NULL;
    char* property_name = NULL;
    NativeExtensionConstVariantPtr callable_arg = NULL;
    
    if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_STRING) {
        property_name = variant_string_to_c_str(p_args[0]);
        callable_arg = p_args[1];
        
        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room) {
            instance = colyseus_room_get_state(wrapper->room_wrapper->native_room);
        }
        
    } else if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) {
        if (p_argument_count < 3) {
            if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
            return;
        }
        
        int ref_id = colyseus_extract_ref_id(p_args[0]);
        property_name = variant_string_to_c_str(p_args[1]);
        callable_arg = p_args[2];
        
        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room &&
            wrapper->room_wrapper->native_room->serializer &&
            wrapper->room_wrapper->native_room->serializer->decoder) {
            
            colyseus_ref_tracker_t* refs = 
                wrapper->room_wrapper->native_room->serializer->decoder->refs;
            if (refs) {
                instance = colyseus_ref_tracker_get(refs, ref_id);
            }
        }
    } else {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INVALID_ARGUMENT;
        return;
    }
    
    GobuCallbackEntry* entry = find_free_entry(wrapper);
    if (!entry) {
        if (property_name) free(property_name);
        int64_t result = -1;
        if (r_return) constructors.variant_from_int_constructor(r_return, &result);
        return;
    }
    
    entry->active = true;
    entry->type = COLYSEUS_CB_ON_ADD;
    entry->property = property_name;
    entry->ref_id = (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) ? colyseus_extract_ref_id(p_args[0]) : 0;
    entry->field_type = COLYSEUS_FIELD_MAP;  // Default for collections
    entry->item_vtable = NULL;
    
    // Look up field info from instance's vtable
    if (instance && property_name) {
        colyseus_schema_t* schema = (colyseus_schema_t*)instance;
        if (schema->__vtable) {
            if (colyseus_vtable_is_dynamic(schema->__vtable)) {
                // Dynamic vtable
                const colyseus_dynamic_vtable_t* dyn_vtable = colyseus_vtable_as_dynamic(schema->__vtable);
                const colyseus_dynamic_field_t* dyn_field = colyseus_dynamic_vtable_find_field_by_name(dyn_vtable, property_name);
                if (dyn_field) {
                    entry->field_type = dyn_field->type;
                    entry->item_vtable = dyn_field->child_vtable ? &dyn_field->child_vtable->base : NULL;
                }
            } else {
                // Static vtable
                for (int i = 0; i < schema->__vtable->field_count; i++) {
                    if (schema->__vtable->fields[i].name && strcmp(schema->__vtable->fields[i].name, property_name) == 0) {
                        entry->field_type = schema->__vtable->fields[i].type;
                        entry->item_vtable = schema->__vtable->fields[i].child_vtable;
                        break;
                    }
                }
            }
        }
    }
    
    api.variant_new_copy(&entry->callable, callable_arg);
    
    if (wrapper->native_callbacks && instance && property_name) {
        entry->handle = colyseus_callbacks_on_add(
            wrapper->native_callbacks,
            instance,
            property_name,
            item_add_trampoline,
            entry,
            true  // immediate
        );
    } else {
        entry->handle = wrapper->entry_count;
    }
    
    wrapper->entry_count++;
    
    int64_t handle = entry->handle;
    if (r_return) constructors.variant_from_int_constructor(r_return, &handle);
}

void gobuext_colyseus_callbacks_on_remove(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
) {
    (void)p_method_userdata;
    
    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
        return;
    }
    
    if (p_argument_count < 2) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
        return;
    }
    
    NativeExtensionVariantType first_arg_type = api.variant_get_type(p_args[0]);
    
    void* instance = NULL;
    char* property_name = NULL;
    NativeExtensionConstVariantPtr callable_arg = NULL;
    
    if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_STRING) {
        property_name = variant_string_to_c_str(p_args[0]);
        callable_arg = p_args[1];
        
        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room) {
            instance = colyseus_room_get_state(wrapper->room_wrapper->native_room);
        }
        
    } else if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) {
        if (p_argument_count < 3) {
            if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
            return;
        }
        
        int ref_id = colyseus_extract_ref_id(p_args[0]);
        property_name = variant_string_to_c_str(p_args[1]);
        callable_arg = p_args[2];
        
        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room &&
            wrapper->room_wrapper->native_room->serializer &&
            wrapper->room_wrapper->native_room->serializer->decoder) {
            
            colyseus_ref_tracker_t* refs = 
                wrapper->room_wrapper->native_room->serializer->decoder->refs;
            if (refs) {
                instance = colyseus_ref_tracker_get(refs, ref_id);
            }
        }
    } else {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INVALID_ARGUMENT;
        return;
    }
    
    GobuCallbackEntry* entry = find_free_entry(wrapper);
    if (!entry) {
        if (property_name) free(property_name);
        int64_t result = -1;
        if (r_return) constructors.variant_from_int_constructor(r_return, &result);
        return;
    }
    
    entry->active = true;
    entry->type = COLYSEUS_CB_ON_REMOVE;
    entry->property = property_name;
    entry->ref_id = (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) ? colyseus_extract_ref_id(p_args[0]) : 0;
    entry->field_type = COLYSEUS_FIELD_MAP;  // Default for collections
    entry->item_vtable = NULL;
    
    // Look up field info from instance's vtable
    if (instance && property_name) {
        colyseus_schema_t* schema = (colyseus_schema_t*)instance;
        if (schema->__vtable) {
            if (colyseus_vtable_is_dynamic(schema->__vtable)) {
                // Dynamic vtable
                const colyseus_dynamic_vtable_t* dyn_vtable = colyseus_vtable_as_dynamic(schema->__vtable);
                const colyseus_dynamic_field_t* dyn_field = colyseus_dynamic_vtable_find_field_by_name(dyn_vtable, property_name);
                if (dyn_field) {
                    entry->field_type = dyn_field->type;
                    entry->item_vtable = dyn_field->child_vtable ? &dyn_field->child_vtable->base : NULL;
                }
            } else {
                // Static vtable
                for (int i = 0; i < schema->__vtable->field_count; i++) {
                    if (schema->__vtable->fields[i].name && strcmp(schema->__vtable->fields[i].name, property_name) == 0) {
                        entry->field_type = schema->__vtable->fields[i].type;
                        entry->item_vtable = schema->__vtable->fields[i].child_vtable;
                        break;
                    }
                }
            }
        }
    }
    
    api.variant_new_copy(&entry->callable, callable_arg);
    
    if (wrapper->native_callbacks && instance && property_name) {
        entry->handle = colyseus_callbacks_on_remove(
            wrapper->native_callbacks,
            instance,
            property_name,
            item_remove_trampoline,
            entry
        );
    } else {
        entry->handle = wrapper->entry_count;
    }
    
    wrapper->entry_count++;
    
    int64_t handle = entry->handle;
    if (r_return) constructors.variant_from_int_constructor(r_return, &handle);
}

void gobuext_colyseus_callbacks_on_change(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
) {
    (void)p_method_userdata;

    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
        return;
    }

    if (p_argument_count < 1) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
        return;
    }

    NativeExtensionVariantType first_arg_type = api.variant_get_type(p_args[0]);

    void* instance = NULL;
    char* property_name = NULL;
    NativeExtensionConstVariantPtr callable_arg = NULL;
    bool is_instance_change = false;  // true = instance on_change (no property), false = collection on_change

    if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_CALLABLE) {
        // on_change(callback) — instance change on root state
        callable_arg = p_args[0];
        is_instance_change = true;

        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room) {
            instance = colyseus_room_get_state(wrapper->room_wrapper->native_room);
        }

    } else if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_STRING) {
        // on_change("property", callback) — collection change on root state
        if (p_argument_count < 2) {
            if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
            return;
        }
        property_name = variant_string_to_c_str(p_args[0]);
        callable_arg = p_args[1];

        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room) {
            instance = colyseus_room_get_state(wrapper->room_wrapper->native_room);
        }

    } else if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) {
        if (p_argument_count < 2) {
            if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
            return;
        }

        NativeExtensionVariantType second_arg_type = api.variant_get_type(p_args[1]);
        int ref_id = colyseus_extract_ref_id(p_args[0]);

        if (second_arg_type == NATIVEEXTENSION_VARIANT_TYPE_CALLABLE) {
            // on_change(schema_dict, callback) — instance change
            callable_arg = p_args[1];
            is_instance_change = true;
        } else {
            // on_change(schema_dict, "property", callback) — collection change
            if (p_argument_count < 3) {
                if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
                return;
            }
            property_name = variant_string_to_c_str(p_args[1]);
            callable_arg = p_args[2];
        }

        if (wrapper->room_wrapper && wrapper->room_wrapper->native_room &&
            wrapper->room_wrapper->native_room->serializer &&
            wrapper->room_wrapper->native_room->serializer->decoder) {

            colyseus_ref_tracker_t* refs =
                wrapper->room_wrapper->native_room->serializer->decoder->refs;
            if (refs) {
                instance = colyseus_ref_tracker_get(refs, ref_id);
            }
        }

    } else {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INVALID_ARGUMENT;
        return;
    }

    GobuCallbackEntry* entry = find_free_entry(wrapper);
    if (!entry) {
        if (property_name) free(property_name);
        int64_t result = -1;
        if (r_return) constructors.variant_from_int_constructor(r_return, &result);
        return;
    }

    entry->active = true;
    entry->type = COLYSEUS_CB_ON_CHANGE;
    entry->property = property_name;
    entry->ref_id = 0;
    entry->field_type = COLYSEUS_FIELD_MAP;  // Default for collections
    entry->item_vtable = NULL;

    if (first_arg_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY) {
        entry->ref_id = colyseus_extract_ref_id(p_args[0]);
    }

    // Look up field info for collection changes
    if (!is_instance_change && instance && property_name) {
        colyseus_schema_t* schema = (colyseus_schema_t*)instance;
        if (schema->__vtable) {
            if (colyseus_vtable_is_dynamic(schema->__vtable)) {
                const colyseus_dynamic_vtable_t* dyn_vtable = colyseus_vtable_as_dynamic(schema->__vtable);
                const colyseus_dynamic_field_t* dyn_field = colyseus_dynamic_vtable_find_field_by_name(dyn_vtable, property_name);
                if (dyn_field) {
                    entry->field_type = dyn_field->type;
                    entry->item_vtable = dyn_field->child_vtable ? &dyn_field->child_vtable->base : NULL;
                }
            } else {
                for (int i = 0; i < schema->__vtable->field_count; i++) {
                    if (schema->__vtable->fields[i].name && strcmp(schema->__vtable->fields[i].name, property_name) == 0) {
                        entry->field_type = schema->__vtable->fields[i].type;
                        entry->item_vtable = schema->__vtable->fields[i].child_vtable;
                        break;
                    }
                }
            }
        }
    }

    api.variant_new_copy(&entry->callable, callable_arg);

    if (wrapper->native_callbacks && instance) {
        if (is_instance_change) {
            entry->handle = colyseus_callbacks_on_change_instance(
                wrapper->native_callbacks,
                instance,
                instance_change_trampoline,
                entry
            );
        } else if (property_name) {
            entry->handle = colyseus_callbacks_on_change_collection(
                wrapper->native_callbacks,
                instance,
                property_name,
                collection_change_trampoline,
                entry
            );
        } else {
            entry->handle = wrapper->entry_count;
        }
    } else {
        entry->handle = wrapper->entry_count;
    }

    wrapper->entry_count++;

    int64_t handle = entry->handle;
    if (r_return) constructors.variant_from_int_constructor(r_return, &handle);
}

void gobuext_colyseus_callbacks_remove(
    void* p_method_userdata,
    NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args,
    NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return,
    NativeExtensionCallError* r_error
) {
    (void)p_method_userdata;
    (void)r_return;
    
    ColyseusCallbacksWrapper* wrapper = (ColyseusCallbacksWrapper*)p_instance;
    if (!wrapper) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
        return;
    }
    
    if (p_argument_count < 1) {
        if (r_error) r_error->error = NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
        return;
    }
    
    int64_t handle = variant_to_int64(p_args[0]);
    
    GobuCallbackEntry* entry = find_entry_by_handle(wrapper, (int)handle);
    if (entry) {
        // Remove from native callbacks
        if (wrapper->native_callbacks) {
            colyseus_callbacks_remove(wrapper->native_callbacks, entry->handle);
        }
        
        // Clean up entry
        if (entry->property) {
            free(entry->property);
            entry->property = NULL;
        }
        destructors.variant_destroy(&entry->callable);
        entry->active = false;
    }
}
