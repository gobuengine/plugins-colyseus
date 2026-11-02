#include "gobu_compat.h"
#include "gobu_colyseus.h"
#include "colyseus_callbacks.h"
#include "colyseus_schema_registry.h"
#include "tls_certificates.h"
#include <nativeextension_interface.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Global NativeExtension interface and library
NativeExtensionInterface api = {0};
NativeExtensionClassLibraryPtr class_library = NULL;

// Global constructors and destructors
struct Constructors constructors = {0};
struct Destructors destructors = {0};

static void register_extension_class(
    NativeExtensionConstStringNamePtr class_name,
    NativeExtensionConstStringNamePtr parent_class_name,
    NativeExtensionClassCreateInstance create_instance,
    NativeExtensionClassCreateInstance2 create_instance2,
    NativeExtensionClassFreeInstance free_instance
) {
    if (api.classdb_register_extension_class5) {
        NativeExtensionClassCreationInfo5 class_info = {
            .is_virtual = false,
            .is_abstract = false,
            .is_exposed = true,
            .is_runtime = false,
            .icon_path = NULL,
            .set_func = NULL,
            .get_func = NULL,
            .get_property_list_func = NULL,
            .free_property_list_func = NULL,
            .property_can_revert_func = NULL,
            .property_get_revert_func = NULL,
            .validate_property_func = NULL,
            .notification_func = NULL,
            .to_string_func = NULL,
            .reference_func = NULL,
            .unreference_func = NULL,
            .create_instance_func = create_instance2,
            .free_instance_func = free_instance,
            .recreate_instance_func = NULL,
            .get_virtual_func = NULL,
            .get_virtual_call_data_func = NULL,
            .call_virtual_with_data_func = NULL,
            .class_userdata = NULL,
        };
        api.classdb_register_extension_class5(class_library, class_name, parent_class_name, &class_info);
        return;
    }

    if (api.classdb_register_extension_class4) {
        NativeExtensionClassCreationInfo4 class_info = {
            .is_virtual = false,
            .is_abstract = false,
            .is_exposed = true,
            .is_runtime = false,
            .icon_path = NULL,
            .set_func = NULL,
            .get_func = NULL,
            .get_property_list_func = NULL,
            .free_property_list_func = NULL,
            .property_can_revert_func = NULL,
            .property_get_revert_func = NULL,
            .validate_property_func = NULL,
            .notification_func = NULL,
            .to_string_func = NULL,
            .reference_func = NULL,
            .unreference_func = NULL,
            .create_instance_func = create_instance2,
            .free_instance_func = free_instance,
            .recreate_instance_func = NULL,
            .get_virtual_func = NULL,
            .get_virtual_call_data_func = NULL,
            .call_virtual_with_data_func = NULL,
            .class_userdata = NULL,
        };
        api.classdb_register_extension_class4(class_library, class_name, parent_class_name, &class_info);
        return;
    }

    {
        NativeExtensionClassCreationInfo2 class_info = {
            .is_virtual = false,
            .is_abstract = false,
            .is_exposed = true,
            .set_func = NULL,
            .get_func = NULL,
            .get_property_list_func = NULL,
            .free_property_list_func = NULL,
            .property_can_revert_func = NULL,
            .property_get_revert_func = NULL,
            .validate_property_func = NULL,
            .notification_func = NULL,
            .to_string_func = NULL,
            .reference_func = NULL,
            .unreference_func = NULL,
            .create_instance_func = create_instance,
            .free_instance_func = free_instance,
            .recreate_instance_func = NULL,
            .get_virtual_func = NULL,
            .get_virtual_call_data_func = NULL,
            .call_virtual_with_data_func = NULL,
            .get_rid_func = NULL,
            .class_userdata = NULL,
        };
        api.classdb_register_extension_class2(class_library, class_name, parent_class_name, &class_info);
    }
}

NativeExtensionPropertyInfo make_property_full(
    NativeExtensionVariantType type,
    const char *name,
    uint32_t hint,
    const char *hint_string,
    const char *class_name,
    uint32_t usage_flags)
{

    StringName *prop_name = api.mem_alloc(sizeof(StringName));
    constructors.string_name_new_with_latin1_chars(prop_name, name, false);
    String *prop_hint_string = api.mem_alloc(sizeof(String));
    constructors.string_new_with_utf8_chars(prop_hint_string, hint_string);
    StringName *prop_class_name = api.mem_alloc(sizeof(StringName));
    constructors.string_name_new_with_latin1_chars(prop_class_name, class_name, false);

    NativeExtensionPropertyInfo info = {
        .name = prop_name,
        .type = type,
        .hint = hint,
        .hint_string = prop_hint_string,
        .class_name = prop_class_name,
        .usage = usage_flags,
    };

    return info;
}

// Helper function to create property info
NativeExtensionPropertyInfo make_property(
    NativeExtensionVariantType type,
    const char *name)
{
    return make_property_full(type, name, PROPERTY_HINT_NONE, "", "", PROPERTY_USAGE_DEFAULT);
}


// Helper function to destruct property info
void destruct_property(NativeExtensionPropertyInfo *info)
{
    destructors.string_name_destructor(info->name);
    destructors.string_destructor(info->hint_string);
    destructors.string_name_destructor(info->class_name);
    api.mem_free(info->name);
    api.mem_free(info->hint_string);
    api.mem_free(info->class_name);
}

// =============================================================================
// Generic call/ptrcall implementations
//
// The ptrcall variants are used when Gobu knows the exact types at call time.
// The call variants are used from scripts (variant path) — they must convert
// Variant args to raw types before forwarding to the ptrcall-signature function.
// =============================================================================

typedef void (*ptrcall_fn)(void*, NativeExtensionClassInstancePtr, const NativeExtensionConstTypePtr*, NativeExtensionTypePtr);

// 0 arguments, no return
static void call_0_args_no_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error)
{
    (void)p_args; (void)p_argument_count; (void)r_return; (void)r_error;
    ((ptrcall_fn)method_userdata)(method_userdata, p_instance, NULL, NULL);
}

static void ptrcall_0_args_no_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret)
{
    ((ptrcall_fn)method_userdata)(method_userdata, p_instance, p_args, r_ret);
}

// Method info struct for methods with return values — stores function + return type
typedef struct {
    ptrcall_fn function;
    NativeExtensionVariantType return_type;
} method_with_ret_info_t;

#define MAX_METHOD_INFOS 32
static method_with_ret_info_t g_method_infos[MAX_METHOD_INFOS];
static int g_method_info_count = 0;

static method_with_ret_info_t* alloc_method_info(ptrcall_fn fn, NativeExtensionVariantType ret_type) {
    if (g_method_info_count >= MAX_METHOD_INFOS) return NULL;
    method_with_ret_info_t* info = &g_method_infos[g_method_info_count++];
    info->function = fn;
    info->return_type = ret_type;
    return info;
}

