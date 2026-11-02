#ifndef gobuext_colyseus_H
#define gobuext_colyseus_H

#include <nativeextension_interface.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <colyseus/client.h>

/*
 * Gobu loads the binding through its plugin interface (gobu_plugin.h), with
 * NativeExtension implemented on top of it (gobu_compat.c). There every
 * engine value the binding holds (String, StringName, Dictionary, Array,
 * PackedByteArray) is a Variant holding it, so all have a Variant's size.
 */
#define GOBUEXT_COLYSEUS_VARIANT_SIZE 24
#define GOBUEXT_COLYSEUS_STRING_NAME_SIZE GOBUEXT_COLYSEUS_VARIANT_SIZE
#define GOBUEXT_COLYSEUS_STRING_SIZE GOBUEXT_COLYSEUS_VARIANT_SIZE
#define GOBUEXT_COLYSEUS_PACKED_BYTE_ARRAY_SIZE GOBUEXT_COLYSEUS_VARIANT_SIZE
#define GOBUEXT_COLYSEUS_DICTIONARY_SIZE GOBUEXT_COLYSEUS_VARIANT_SIZE
#define GOBUEXT_COLYSEUS_ARRAY_SIZE GOBUEXT_COLYSEUS_VARIANT_SIZE

// Opaque types
#define GOBUEXT_COLYSEUS_VARIANT_WORDS (GOBUEXT_COLYSEUS_VARIANT_SIZE / sizeof(uintptr_t))
#define GOBUEXT_COLYSEUS_STRING_WORDS (GOBUEXT_COLYSEUS_STRING_SIZE / sizeof(uintptr_t))
#define GOBUEXT_COLYSEUS_STRING_NAME_WORDS (GOBUEXT_COLYSEUS_STRING_NAME_SIZE / sizeof(uintptr_t))
#define GOBUEXT_COLYSEUS_PACKED_BYTE_ARRAY_WORDS (GOBUEXT_COLYSEUS_PACKED_BYTE_ARRAY_SIZE / sizeof(uintptr_t))
#define GOBUEXT_COLYSEUS_DICTIONARY_WORDS (GOBUEXT_COLYSEUS_DICTIONARY_SIZE / sizeof(uintptr_t))
#define GOBUEXT_COLYSEUS_ARRAY_WORDS (GOBUEXT_COLYSEUS_ARRAY_SIZE / sizeof(uintptr_t))

_Static_assert(GOBUEXT_COLYSEUS_VARIANT_SIZE % sizeof(uintptr_t) == 0, "Variant size must be word-aligned");
_Static_assert(GOBUEXT_COLYSEUS_STRING_SIZE % sizeof(uintptr_t) == 0, "String size must be word-aligned");
_Static_assert(GOBUEXT_COLYSEUS_STRING_NAME_SIZE % sizeof(uintptr_t) == 0, "StringName size must be word-aligned");
_Static_assert(GOBUEXT_COLYSEUS_PACKED_BYTE_ARRAY_SIZE % sizeof(uintptr_t) == 0, "PackedByteArray size must be word-aligned");
_Static_assert(GOBUEXT_COLYSEUS_DICTIONARY_SIZE % sizeof(uintptr_t) == 0, "Dictionary size must be word-aligned");
_Static_assert(GOBUEXT_COLYSEUS_ARRAY_SIZE % sizeof(uintptr_t) == 0, "Array size must be word-aligned");

typedef union {
    uint8_t data[GOBUEXT_COLYSEUS_STRING_NAME_SIZE];
    uintptr_t words[GOBUEXT_COLYSEUS_STRING_NAME_WORDS];
} StringName;

typedef union {
    uint8_t data[GOBUEXT_COLYSEUS_STRING_SIZE];
    uintptr_t words[GOBUEXT_COLYSEUS_STRING_WORDS];
} String;

typedef union {
    uint8_t data[GOBUEXT_COLYSEUS_PACKED_BYTE_ARRAY_SIZE];
    uintptr_t words[GOBUEXT_COLYSEUS_PACKED_BYTE_ARRAY_WORDS];
    uint64_t align_u64;
    void *align_ptr;
} PackedByteArray;

typedef union {
    uint8_t data[GOBUEXT_COLYSEUS_DICTIONARY_SIZE];
    uintptr_t words[GOBUEXT_COLYSEUS_DICTIONARY_WORDS];
} Dictionary;

typedef union {
    uint8_t data[GOBUEXT_COLYSEUS_ARRAY_SIZE];
    uintptr_t words[GOBUEXT_COLYSEUS_ARRAY_WORDS];
} Array;

typedef union {
    uint8_t data[GOBUEXT_COLYSEUS_VARIANT_SIZE];
    uintptr_t words[GOBUEXT_COLYSEUS_VARIANT_WORDS];
    uint64_t align_u64;
    double align_f64;
    void *align_ptr;
} Variant;

typedef Variant NativeExtensionValueStorage;