static NativeExtensionTypeFromVariantConstructorFunc get_from_variant_constructor(NativeExtensionVariantType type) {
    switch (type) {
        case NATIVEEXTENSION_VARIANT_TYPE_STRING:
            return constructors.string_from_variant_constructor;
        case NATIVEEXTENSION_VARIANT_TYPE_BOOL:
            return constructors.bool_from_variant_constructor;
        case NATIVEEXTENSION_VARIANT_TYPE_INT:
            return constructors.int_from_variant_constructor;
        case NATIVEEXTENSION_VARIANT_TYPE_FLOAT:
            return constructors.float_from_variant_constructor;
        case NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY:
            return constructors.dictionary_from_variant_constructor;
        case NATIVEEXTENSION_VARIANT_TYPE_ARRAY:
            return constructors.array_from_variant_constructor;
        case NATIVEEXTENSION_VARIANT_TYPE_OBJECT:
            return constructors.object_from_variant_constructor;
        default:
            return NULL;
    }
}

static void cleanup_extracted_value(NativeExtensionVariantType type, NativeExtensionTypePtr value) {
    switch (type) {
        case NATIVEEXTENSION_VARIANT_TYPE_STRING:
            destructors.string_destructor(value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY:
            destructors.dictionary_destructor(value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_ARRAY:
            destructors.array_destructor(value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY:
            destructors.packed_byte_array_destructor(value);
            break;
        default:
            break;
    }
}

static void convert_ptrcall_return_to_variant(
    NativeExtensionVariantType return_type,
    NativeExtensionTypePtr value,
    NativeExtensionVariantPtr r_return
) {
    switch (return_type) {
        case NATIVEEXTENSION_VARIANT_TYPE_STRING:
            constructors.variant_from_string_constructor(r_return, value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_BOOL:
            constructors.variant_from_bool_constructor(r_return, value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_INT:
            constructors.variant_from_int_constructor(r_return, value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_FLOAT:
            constructors.variant_from_float_constructor(r_return, value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY:
            constructors.variant_from_dictionary_constructor(r_return, value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_ARRAY:
            constructors.variant_from_array_constructor(r_return, value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY:
            constructors.variant_from_packed_byte_array_constructor(r_return, value);
            break;
        case NATIVEEXTENSION_VARIANT_TYPE_OBJECT:
            constructors.variant_from_object_constructor(r_return, value);
            break;
        default:
            gobuext_variant_new_nil((Variant *)r_return);
            break;
    }
}

static void cleanup_ptrcall_return_value(NativeExtensionVariantType return_type, NativeExtensionTypePtr value) {
    cleanup_extracted_value(return_type, value);
}

static NativeExtensionObjectPtr gobuext_colyseus_client_constructor2(void *p_class_userdata, NativeExtensionBool p_notify_postinitialize) {
    (void)p_notify_postinitialize;
    return gobuext_colyseus_client_constructor(p_class_userdata);
}

static NativeExtensionObjectPtr gobuext_colyseus_room_constructor2(void *p_class_userdata, NativeExtensionBool p_notify_postinitialize) {
    (void)p_notify_postinitialize;
    return gobuext_colyseus_room_constructor(p_class_userdata);
}

static NativeExtensionObjectPtr gobuext_colyseus_callbacks_constructor2(void *p_class_userdata, NativeExtensionBool p_notify_postinitialize) {
    (void)p_notify_postinitialize;
    return gobuext_colyseus_callbacks_constructor(p_class_userdata);
}

// 0 arguments, with return
static void call_0_args_with_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error)
{
    (void)p_args; (void)p_argument_count; (void)r_error;
    method_with_ret_info_t* info = (method_with_ret_info_t*)method_userdata;

    NativeExtensionValueStorage ret_buf = {0};
    info->function(method_userdata, p_instance, NULL, (NativeExtensionTypePtr)&ret_buf);

    if (r_return) {
        convert_ptrcall_return_to_variant(info->return_type, (NativeExtensionTypePtr)&ret_buf, r_return);
    }

    cleanup_ptrcall_return_value(info->return_type, (NativeExtensionTypePtr)&ret_buf);
}

static void ptrcall_0_args_with_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret)
{
    method_with_ret_info_t* info = (method_with_ret_info_t*)method_userdata;
    info->function(method_userdata, p_instance, p_args, r_ret);
}

// 1 argument, no return — extract raw value from Variant arg before calling ptrcall
static void call_1_arg_no_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error)
{
    (void)p_argument_count; (void)r_return; (void)r_error;
    NativeExtensionVariantType arg_type = api.variant_get_type(p_args[0]);
    NativeExtensionValueStorage arg_buf = {0};
    NativeExtensionTypeFromVariantConstructorFunc from_variant = get_from_variant_constructor(arg_type);

    NativeExtensionConstTypePtr raw_args[1];
    if (from_variant) {
        from_variant((NativeExtensionTypePtr)&arg_buf, (NativeExtensionVariantPtr)p_args[0]);
        raw_args[0] = (NativeExtensionConstTypePtr)&arg_buf;
    } else {
        raw_args[0] = (NativeExtensionConstTypePtr)p_args[0];
    }

    ((ptrcall_fn)method_userdata)(method_userdata, p_instance, raw_args, NULL);

    if (from_variant) {
        cleanup_extracted_value(arg_type, (NativeExtensionTypePtr)&arg_buf);
    }
}

static void ptrcall_1_arg_no_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret)
{
    ((ptrcall_fn)method_userdata)(method_userdata, p_instance, p_args, r_ret);
}

// 2 arguments, no return
static void call_2_args_no_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error)
{
    (void)p_argument_count; (void)r_return; (void)r_error;
    NativeExtensionValueStorage arg_bufs[2] = {0};
    NativeExtensionConstTypePtr raw_args[2];
    NativeExtensionTypeFromVariantConstructorFunc from_variants[2] = {0};

    for (int i = 0; i < 2; i++) {
        NativeExtensionVariantType t = api.variant_get_type(p_args[i]);
        from_variants[i] = get_from_variant_constructor(t);
        if (from_variants[i]) {
            from_variants[i]((NativeExtensionTypePtr)&arg_bufs[i], (NativeExtensionVariantPtr)p_args[i]);
            raw_args[i] = (NativeExtensionConstTypePtr)&arg_bufs[i];
        } else {
            raw_args[i] = (NativeExtensionConstTypePtr)p_args[i];
        }
    }

    ((ptrcall_fn)method_userdata)(method_userdata, p_instance, raw_args, NULL);

    for (int i = 0; i < 2; i++) {
        if (from_variants[i]) {
            cleanup_extracted_value(api.variant_get_type(p_args[i]), (NativeExtensionTypePtr)&arg_bufs[i]);
        }
    }
}

static void ptrcall_2_args_no_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret)
{
    ((ptrcall_fn)method_userdata)(method_userdata, p_instance, p_args, r_ret);
}

// Generic method binding: 0 arguments, no return
static void bind_method_0_no_ret(
    const char *class_name,
    const char *method_name,
    void *function)
{
    StringName method_name_string;
    constructors.string_name_new_with_latin1_chars(&method_name_string, method_name, false);

    NativeExtensionClassMethodCall call_func = call_0_args_no_ret;
    NativeExtensionClassMethodPtrCall ptrcall_func = ptrcall_0_args_no_ret;

    NativeExtensionClassMethodInfo method_info = {
        .name = &method_name_string,
        .method_userdata = function,
        .call_func = call_func,
        .ptrcall_func = ptrcall_func,
        .method_flags = NATIVEEXTENSION_METHOD_FLAGS_DEFAULT,
        .has_return_value = false,
        .argument_count = 0,
    };

    StringName class_name_string;
    constructors.string_name_new_with_latin1_chars(&class_name_string, class_name, false);

    api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

    // Destruct things
    destructors.string_name_destructor(&method_name_string);
    destructors.string_name_destructor(&class_name_string);
}

// Generic method binding: 0 arguments, with return
static void bind_method_0_with_ret(
    const char *class_name,
    const char *method_name,
    void *function,
    NativeExtensionVariantType return_type)
{
    StringName method_name_string;
    constructors.string_name_new_with_latin1_chars(&method_name_string, method_name, false);

    NativeExtensionClassMethodCall call_func = call_0_args_with_ret;
    NativeExtensionClassMethodPtrCall ptrcall_func = ptrcall_0_args_with_ret;

    NativeExtensionPropertyInfo return_info = make_property(return_type, "");

    method_with_ret_info_t* info = alloc_method_info((ptrcall_fn)function, return_type);

    NativeExtensionClassMethodInfo method_info = {
        .name = &method_name_string,
        .method_userdata = info,
        .call_func = call_func,
        .ptrcall_func = ptrcall_func,
        .method_flags = NATIVEEXTENSION_METHOD_FLAGS_DEFAULT,
        .has_return_value = true,
        .return_value_info = &return_info,
        .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        .argument_count = 0,
    };

    StringName class_name_string;
    constructors.string_name_new_with_latin1_chars(&class_name_string, class_name, false);

    api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

    destructors.string_name_destructor(&method_name_string);
    destructors.string_name_destructor(&class_name_string);
    destruct_property(&return_info);
}

// Generic method binding: 1 argument, no return
static void bind_method_1_no_ret(
    const char *class_name,
    const char *method_name,
    void *function,
    const char *arg1_name,
    NativeExtensionVariantType arg1_type)
{
    StringName method_name_string;
    constructors.string_name_new_with_latin1_chars(&method_name_string, method_name, false);

    NativeExtensionPropertyInfo args_info[] = {
        make_property(arg1_type, arg1_name),
    };
    NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
        NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
    };

    NativeExtensionClassMethodInfo method_info = {
        .name = &method_name_string,
        .method_userdata = function,
        .call_func = call_1_arg_no_ret,
        .ptrcall_func = ptrcall_1_arg_no_ret,
        .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
        .has_return_value = false,
        .return_value_info = NULL,
        .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        .argument_count = 1,
        .arguments_info = args_info,
        .arguments_metadata = args_metadata,
        .default_argument_count = 0,
        .default_arguments = NULL
    };

    StringName class_name_string;
    constructors.string_name_new_with_latin1_chars(&class_name_string, class_name, false);

    api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

    destructors.string_name_destructor(&method_name_string);
    destructors.string_name_destructor(&class_name_string);
    destruct_property(&args_info[0]);
}

// 1 argument, with return — extract arg from Variant, call ptrcall, convert return to Variant
static void call_1_arg_with_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error)
{
    (void)p_argument_count; (void)r_error;
    method_with_ret_info_t* info = (method_with_ret_info_t*)method_userdata;

    NativeExtensionVariantType arg_type = api.variant_get_type(p_args[0]);
    NativeExtensionValueStorage arg_buf = {0};
    NativeExtensionTypeFromVariantConstructorFunc from_variant = get_from_variant_constructor(arg_type);
    NativeExtensionConstTypePtr raw_args[1];
    if (from_variant) {
        from_variant((NativeExtensionTypePtr)&arg_buf, (NativeExtensionVariantPtr)p_args[0]);
        raw_args[0] = (NativeExtensionConstTypePtr)&arg_buf;
    } else {
        raw_args[0] = (NativeExtensionConstTypePtr)p_args[0];
    }

    NativeExtensionValueStorage ret_buf = {0};
    info->function(method_userdata, p_instance, raw_args, (NativeExtensionTypePtr)&ret_buf);

    if (r_return) {
        convert_ptrcall_return_to_variant(info->return_type, (NativeExtensionTypePtr)&ret_buf, r_return);
    }

    cleanup_ptrcall_return_value(info->return_type, (NativeExtensionTypePtr)&ret_buf);

    if (from_variant) {
        cleanup_extracted_value(arg_type, (NativeExtensionTypePtr)&arg_buf);
    }
}

static void ptrcall_1_arg_with_ret(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret)
{
    method_with_ret_info_t* info = (method_with_ret_info_t*)method_userdata;
    info->function(method_userdata, p_instance, p_args, r_ret);
}

// Generic method binding: 1 argument, with return
static void bind_method_1_with_ret(
    const char *class_name,
    const char *method_name,
    void *function,
    const char *arg1_name,
    NativeExtensionVariantType arg1_type,
    NativeExtensionVariantType return_type)
{
    StringName method_name_string;
    constructors.string_name_new_with_latin1_chars(&method_name_string, method_name, false);

    NativeExtensionPropertyInfo args_info[] = {
        make_property(arg1_type, arg1_name),
    };
    NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
        NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
    };

    NativeExtensionPropertyInfo return_info = make_property(return_type, "");

    method_with_ret_info_t* info = alloc_method_info((ptrcall_fn)function, return_type);

    NativeExtensionClassMethodInfo method_info = {
        .name = &method_name_string,
        .method_userdata = info,
        .call_func = call_1_arg_with_ret,
        .ptrcall_func = ptrcall_1_arg_with_ret,
        .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
        .has_return_value = true,
        .return_value_info = &return_info,
        .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        .argument_count = 1,
        .arguments_info = args_info,
        .arguments_metadata = args_metadata,
        .default_argument_count = 0,
        .default_arguments = NULL
    };

    StringName class_name_string;
    constructors.string_name_new_with_latin1_chars(&class_name_string, class_name, false);

    api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

    destructors.string_name_destructor(&method_name_string);
    destructors.string_name_destructor(&class_name_string);
    destruct_property(&args_info[0]);
    destruct_property(&return_info);
}

// Generic method binding: 2 arguments, no return
static void bind_method_2_no_ret(
    const char *class_name,
    const char *method_name,
    void *function,
    const char *arg1_name,
    NativeExtensionVariantType arg1_type,
    const char *arg2_name,
    NativeExtensionVariantType arg2_type)
{
    StringName method_name_string;
    constructors.string_name_new_with_latin1_chars(&method_name_string, method_name, false);

    NativeExtensionPropertyInfo args_info[] = {
        make_property(arg1_type, arg1_name),
        make_property(arg2_type, arg2_name),
    };
    NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
        NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
    };

    NativeExtensionClassMethodInfo method_info = {
        .name = &method_name_string,
        .method_userdata = function,
        .call_func = call_2_args_no_ret,
        .ptrcall_func = ptrcall_2_args_no_ret,
        .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
        .has_return_value = false,
        .return_value_info = NULL,
        .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        .argument_count = 2,
        .arguments_info = args_info,
        .arguments_metadata = args_metadata,
        .default_argument_count = 0,
        .default_arguments = NULL
    };

    StringName class_name_string;
    constructors.string_name_new_with_latin1_chars(&class_name_string, class_name, false);

    api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

    destructors.string_name_destructor(&method_name_string);
    destructors.string_name_destructor(&class_name_string);
    destruct_property(&args_info[0]);
    destruct_property(&args_info[1]);
}