// Enums.
typedef enum
{
    PROPERTY_HINT_NONE = 0,
} PropertyHint;

typedef enum
{
    PROPERTY_USAGE_NONE = 0,
    PROPERTY_USAGE_STORAGE = 2,
    PROPERTY_USAGE_EDITOR = 4,
    PROPERTY_USAGE_DEFAULT = PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR,
    PROPERTY_USAGE_NIL_IS_VARIANT = 1 << 17,  // 131072 - indicates NIL type means "any Variant"
} PropertyUsageFlags;

// Forward declarations
typedef struct ColyseusClient ColyseusClient;
typedef struct ColyseusRoom ColyseusRoom;

// Constructors struct
struct Constructors
{
    NativeExtensionInterfaceStringNameNewWithLatin1Chars string_name_new_with_latin1_chars;
    NativeExtensionInterfaceStringNewWithUtf8Chars string_new_with_utf8_chars;
    NativeExtensionInterfaceStringNewWithUtf8CharsAndLen string_new_with_utf8_chars_and_len;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_string_constructor;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_bool_constructor;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_object_constructor;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_packed_byte_array_constructor;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_int_constructor;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_float_constructor;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_dictionary_constructor;
    NativeExtensionVariantFromTypeConstructorFunc variant_from_array_constructor;
    NativeExtensionTypeFromVariantConstructorFunc string_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc bool_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc dictionary_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc array_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc object_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc int_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc float_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc string_name_from_variant_constructor;
    NativeExtensionTypeFromVariantConstructorFunc packed_byte_array_from_variant_constructor;
    NativeExtensionPtrConstructor packed_byte_array_constructor;
    NativeExtensionPtrConstructor dictionary_constructor;
    NativeExtensionPtrConstructor array_constructor;
    NativeExtensionPtrConstructor string_from_string_name_constructor;  // String(StringName) constructor
};

// Destructors struct
struct Destructors
{
    NativeExtensionPtrDestructor string_name_destructor;
    NativeExtensionPtrDestructor string_destructor;
    NativeExtensionPtrDestructor dictionary_destructor;
    NativeExtensionPtrDestructor array_destructor;
    NativeExtensionPtrDestructor packed_byte_array_destructor;
    NativeExtensionInterfaceVariantDestroy variant_destroy;
};

// Global instances (defined in plugin.c)
extern struct Constructors constructors;
extern struct Destructors destructors;

// NativeExtension interface struct - holds function pointers
typedef struct {
    NativeExtensionInterfaceClassdbRegisterExtensionClass2 classdb_register_extension_class2;
    NativeExtensionInterfaceClassdbRegisterExtensionClass4 classdb_register_extension_class4;
    NativeExtensionInterfaceClassdbRegisterExtensionClass5 classdb_register_extension_class5;
    NativeExtensionInterfaceClassdbRegisterExtensionClassMethod classdb_register_extension_class_method;
    NativeExtensionInterfaceClassdbRegisterExtensionClassSignal classdb_register_extension_class_signal;
    NativeExtensionInterfaceClassdbConstructObject classdb_construct_object;
    NativeExtensionInterfaceObjectSetInstance object_set_instance;
    NativeExtensionInterfaceObjectSetInstanceBinding object_set_instance_binding;
    NativeExtensionInterfaceObjectGetInstanceBinding object_get_instance_binding;
    NativeExtensionInterfaceMemAlloc mem_alloc;
    NativeExtensionInterfaceMemFree mem_free;
    NativeExtensionInterfaceVariantGetPtrDestructor variant_get_ptr_destructor;
    NativeExtensionInterfaceStringNewWithUtf8CharsAndLen string_new_with_utf8_chars_and_len;
    NativeExtensionInterfaceStringToUtf8Chars string_to_utf8_chars;
    NativeExtensionInterfaceVariantCall variant_call;
    NativeExtensionInterfacePackedByteArrayOperatorIndex packed_byte_array_operator_index;
    NativeExtensionInterfacePackedByteArrayOperatorIndexConst packed_byte_array_operator_index_const;
    NativeExtensionInterfaceRefSetObject ref_set_object;
    NativeExtensionInterfaceDictionaryOperatorIndex dictionary_operator_index;
    NativeExtensionInterfaceArrayOperatorIndex array_operator_index;
    NativeExtensionInterfaceArrayOperatorIndexConst array_operator_index_const;
    NativeExtensionInterfaceVariantGetType variant_get_type;
    NativeExtensionInterfaceVariantSetKeyed variant_set_keyed;
    NativeExtensionInterfaceVariantSetIndexed variant_set_indexed;
    NativeExtensionInterfaceVariantNewNil variant_new_nil;
    NativeExtensionInterfaceObjectGetInstanceId object_get_instance_id;
    NativeExtensionInterfaceObjectGetInstanceFromId object_get_instance_from_id;
    NativeExtensionInterfaceCallableCustomCreate callable_custom_create;
    NativeExtensionInterfaceVariantNewCopy variant_new_copy;
    NativeExtensionInterfaceVariantStringify variant_stringify;
} NativeExtensionInterface;