// Signal binding helpers
static void bind_signal_0(
    const char *class_name,
    const char *signal_name)
{
    StringName class_string_name;
    constructors.string_name_new_with_latin1_chars(&class_string_name, class_name, false);
    StringName signal_string_name;
    constructors.string_name_new_with_latin1_chars(&signal_string_name, signal_name, false);

    api.classdb_register_extension_class_signal(class_library, &class_string_name, &signal_string_name, NULL, 0);

    // Destruct things.
    destructors.string_name_destructor(&class_string_name);
    destructors.string_name_destructor(&signal_string_name);
}

static void bind_signal_1(
    const char *class_name,
    const char *signal_name,
    const char *arg1_name,
    NativeExtensionVariantType arg1_type)
{
    StringName class_string_name;
    constructors.string_name_new_with_latin1_chars(&class_string_name, class_name, false);
    StringName signal_string_name;
    constructors.string_name_new_with_latin1_chars(&signal_string_name, signal_name, false);

    NativeExtensionPropertyInfo args_info[] = {
        make_property(arg1_type, arg1_name),
    };

    api.classdb_register_extension_class_signal(class_library, &class_string_name, &signal_string_name, args_info, 1);

    // Destruct things.
    destructors.string_name_destructor(&class_string_name);
    destructors.string_name_destructor(&signal_string_name);
    destruct_property(&args_info[0]);
}

static void bind_signal_2(
    const char *class_name,
    const char *signal_name,
    const char *arg1_name,
    NativeExtensionVariantType arg1_type,
    const char *arg2_name,
    NativeExtensionVariantType arg2_type)
{
    StringName class_string_name;
    constructors.string_name_new_with_latin1_chars(&class_string_name, class_name, false);
    StringName signal_string_name;
    constructors.string_name_new_with_latin1_chars(&signal_string_name, signal_name, false);

    NativeExtensionPropertyInfo args_info[] = {
        make_property(arg1_type, arg1_name),
        make_property(arg2_type, arg2_name),
    };

    api.classdb_register_extension_class_signal(class_library, &class_string_name, &signal_string_name, args_info, 2);

    // Destruct things.
    destructors.string_name_destructor(&class_string_name);
    destructors.string_name_destructor(&signal_string_name);
    destruct_property(&args_info[0]);
    destruct_property(&args_info[1]);
}

static void bind_signal_3(
    const char *class_name,
    const char *signal_name,
    const char *arg1_name, NativeExtensionVariantType arg1_type,
    const char *arg2_name, NativeExtensionVariantType arg2_type,
    const char *arg3_name, NativeExtensionVariantType arg3_type)
{
    StringName class_string_name;
    constructors.string_name_new_with_latin1_chars(&class_string_name, class_name, false);
    StringName signal_string_name;
    constructors.string_name_new_with_latin1_chars(&signal_string_name, signal_name, false);

    NativeExtensionPropertyInfo args_info[] = {
        make_property(arg1_type, arg1_name),
        make_property(arg2_type, arg2_name),
        make_property(arg3_type, arg3_name),
    };

    api.classdb_register_extension_class_signal(class_library, &class_string_name, &signal_string_name, args_info, 3);

    destructors.string_name_destructor(&class_string_name);
    destructors.string_name_destructor(&signal_string_name);
    destruct_property(&args_info[0]);
    destruct_property(&args_info[1]);
    destruct_property(&args_info[2]);
}

// Forward declaration for vararg call wrapper (defined later, needed by HTTP method registration)
static void call_vararg(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error);

extern void colyseus_http_poll(void);
extern void colyseus_ws_poll(void);

// Process queued events and emit signals on main thread
extern void gobuext_http_process_events(void);
extern void gobuext_room_process_events(void);

static void gobuext_colyseus_client_poll_call(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstVariantPtr* p_args, NativeExtensionInt p_argument_count,
    NativeExtensionVariantPtr r_return, NativeExtensionCallError* r_error) {
    (void)p_method_userdata; (void)p_instance; (void)p_args;
    (void)p_argument_count; (void)r_return; (void)r_error;
    colyseus_http_poll();
    colyseus_ws_poll();
    gobuext_http_process_events();
    gobuext_room_process_events();
}

static void gobuext_colyseus_client_poll_ptrcall(void* p_method_userdata, NativeExtensionClassInstancePtr p_instance,
    const NativeExtensionConstTypePtr* p_args, NativeExtensionTypePtr r_ret) {
    (void)p_method_userdata; (void)p_instance; (void)p_args; (void)r_ret;
    colyseus_http_poll();
    colyseus_ws_poll();
    gobuext_http_process_events();
    gobuext_room_process_events();
}

static void register_colyseus_client(void) {
    // Initialize StringNames
    StringName class_name;
    StringName parent_class_name;

    constructors.string_name_new_with_latin1_chars(&class_name, "_ColyseusClient", false);
    constructors.string_name_new_with_latin1_chars(&parent_class_name, "RefCounted", false);

    register_extension_class(&class_name, &parent_class_name,
        gobuext_colyseus_client_constructor,
        gobuext_colyseus_client_constructor2,
        gobuext_colyseus_client_destructor);

    destructors.string_name_destructor(&class_name);
    destructors.string_name_destructor(&parent_class_name);

    // Register set_endpoint method
    bind_method_1_no_ret(
        "_ColyseusClient",
        "set_endpoint",
        gobuext_colyseus_client_set_endpoint,
        "endpoint",
        NATIVEEXTENSION_VARIANT_TYPE_STRING
    );

    // Manual registration for get_endpoint (needs separate call/ptrcall functions)
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "get_endpoint", false);

        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_STRING, "");

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = NULL,
            .call_func = (NativeExtensionClassMethodCall)gobuext_colyseus_client_get_endpoint_call,
            .ptrcall_func = (NativeExtensionClassMethodPtrCall)gobuext_colyseus_client_get_endpoint_ptrcall,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
            .has_return_value = true,
            .return_value_info = &return_info,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 0,
            .arguments_info = NULL,
            .arguments_metadata = NULL,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusClient", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&return_info);
    }

    // Helper macro: register a matchmaking method with options (String + optional String -> returns Object)
    // The second arg (options_json) is handled as optional in the C call wrapper
    #define REGISTER_MATCHMAKING_METHOD_WITH_OPTIONS(method_name_str, arg_name_str, call_fn, ptrcall_fn) \
    { \
        StringName method_name_string; \
        constructors.string_name_new_with_latin1_chars(&method_name_string, method_name_str, false); \
        NativeExtensionPropertyInfo args_info[] = { \
            make_property(NATIVEEXTENSION_VARIANT_TYPE_STRING, arg_name_str), \
            make_property(NATIVEEXTENSION_VARIANT_TYPE_STRING, "options_json"), \
        }; \
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = { \
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE, \
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE, \
        }; \
        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_OBJECT, ""); \
        NativeExtensionClassMethodInfo method_info = { \
            .name = &method_name_string, \
            .method_userdata = NULL, \
            .call_func = (NativeExtensionClassMethodCall)(call_fn), \
            .ptrcall_func = (NativeExtensionClassMethodPtrCall)(ptrcall_fn), \
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL | NATIVEEXTENSION_METHOD_FLAG_VARARG, \
            .has_return_value = true, \
            .return_value_info = &return_info, \
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE, \
            .argument_count = 1, \
            .arguments_info = args_info, \
            .arguments_metadata = args_metadata, \
            .default_argument_count = 0, \
            .default_arguments = NULL \
        }; \
        StringName class_name_string; \
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusClient", false); \
        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info); \
        destructors.string_name_destructor(&method_name_string); \
        destructors.string_name_destructor(&class_name_string); \
        destruct_property(&args_info[0]); \
        destruct_property(&args_info[1]); \
        destruct_property(&return_info); \
    }

    // Helper macro: register a matchmaking method with 1 arg (reconnect)
    #define REGISTER_MATCHMAKING_METHOD_1(method_name_str, arg_name_str, call_fn, ptrcall_fn) \
    { \
        StringName method_name_string; \
        constructors.string_name_new_with_latin1_chars(&method_name_string, method_name_str, false); \
        NativeExtensionPropertyInfo args_info[] = { \
            make_property(NATIVEEXTENSION_VARIANT_TYPE_STRING, arg_name_str), \
        }; \
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = { \
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE, \
        }; \
        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_OBJECT, ""); \
        NativeExtensionClassMethodInfo method_info = { \
            .name = &method_name_string, \
            .method_userdata = NULL, \
            .call_func = (NativeExtensionClassMethodCall)(call_fn), \
            .ptrcall_func = (NativeExtensionClassMethodPtrCall)(ptrcall_fn), \
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL, \
            .has_return_value = true, \
            .return_value_info = &return_info, \
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE, \
            .argument_count = 1, \
            .arguments_info = args_info, \
            .arguments_metadata = args_metadata, \
            .default_argument_count = 0, \
            .default_arguments = NULL \
        }; \
        StringName class_name_string; \
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusClient", false); \
        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info); \
        destructors.string_name_destructor(&method_name_string); \
        destructors.string_name_destructor(&class_name_string); \
        destruct_property(&args_info[0]); \
        destruct_property(&return_info); \
    }

    // Register all matchmaking methods
    REGISTER_MATCHMAKING_METHOD_WITH_OPTIONS("join_or_create", "room_name", gobuext_colyseus_client_join_or_create, gobuext_colyseus_client_join_or_create_ptrcall)
    REGISTER_MATCHMAKING_METHOD_WITH_OPTIONS("create", "room_name", gobuext_colyseus_client_create, gobuext_colyseus_client_create_ptrcall)
    REGISTER_MATCHMAKING_METHOD_WITH_OPTIONS("join", "room_name", gobuext_colyseus_client_join, gobuext_colyseus_client_join_ptrcall)
    REGISTER_MATCHMAKING_METHOD_WITH_OPTIONS("join_by_id", "room_id", gobuext_colyseus_client_join_by_id, gobuext_colyseus_client_join_by_id_ptrcall)
    REGISTER_MATCHMAKING_METHOD_1("reconnect", "reconnection_token", gobuext_colyseus_client_reconnect, gobuext_colyseus_client_reconnect_ptrcall)

    #undef REGISTER_MATCHMAKING_METHOD_WITH_OPTIONS
    #undef REGISTER_MATCHMAKING_METHOD_1

    // Register HTTP signals on ColyseusClient
    bind_signal_3("_ColyseusClient", "_http_response",
        "request_id", NATIVEEXTENSION_VARIANT_TYPE_INT,
        "status_code", NATIVEEXTENSION_VARIANT_TYPE_INT,
        "body", NATIVEEXTENSION_VARIANT_TYPE_STRING);
    bind_signal_3("_ColyseusClient", "_http_error",
        "request_id", NATIVEEXTENSION_VARIANT_TYPE_INT,
        "code", NATIVEEXTENSION_VARIANT_TYPE_INT,
        "message", NATIVEEXTENSION_VARIANT_TYPE_STRING);

    // Register HTTP methods (use call_vararg — functions take Variant args)
    {
        const char* http_methods[] = { "http_get", "http_post", "http_put", "http_delete", "http_patch" };
        void* http_funcs[] = {
            gobuext_colyseus_client_http_get, gobuext_colyseus_client_http_post,
            gobuext_colyseus_client_http_put, gobuext_colyseus_client_http_delete,
            gobuext_colyseus_client_http_patch
        };
        for (int i = 0; i < 5; i++) {
            StringName method_name_string;
            constructors.string_name_new_with_latin1_chars(&method_name_string, http_methods[i], false);

            NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_INT, "");

            NativeExtensionClassMethodInfo method_info = {
                .name = &method_name_string,
                .method_userdata = http_funcs[i],
                .call_func = call_vararg,
                .ptrcall_func = NULL,
                .method_flags = NATIVEEXTENSION_METHOD_FLAG_VARARG,
                .has_return_value = true,
                .return_value_info = &return_info,
                .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
                .argument_count = 0,
                .arguments_info = NULL,
                .arguments_metadata = NULL,
                .default_argument_count = 0,
                .default_arguments = NULL
            };

            StringName class_name_string;
            constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusClient", false);
            api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

            destructors.string_name_destructor(&method_name_string);
            destructors.string_name_destructor(&class_name_string);
            destruct_property(&return_info);
        }
    }

    // Register auth methods (ptrcall — simple types)
    bind_method_1_no_ret("_ColyseusClient", "auth_set_token", gobuext_colyseus_client_auth_set_token,
        "token", NATIVEEXTENSION_VARIANT_TYPE_STRING);
    bind_method_0_with_ret("_ColyseusClient", "auth_get_token", gobuext_colyseus_client_auth_get_token,
        NATIVEEXTENSION_VARIANT_TYPE_STRING);

    // Register static poll() method
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "poll", false);

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = NULL,
            .call_func = (NativeExtensionClassMethodCall)gobuext_colyseus_client_poll_call,
            .ptrcall_func = (NativeExtensionClassMethodPtrCall)gobuext_colyseus_client_poll_ptrcall,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_STATIC,
            .has_return_value = false,
            .return_value_info = NULL,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 0,
            .arguments_info = NULL,
            .arguments_metadata = NULL,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusClient", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
    }

}