// NativeExtension interface globals
extern NativeExtensionInterface api;
extern NativeExtensionClassLibraryPtr class_library;

static inline void gobuext_variant_new_nil(Variant *variant) {
    if (api.variant_new_nil) {
        api.variant_new_nil(variant);
    } else {
        memset(variant, 0, sizeof(*variant));
    }
}

static inline void gobuext_variant_assign(Variant *dst, NativeExtensionConstVariantPtr src) {
    destructors.variant_destroy(dst);
    api.variant_new_copy(dst, src);
}

static inline void gobuext_variant_move_assign(Variant *dst, Variant *src) {
    gobuext_variant_assign(dst, src);
    destructors.variant_destroy(src);
    gobuext_variant_new_nil(src);
}

static inline void gobuext_variant_set_keyed(Variant *container, NativeExtensionConstVariantPtr key, Variant *value) {
    NativeExtensionBool valid = 0;
    api.variant_set_keyed(container, key, value, &valid);
}

static inline void gobuext_variant_set_indexed(Variant *container, NativeExtensionInt index, Variant *value) {
    NativeExtensionBool valid = 0;
    NativeExtensionBool oob = 0;
    api.variant_set_indexed(container, index, value, &valid, &oob);
}

// ColyseusClientWrapper type
typedef struct ColyseusClientWrapper {
    colyseus_client_t* native_client;
    NativeExtensionObjectPtr gobu_object;
    volatile int pending_http_requests;  // Track in-flight HTTP requests
} ColyseusClientWrapper;

// A schema definition of Colyseus.schema() (colyseus_schema_definition.h)
typedef struct colyseus_schema_definition colyseus_schema_definition_t;

// ColyseusRoomWrapper type (forward declaration)
typedef struct {
    colyseus_room_t* native_room;
    NativeExtensionObjectPtr gobu_object;
    const colyseus_schema_vtable_t* pending_vtable;  // Vtable to set when native room is assigned
    colyseus_schema_definition_t* schema_definition;  // The one of set_state_type(), if any
} ColyseusRoomWrapper;

// ColyseusClient methods
NativeExtensionObjectPtr gobuext_colyseus_client_constructor(void* p_class_userdata);
void gobuext_colyseus_client_destructor(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance);
void gobuext_colyseus_client_reference(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance);
void gobuext_colyseus_client_unreference(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance);
void gobuext_colyseus_client_set_endpoint(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
// Matchmaking methods (call + ptrcall versions)
void gobuext_colyseus_client_join_or_create(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_join_or_create_ptrcall(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_client_create(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_create_ptrcall(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_client_join(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_join_ptrcall(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_client_join_by_id(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_join_by_id_ptrcall(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_client_reconnect(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_reconnect_ptrcall(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);

// Simple getter (reference implementation style)
const char* gobuext_colyseus_client_get_endpoint(const ColyseusClientWrapper* self);

// NativeExtension wrappers for get_endpoint (call and ptrcall versions)
void gobuext_colyseus_client_get_endpoint_ptrcall(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_client_get_endpoint_call(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);

// ColyseusClient HTTP methods (vararg call convention)
void gobuext_colyseus_client_http_get(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_http_post(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_http_put(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_http_delete(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_client_http_patch(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);

// ColyseusClient auth methods
void gobuext_colyseus_client_auth_set_token(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_client_auth_get_token(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);

// ColyseusRoom methods
NativeExtensionObjectPtr gobuext_colyseus_room_constructor(void* p_class_userdata);
ColyseusRoomWrapper* gobuext_colyseus_room_get_last_wrapper(void);
ColyseusRoomWrapper* gobuext_colyseus_room_get_wrapper_by_id(NativeExtensionObjectInstanceID instance_id);
void gobuext_colyseus_room_destructor(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance);
void gobuext_colyseus_room_reference(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance);
void gobuext_colyseus_room_unreference(void* p_class_userdata, NativeExtensionClassInstancePtr p_instance);
void gobuext_colyseus_room_send_message(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_room_send_message_int(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);
void gobuext_colyseus_room_leave(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_room_get_id(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_room_get_session_id(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_room_get_reconnection_token(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_room_get_name(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_room_is_connected(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_room_is_reconnecting(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);
void gobuext_colyseus_room_set_reconnection_options(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);

// get_state() - returns current state as Dictionary (uses ptrcall signature like other methods)
void gobuext_colyseus_room_get_state(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret);

// set_state_type(type) - sets the schema vtable for state decoding
// Accepts either:
//   - String: a vtable registered by name
//   - Dictionary or Array: a schema definition of Colyseus.schema() (colyseus.gro)
void gobuext_colyseus_room_set_state_type(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error);

#endif // gobuext_colyseus_H