static void register_colyseus_room(void) {
    // Initialize StringNames
    StringName class_name;
    StringName parent_class_name;

    constructors.string_name_new_with_latin1_chars(&class_name, "_ColyseusRoom", false);
    constructors.string_name_new_with_latin1_chars(&parent_class_name, "RefCounted", false);

    register_extension_class(&class_name, &parent_class_name,
        gobuext_colyseus_room_constructor,
        gobuext_colyseus_room_constructor2,
        gobuext_colyseus_room_destructor);

    destructors.string_name_destructor(&class_name);
    destructors.string_name_destructor(&parent_class_name);

    // Register signals using helper functions
    bind_signal_0("_ColyseusRoom", "joined");
    bind_signal_0("_ColyseusRoom", "state_changed");
    bind_signal_0("_ColyseusRoom", "reconnected");
    bind_signal_2("_ColyseusRoom", "message_received", "type", NATIVEEXTENSION_VARIANT_TYPE_NIL, "data", NATIVEEXTENSION_VARIANT_TYPE_NIL);
    bind_signal_2("_ColyseusRoom", "error", "code", NATIVEEXTENSION_VARIANT_TYPE_INT, "message", NATIVEEXTENSION_VARIANT_TYPE_STRING);
    bind_signal_2("_ColyseusRoom", "left", "code", NATIVEEXTENSION_VARIANT_TYPE_INT, "reason", NATIVEEXTENSION_VARIANT_TYPE_STRING);
    bind_signal_2("_ColyseusRoom", "dropped", "code", NATIVEEXTENSION_VARIANT_TYPE_INT, "reason", NATIVEEXTENSION_VARIANT_TYPE_STRING);

    // Register send_message methods using vararg interface (accepts any Variant type for data)
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "send_message", false);

        NativeExtensionPropertyInfo args_info[] = {
            make_property(NATIVEEXTENSION_VARIANT_TYPE_STRING, "type"),
            make_property(NATIVEEXTENSION_VARIANT_TYPE_NIL, "data"),  // NIL = any Variant type
        };
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        };

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_room_send_message,
            .call_func = call_vararg,
            .ptrcall_func = NULL,  // Vararg methods don't support ptrcall
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
            .has_return_value = false,
            .return_value_info = NULL,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 2,
            .arguments_info = args_info,
            .arguments_metadata = args_metadata,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusRoom", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&args_info[0]);
        destruct_property(&args_info[1]);
    }

    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "send_message_int", false);

        NativeExtensionPropertyInfo args_info[] = {
            make_property(NATIVEEXTENSION_VARIANT_TYPE_INT, "type"),
            make_property(NATIVEEXTENSION_VARIANT_TYPE_NIL, "data"),  // NIL = any Variant type
        };
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        };

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_room_send_message_int,
            .call_func = call_vararg,
            .ptrcall_func = NULL,  // Vararg methods don't support ptrcall
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
            .has_return_value = false,
            .return_value_info = NULL,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 2,
            .arguments_info = args_info,
            .arguments_metadata = args_metadata,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusRoom", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&args_info[0]);
        destruct_property(&args_info[1]);
    }

    bind_method_0_no_ret(
        "_ColyseusRoom",
        "leave",
        gobuext_colyseus_room_leave
    );

    bind_method_0_with_ret(
        "_ColyseusRoom",
        "get_id",
        gobuext_colyseus_room_get_id,
        NATIVEEXTENSION_VARIANT_TYPE_STRING
    );

    bind_method_0_with_ret(
        "_ColyseusRoom",
        "get_session_id",
        gobuext_colyseus_room_get_session_id,
        NATIVEEXTENSION_VARIANT_TYPE_STRING
    );

    bind_method_0_with_ret(
        "_ColyseusRoom",
        "get_reconnection_token",
        gobuext_colyseus_room_get_reconnection_token,
        NATIVEEXTENSION_VARIANT_TYPE_STRING
    );

    bind_method_0_with_ret(
        "_ColyseusRoom",
        "get_name",
        gobuext_colyseus_room_get_name,
        NATIVEEXTENSION_VARIANT_TYPE_STRING
    );

    bind_method_0_with_ret(
        "_ColyseusRoom",
        "is_connected",
        gobuext_colyseus_room_is_connected,
        NATIVEEXTENSION_VARIANT_TYPE_BOOL
    );

    bind_method_0_with_ret(
        "_ColyseusRoom",
        "is_reconnecting",
        gobuext_colyseus_room_is_reconnecting,
        NATIVEEXTENSION_VARIANT_TYPE_BOOL
    );

    // set_reconnection_options(options: Dictionary) — vararg to accept Dictionary
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "set_reconnection_options", false);

        NativeExtensionPropertyInfo args_info[] = {
            make_property(NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY, "options"),
        };
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        };

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_room_set_reconnection_options,
            .call_func = call_vararg,
            .ptrcall_func = NULL,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
            .has_return_value = false,
            .return_value_info = NULL,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 1,
            .arguments_info = args_info,
            .arguments_metadata = args_metadata,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusRoom", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&args_info[0]);
    }

    // get_state - returns Dictionary with current state
    // Uses bind_method_0_with_ret like other working methods (is_connected, get_id, etc.)
    bind_method_0_with_ret(
        "_ColyseusRoom",
        "get_state",
        gobuext_colyseus_room_get_state,
        NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY
    );

    // set_state_type - vararg to accept either a String or a schema definition (Colyseus.schema())
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "set_state_type", false);

        NativeExtensionPropertyInfo args_info[] = {
            make_property(NATIVEEXTENSION_VARIANT_TYPE_NIL, "state_type"),  // NIL means any type
        };
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        };

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_room_set_state_type,
            .call_func = call_vararg,
            .ptrcall_func = NULL,  // Vararg methods don't support ptrcall
            .method_flags = NATIVEEXTENSION_METHOD_FLAGS_DEFAULT,
            .has_return_value = false,
            .return_value_info = NULL,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 1,
            .arguments_info = args_info,
            .arguments_metadata = args_metadata,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusRoom", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&args_info[0]);
    }
}

// Vararg call wrapper for methods that take variable arguments
static void call_vararg(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error)
{
    void (*function)(void *, NativeExtensionClassInstancePtr, const NativeExtensionConstVariantPtr *, NativeExtensionInt, NativeExtensionVariantPtr, NativeExtensionCallError *) = method_userdata;
    function(method_userdata, p_instance, p_args, p_argument_count, r_return, r_error);
}

static void register_colyseus_callbacks(void) {
    StringName class_name;
    StringName parent_class_name;

    constructors.string_name_new_with_latin1_chars(&class_name, "_ColyseusCallbacks", false);
    constructors.string_name_new_with_latin1_chars(&parent_class_name, "RefCounted", false);

    register_extension_class(&class_name, &parent_class_name,
        gobuext_colyseus_callbacks_constructor,
        gobuext_colyseus_callbacks_constructor2,
        gobuext_colyseus_callbacks_destructor);

    destructors.string_name_destructor(&class_name);
    destructors.string_name_destructor(&parent_class_name);

    // Register static method: get(room) -> ColyseusCallbacks
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "get", false);

        NativeExtensionPropertyInfo args_info[] = {
            make_property(NATIVEEXTENSION_VARIANT_TYPE_OBJECT, "room"),
        };
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        };

        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_OBJECT, "");

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_callbacks_get,
            .call_func = call_vararg,
            .ptrcall_func = NULL,  // Vararg methods don't support ptrcall
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_STATIC,  // Static method
            .has_return_value = true,
            .return_value_info = &return_info,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 1,
            .arguments_info = args_info,
            .arguments_metadata = args_metadata,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusCallbacks", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&args_info[0]);
        destruct_property(&return_info);
    }

    // Register listen method (vararg: target, property_or_callback, [callback])
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "listen", false);

        // We declare 2 args but accept more via vararg
        NativeExtensionPropertyInfo args_info[] = {
            make_property(NATIVEEXTENSION_VARIANT_TYPE_NIL, "target"),  // String or Dictionary
            make_property(NATIVEEXTENSION_VARIANT_TYPE_NIL, "property_or_callback"),
        };
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        };

        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_INT, "");

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_callbacks_listen,
            .call_func = call_vararg,
            .ptrcall_func = NULL,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_VARARG,  // Vararg to handle overloads
            .has_return_value = true,
            .return_value_info = &return_info,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 0,  // 0 for vararg
            .arguments_info = NULL,
            .arguments_metadata = NULL,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusCallbacks", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&args_info[0]);
        destruct_property(&args_info[1]);
        destruct_property(&return_info);
    }

    // Register on_add method (vararg)
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "on_add", false);

        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_INT, "");

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_callbacks_on_add,
            .call_func = call_vararg,
            .ptrcall_func = NULL,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_VARARG,
            .has_return_value = true,
            .return_value_info = &return_info,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 0,
            .arguments_info = NULL,
            .arguments_metadata = NULL,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusCallbacks", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&return_info);
    }

    // Register on_remove method (vararg)
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "on_remove", false);

        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_INT, "");

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_callbacks_on_remove,
            .call_func = call_vararg,
            .ptrcall_func = NULL,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_VARARG,
            .has_return_value = true,
            .return_value_info = &return_info,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 0,
            .arguments_info = NULL,
            .arguments_metadata = NULL,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusCallbacks", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&return_info);
    }

    // Register on_change method (vararg)
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "on_change", false);

        NativeExtensionPropertyInfo return_info = make_property(NATIVEEXTENSION_VARIANT_TYPE_INT, "");

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_callbacks_on_change,
            .call_func = call_vararg,
            .ptrcall_func = NULL,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_VARARG,
            .has_return_value = true,
            .return_value_info = &return_info,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 0,
            .arguments_info = NULL,
            .arguments_metadata = NULL,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusCallbacks", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&return_info);
    }

    // Register remove method
    {
        StringName method_name_string;
        constructors.string_name_new_with_latin1_chars(&method_name_string, "remove", false);

        NativeExtensionPropertyInfo args_info[] = {
            make_property(NATIVEEXTENSION_VARIANT_TYPE_INT, "handle"),
        };
        NativeExtensionClassMethodArgumentMetadata args_metadata[] = {
            NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
        };

        NativeExtensionClassMethodInfo method_info = {
            .name = &method_name_string,
            .method_userdata = (void*)gobuext_colyseus_callbacks_remove,
            .call_func = call_vararg,
            .ptrcall_func = NULL,
            .method_flags = NATIVEEXTENSION_METHOD_FLAG_NORMAL,
            .has_return_value = false,
            .return_value_info = NULL,
            .return_value_metadata = NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE,
            .argument_count = 1,
            .arguments_info = args_info,
            .arguments_metadata = args_metadata,
            .default_argument_count = 0,
            .default_arguments = NULL
        };

        StringName class_name_string;
        constructors.string_name_new_with_latin1_chars(&class_name_string, "_ColyseusCallbacks", false);

        api.classdb_register_extension_class_method(class_library, &class_name_string, &method_info);

        destructors.string_name_destructor(&method_name_string);
        destructors.string_name_destructor(&class_name_string);
        destruct_property(&args_info[0]);
    }

    destructors.string_name_destructor(&class_name);
    destructors.string_name_destructor(&parent_class_name);
}

static void colyseus_initialize(void *userdata, NativeExtensionInitializationLevel p_level) {
    (void)userdata;
    if (p_level != NATIVEEXTENSION_INITIALIZATION_SCENE) {
        return;
    }
    
    /* Initialize TLS certificates (auto-loads bundled or override certs) */
    gobuext_tls_certificates_init();
    
    register_colyseus_client();
    register_colyseus_room();
    register_colyseus_callbacks();
}

static void colyseus_deinitialize(void *userdata, NativeExtensionInitializationLevel p_level) {
    (void)userdata;
    if (p_level != NATIVEEXTENSION_INITIALIZATION_SCENE) {
        return;
    }
    
    /* Cleanup TLS certificates */
    gobuext_tls_certificates_cleanup();
}

static NativeExtensionBool colyseus_sdk_init(
    NativeExtensionInterfaceGetProcAddress p_get_proc_address,
    NativeExtensionClassLibraryPtr p_library,
    NativeExtensionInitialization *r_initialization
) {
    class_library = p_library;

    // API functions.
    api.classdb_register_extension_class2 = (NativeExtensionInterfaceClassdbRegisterExtensionClass2)p_get_proc_address("classdb_register_extension_class2");
    api.classdb_register_extension_class_method = (NativeExtensionInterfaceClassdbRegisterExtensionClassMethod)p_get_proc_address("classdb_register_extension_class_method");
    api.classdb_register_extension_class_signal = (NativeExtensionInterfaceClassdbRegisterExtensionClassSignal)p_get_proc_address("classdb_register_extension_class_signal");
    api.classdb_construct_object = (NativeExtensionInterfaceClassdbConstructObject)p_get_proc_address("classdb_construct_object");
    api.object_set_instance = (NativeExtensionInterfaceObjectSetInstance)p_get_proc_address("object_set_instance");
    api.object_set_instance_binding = (NativeExtensionInterfaceObjectSetInstanceBinding)p_get_proc_address("object_set_instance_binding");
    api.object_get_instance_binding = (NativeExtensionInterfaceObjectGetInstanceBinding)p_get_proc_address("object_get_instance_binding");
    api.mem_alloc = (NativeExtensionInterfaceMemAlloc)p_get_proc_address("mem_alloc");
    api.mem_free = (NativeExtensionInterfaceMemFree)p_get_proc_address("mem_free");
    api.variant_get_ptr_destructor = (NativeExtensionInterfaceVariantGetPtrDestructor)p_get_proc_address("variant_get_ptr_destructor");
    api.string_new_with_utf8_chars_and_len = (NativeExtensionInterfaceStringNewWithUtf8CharsAndLen)p_get_proc_address("string_new_with_utf8_chars_and_len");
    api.string_to_utf8_chars = (NativeExtensionInterfaceStringToUtf8Chars)p_get_proc_address("string_to_utf8_chars");
    api.variant_call = (NativeExtensionInterfaceVariantCall)p_get_proc_address("variant_call");
    api.packed_byte_array_operator_index = (NativeExtensionInterfacePackedByteArrayOperatorIndex)p_get_proc_address("packed_byte_array_operator_index");
    api.packed_byte_array_operator_index_const = (NativeExtensionInterfacePackedByteArrayOperatorIndexConst)p_get_proc_address("packed_byte_array_operator_index_const");
    api.ref_set_object = (NativeExtensionInterfaceRefSetObject)p_get_proc_address("ref_set_object");
    api.dictionary_operator_index = (NativeExtensionInterfaceDictionaryOperatorIndex)p_get_proc_address("dictionary_operator_index");
    api.array_operator_index = (NativeExtensionInterfaceArrayOperatorIndex)p_get_proc_address("array_operator_index");
    api.array_operator_index_const = (NativeExtensionInterfaceArrayOperatorIndexConst)p_get_proc_address("array_operator_index_const");
    api.variant_get_type = (NativeExtensionInterfaceVariantGetType)p_get_proc_address("variant_get_type");
    api.variant_set_keyed = (NativeExtensionInterfaceVariantSetKeyed)p_get_proc_address("variant_set_keyed");
    api.variant_set_indexed = (NativeExtensionInterfaceVariantSetIndexed)p_get_proc_address("variant_set_indexed");
    api.variant_new_nil = (NativeExtensionInterfaceVariantNewNil)p_get_proc_address("variant_new_nil");
    api.object_get_instance_id = (NativeExtensionInterfaceObjectGetInstanceId)p_get_proc_address("object_get_instance_id");
    api.object_get_instance_from_id = (NativeExtensionInterfaceObjectGetInstanceFromId)p_get_proc_address("object_get_instance_from_id");
    api.callable_custom_create = (NativeExtensionInterfaceCallableCustomCreate)p_get_proc_address("callable_custom_create");
    api.variant_new_copy = (NativeExtensionInterfaceVariantNewCopy)p_get_proc_address("variant_new_copy");
    api.variant_stringify = (NativeExtensionInterfaceVariantStringify)p_get_proc_address("variant_stringify");
    api.classdb_register_extension_class4 = (NativeExtensionInterfaceClassdbRegisterExtensionClass4)p_get_proc_address("classdb_register_extension_class4");
    api.classdb_register_extension_class5 = (NativeExtensionInterfaceClassdbRegisterExtensionClass5)p_get_proc_address("classdb_register_extension_class5");

    // Get variant from type constructor function
    NativeExtensionInterfaceGetVariantFromTypeConstructor get_variant_from_type_constructor = 
        (NativeExtensionInterfaceGetVariantFromTypeConstructor)p_get_proc_address("get_variant_from_type_constructor");
    NativeExtensionInterfaceGetVariantToTypeConstructor get_variant_to_type_constructor = 
        (NativeExtensionInterfaceGetVariantToTypeConstructor)p_get_proc_address("get_variant_to_type_constructor");

    // Constructors.
    constructors.string_name_new_with_latin1_chars = (NativeExtensionInterfaceStringNameNewWithLatin1Chars)p_get_proc_address("string_name_new_with_latin1_chars");
    constructors.string_new_with_utf8_chars = (NativeExtensionInterfaceStringNewWithUtf8Chars)p_get_proc_address("string_new_with_utf8_chars");
    constructors.string_new_with_utf8_chars_and_len = (NativeExtensionInterfaceStringNewWithUtf8CharsAndLen)p_get_proc_address("string_new_with_utf8_chars_and_len");
    constructors.variant_from_string_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_STRING);
    constructors.variant_from_bool_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_BOOL);
    constructors.variant_from_object_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_OBJECT);
    constructors.variant_from_packed_byte_array_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY);
    constructors.variant_from_int_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_INT);
    constructors.variant_from_float_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_FLOAT);
    constructors.variant_from_dictionary_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY);
    constructors.variant_from_array_constructor = get_variant_from_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_ARRAY);
    constructors.string_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_STRING);
    constructors.bool_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_BOOL);
    constructors.dictionary_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY);
    constructors.array_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_ARRAY);
    constructors.object_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_OBJECT);
    constructors.int_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_INT);
    constructors.float_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_FLOAT);
    constructors.string_name_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_STRING_NAME);
    constructors.packed_byte_array_from_variant_constructor = get_variant_to_type_constructor(NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY);

    // Get type constructors (index 0 = default constructor)
    NativeExtensionInterfaceVariantGetPtrConstructor get_ptr_constructor = 
        (NativeExtensionInterfaceVariantGetPtrConstructor)p_get_proc_address("variant_get_ptr_constructor");
    constructors.packed_byte_array_constructor = get_ptr_constructor(NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY, 0);
    constructors.dictionary_constructor = get_ptr_constructor(NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY, 0);
    constructors.array_constructor = get_ptr_constructor(NATIVEEXTENSION_VARIANT_TYPE_ARRAY, 0);
    constructors.string_from_string_name_constructor = get_ptr_constructor(NATIVEEXTENSION_VARIANT_TYPE_STRING, 2);  // String(StringName)

    // Destructors.
    destructors.string_name_destructor = api.variant_get_ptr_destructor(NATIVEEXTENSION_VARIANT_TYPE_STRING_NAME);
    destructors.string_destructor = api.variant_get_ptr_destructor(NATIVEEXTENSION_VARIANT_TYPE_STRING);
    destructors.dictionary_destructor = api.variant_get_ptr_destructor(NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY);
    destructors.array_destructor = api.variant_get_ptr_destructor(NATIVEEXTENSION_VARIANT_TYPE_ARRAY);
    destructors.packed_byte_array_destructor = api.variant_get_ptr_destructor(NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY);
    destructors.variant_destroy = (NativeExtensionInterfaceVariantDestroy)p_get_proc_address("variant_destroy");

    // Initialize TLS certificate API (needed before init callback)
    gobuext_tls_certificates_set_api(p_get_proc_address);

    r_initialization->initialize = colyseus_initialize;
    r_initialization->deinitialize = colyseus_deinitialize;
    r_initialization->minimum_initialization_level = NATIVEEXTENSION_INITIALIZATION_SCENE;

    return 1;
}

static void colyseus_plugin_deinitialize(void *userdata) {
    colyseus_deinitialize(userdata, NATIVEEXTENSION_INITIALIZATION_SCENE);
}

bool colyseus_register_online_provider(const GobuPluginApi *p_api, GobuPluginHandle p_plugin);

// Gobu's plugin entry point (gobu_plugin.h): the NativeExtension functions come from gobu_compat.c.
GOBU_PLUGIN_EXPORT bool gobu_plugin_init(const GobuPluginApi *p_api, GobuPluginHandle p_plugin, GobuPluginInfo *r_info) {
    // Version 1 is enough; the online provider needs version 2 (colyseus_provider.c checks it).
    if (p_api->version < 1 || p_api->size < offsetof(GobuPluginApi, register_online_provider)) {
        return false;
    }
    compat_set_api(p_api, p_plugin);

    NativeExtensionInitialization initialization = {0};
    if (!colyseus_sdk_init(compat_get_proc_address, (NativeExtensionClassLibraryPtr)p_plugin, &initialization)) {
        return false;
    }
    colyseus_initialize(NULL, NATIVEEXTENSION_INITIALIZATION_SCENE);
    if (!colyseus_register_online_provider(p_api, p_plugin)) {
        return false;
    }

    r_info->userdata = NULL;
    r_info->deinitialize = colyseus_plugin_deinitialize;
    return true;
}
