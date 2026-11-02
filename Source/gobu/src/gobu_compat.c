/*
 * NativeExtension on top of Gobu's plugin interface (include/gobu_plugin.h).
 *
 * The binding was written against NativeExtension (Gobu's former extension
 * API). Gobu now loads plugins through its own C interface, so this file
 * implements the NativeExtension functions the binding uses with it, and
 * gobu_plugin_init() (plugin.c) hands them to colyseus_sdk_init()
 * through compat_get_proc_address().
 *
 * In this mode every engine value the binding holds (String, StringName,
 * Dictionary, Array, PackedByteArray) is a Variant holding it: see the sizes
 * in gobu_colyseus.h.
 */

#include "gobu_compat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const GobuPluginApi *gobu = NULL;
static GobuPluginHandle gobu_plugin = NULL;

/* The object ClassDB is creating while a constructor of the binding runs (see compat_create_instance). */
static GobuObject *creating_object = NULL;
static int creating_object_taken = 0;
static void *created_instance = NULL;

void compat_set_api(const GobuPluginApi *p_api, GobuPluginHandle p_plugin) {
	gobu = p_api;
	gobu_plugin = p_plugin;
}

/* A StringName/String (a Variant here) as a C string: `buffer` if it fits, else malloc'd (free it). */
static char *to_cstr(const void *p_value, char *r_buffer, int64_t p_size) {
	const int64_t length = gobu->variant_to_utf8((const GobuVariant *)p_value, r_buffer, p_size);
	if (length < p_size) {
		return r_buffer;
	}
	char *text = (char *)malloc((size_t)length + 1);
	gobu->variant_to_utf8((const GobuVariant *)p_value, text, length + 1);
	return text;
}

static void free_cstr(char *p_text, char *p_buffer) {
	if (p_text != p_buffer) {
		free(p_text);
	}
}

#define WITH_CSTR(m_name, m_value, m_body)                       \
	{                                                            \
		char m_name##_buffer[128];                               \
		char *m_name = to_cstr(m_value, m_name##_buffer, 128);   \
		m_body;                                                  \
		free_cstr(m_name, m_name##_buffer);                      \
	}

/* Strings. */

static void compat_string_name_new_with_latin1_chars(NativeExtensionUninitializedStringNamePtr r_dest, const char *p_contents, NativeExtensionBool p_is_static) {
	(void)p_is_static;
	gobu->variant_new_string_name((GobuVariant *)r_dest, p_contents, -1);
}

static void compat_string_new_with_utf8_chars(NativeExtensionUninitializedStringPtr r_dest, const char *p_contents) {
	gobu->variant_new_string((GobuVariant *)r_dest, p_contents, -1);
}

static void compat_string_new_with_utf8_chars_and_len(NativeExtensionUninitializedStringPtr r_dest, const char *p_contents, NativeExtensionInt p_size) {
	gobu->variant_new_string((GobuVariant *)r_dest, p_contents, p_size);
}

/* NativeExtension writes up to `p_max_write_length` bytes, without a NUL, and returns the full length. */
static NativeExtensionInt compat_string_to_utf8_chars(NativeExtensionConstStringPtr p_self, char *r_text, NativeExtensionInt p_max_write_length) {
	const int64_t length = gobu->variant_to_utf8((const GobuVariant *)p_self, NULL, 0);
	if (r_text && p_max_write_length > 0) {
		char *text = (char *)malloc((size_t)length + 1);
		gobu->variant_to_utf8((const GobuVariant *)p_self, text, length + 1);
		memcpy(r_text, text, (size_t)(length < p_max_write_length ? length : p_max_write_length));
		free(text);
	}
	return length;
}

/* Variants. */

static void compat_variant_new_nil(NativeExtensionUninitializedVariantPtr r_dest) {
	gobu->variant_new_nil((GobuVariant *)r_dest);
}

static void compat_variant_new_copy(NativeExtensionUninitializedVariantPtr r_dest, NativeExtensionConstVariantPtr p_src) {
	gobu->variant_new_copy((GobuVariant *)r_dest, (const GobuVariant *)p_src);
}

static void compat_variant_destroy(NativeExtensionVariantPtr p_self) {
	gobu->variant_destroy((GobuVariant *)p_self);
}

static NativeExtensionVariantType compat_variant_get_type(NativeExtensionConstVariantPtr p_self) {
	return (NativeExtensionVariantType)gobu->variant_get_type((const GobuVariant *)p_self);
}

static void compat_variant_stringify(NativeExtensionConstVariantPtr p_self, NativeExtensionStringPtr r_ret) {
	gobu->variant_stringify((const GobuVariant *)p_self, (GobuVariant *)r_ret);
}

static void compat_variant_call(NativeExtensionVariantPtr p_self, NativeExtensionConstStringNamePtr p_method, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionUninitializedVariantPtr r_return, NativeExtensionCallError *r_error) {
	GobuCallError error = { 0, 0, 0 };
	WITH_CSTR(method, p_method, gobu->variant_call((GobuVariant *)p_self, method, (const GobuVariant *const *)p_args, p_argument_count, (GobuVariant *)r_return, &error));
	if (r_error) {
		r_error->error = (NativeExtensionCallErrorType)error.error;
		r_error->argument = error.argument;
		r_error->expected = error.expected;
	}
}

static void compat_variant_set_keyed(NativeExtensionVariantPtr p_self, NativeExtensionConstVariantPtr p_key, NativeExtensionConstVariantPtr p_value, NativeExtensionBool *r_valid) {
	const bool valid = gobu->variant_set((GobuVariant *)p_self, (const GobuVariant *)p_key, (const GobuVariant *)p_value);
	if (r_valid) {
		*r_valid = valid;
	}
}

static void compat_variant_set_indexed(NativeExtensionVariantPtr p_self, NativeExtensionInt p_index, NativeExtensionConstVariantPtr p_value, NativeExtensionBool *r_valid, NativeExtensionBool *r_oob) {
	const bool valid = gobu->variant_set_indexed((GobuVariant *)p_self, p_index, (const GobuVariant *)p_value);
	if (r_valid) {
		*r_valid = valid;
	}
	if (r_oob) {
		*r_oob = !valid;
	}
}

/* Typed values to and from Variants. String, StringName, Dictionary, Array and PackedByteArray are
 * Variants here; bool, int, float and Object are the C values NativeExtension uses. */

static void from_bool(NativeExtensionUninitializedVariantPtr r_dest, NativeExtensionTypePtr p_src) {
	gobu->variant_new_bool((GobuVariant *)r_dest, *(const NativeExtensionBool *)p_src != 0);
}
static void from_int(NativeExtensionUninitializedVariantPtr r_dest, NativeExtensionTypePtr p_src) {
	gobu->variant_new_int((GobuVariant *)r_dest, *(const int64_t *)p_src);
}
static void from_float(NativeExtensionUninitializedVariantPtr r_dest, NativeExtensionTypePtr p_src) {
	gobu->variant_new_float((GobuVariant *)r_dest, *(const double *)p_src);
}
static void from_object(NativeExtensionUninitializedVariantPtr r_dest, NativeExtensionTypePtr p_src) {
	gobu->variant_new_object((GobuVariant *)r_dest, *(GobuObject *const *)p_src);
}
static void from_variant_value(NativeExtensionUninitializedVariantPtr r_dest, NativeExtensionTypePtr p_src) {
	gobu->variant_new_copy((GobuVariant *)r_dest, (const GobuVariant *)p_src);
}

static NativeExtensionVariantFromTypeConstructorFunc compat_get_variant_from_type_constructor(NativeExtensionVariantType p_type) {
	switch (p_type) {
		case NATIVEEXTENSION_VARIANT_TYPE_BOOL:
			return from_bool;
		case NATIVEEXTENSION_VARIANT_TYPE_INT:
			return from_int;
		case NATIVEEXTENSION_VARIANT_TYPE_FLOAT:
			return from_float;
		case NATIVEEXTENSION_VARIANT_TYPE_OBJECT:
			return from_object;
		default:
			return from_variant_value;
	}
}

static void to_bool(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	*(NativeExtensionBool *)r_dest = gobu->variant_to_bool((const GobuVariant *)p_src);
}
static void to_int(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	*(int64_t *)r_dest = gobu->variant_to_int((const GobuVariant *)p_src);
}
static void to_float(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	*(double *)r_dest = gobu->variant_to_float((const GobuVariant *)p_src);
}
static void to_object(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	*(GobuObject **)r_dest = gobu->variant_to_object((const GobuVariant *)p_src);
}
static void to_string(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	if (gobu->variant_get_type((const GobuVariant *)p_src) == GOBU_TYPE_STRING) {
		gobu->variant_new_copy((GobuVariant *)r_dest, (const GobuVariant *)p_src);
	} else {
		gobu->variant_stringify((const GobuVariant *)p_src, (GobuVariant *)r_dest);
	}
}
static void to_string_name(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	WITH_CSTR(text, p_src, gobu->variant_new_string_name((GobuVariant *)r_dest, text, -1));
}
/* Dictionary, Array, PackedByteArray: the value if it has the type, else an empty one. */
static void to_same_type(int32_t p_type, NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	if (gobu->variant_get_type((const GobuVariant *)p_src) == p_type) {
		gobu->variant_new_copy((GobuVariant *)r_dest, (const GobuVariant *)p_src);
	} else {
		gobu->variant_new_type((GobuVariant *)r_dest, p_type);
	}
}
static void to_dictionary(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	to_same_type(GOBU_TYPE_DICTIONARY, r_dest, p_src);
}
static void to_array(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	to_same_type(GOBU_TYPE_ARRAY, r_dest, p_src);
}
static void to_packed_byte_array(NativeExtensionUninitializedTypePtr r_dest, NativeExtensionVariantPtr p_src) {
	to_same_type(GOBU_TYPE_PACKED_BYTE_ARRAY, r_dest, p_src);
}

static NativeExtensionTypeFromVariantConstructorFunc compat_get_variant_to_type_constructor(NativeExtensionVariantType p_type) {
	switch (p_type) {
		case NATIVEEXTENSION_VARIANT_TYPE_BOOL:
			return to_bool;
		case NATIVEEXTENSION_VARIANT_TYPE_INT:
			return to_int;
		case NATIVEEXTENSION_VARIANT_TYPE_FLOAT:
			return to_float;
		case NATIVEEXTENSION_VARIANT_TYPE_OBJECT:
			return to_object;
		case NATIVEEXTENSION_VARIANT_TYPE_STRING:
			return to_string;
		case NATIVEEXTENSION_VARIANT_TYPE_STRING_NAME:
			return to_string_name;
		case NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY:
			return to_dictionary;
		case NATIVEEXTENSION_VARIANT_TYPE_ARRAY:
			return to_array;
		case NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY:
			return to_packed_byte_array;
		default:
			return NULL;
	}
}

/* Constructors and destructors of the typed values the binding builds by hand. */

static void new_dictionary(NativeExtensionUninitializedTypePtr p_base, const NativeExtensionConstTypePtr *p_args) {
	(void)p_args;
	gobu->dictionary_new((GobuVariant *)p_base);
}
static void new_array(NativeExtensionUninitializedTypePtr p_base, const NativeExtensionConstTypePtr *p_args) {
	(void)p_args;
	gobu->array_new((GobuVariant *)p_base);
}
static void new_packed_byte_array(NativeExtensionUninitializedTypePtr p_base, const NativeExtensionConstTypePtr *p_args) {
	(void)p_args;
	gobu->variant_new_packed_byte_array((GobuVariant *)p_base, NULL, 0);
}
static void new_string_from_string_name(NativeExtensionUninitializedTypePtr p_base, const NativeExtensionConstTypePtr *p_args) {
	WITH_CSTR(text, p_args[0], gobu->variant_new_string((GobuVariant *)p_base, text, -1));
}

static NativeExtensionPtrConstructor compat_variant_get_ptr_constructor(NativeExtensionVariantType p_type, int32_t p_constructor) {
	if (p_type == NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY && p_constructor == 0) {
		return new_dictionary;
	}
	if (p_type == NATIVEEXTENSION_VARIANT_TYPE_ARRAY && p_constructor == 0) {
		return new_array;
	}
	if (p_type == NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY && p_constructor == 0) {
		return new_packed_byte_array;
	}
	if (p_type == NATIVEEXTENSION_VARIANT_TYPE_STRING && p_constructor == 2) {
		return new_string_from_string_name;
	}
	fprintf(stderr, "colyseus: no constructor %d for type %d.\n", p_constructor, (int)p_type);
	return NULL;
}

static void destroy_value(NativeExtensionTypePtr p_base) {
	gobu->variant_destroy((GobuVariant *)p_base);
}

static NativeExtensionPtrDestructor compat_variant_get_ptr_destructor(NativeExtensionVariantType p_type) {
	(void)p_type;
	return destroy_value; /* Every one is a Variant here. */
}

/* Containers. */

static uint8_t *compat_packed_byte_array_operator_index(NativeExtensionTypePtr p_self, NativeExtensionInt p_index) {
	uint8_t *data = gobu->packed_byte_array_data((GobuVariant *)p_self);
	return data ? data + p_index : NULL;
}

static const uint8_t *compat_packed_byte_array_operator_index_const(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index) {
	uint8_t *data = gobu->packed_byte_array_data((GobuVariant *)p_self);
	return data ? data + p_index : NULL;
}

static NativeExtensionVariantPtr compat_dictionary_operator_index(NativeExtensionTypePtr p_self, NativeExtensionConstVariantPtr p_key) {
	return gobu->dictionary_index((GobuVariant *)p_self, (const GobuVariant *)p_key);
}

static NativeExtensionVariantPtr compat_array_operator_index(NativeExtensionTypePtr p_self, NativeExtensionInt p_index) {
	return gobu->array_index((GobuVariant *)p_self, p_index);
}

static NativeExtensionVariantPtr compat_array_operator_index_const(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index) {
	return gobu->array_index((GobuVariant *)p_self, p_index);
}

/* Memory and objects. */

static void *compat_mem_alloc(size_t p_bytes) {
	return gobu->mem_alloc(p_bytes);
}

static void compat_mem_free(void *p_ptr) {
	gobu->mem_free(p_ptr);
}

static NativeExtensionObjectInstanceID compat_object_get_instance_id(NativeExtensionConstObjectPtr p_object) {
	return gobu->object_get_instance_id((const GobuObject *)p_object);
}

static NativeExtensionObjectPtr compat_object_get_instance_from_id(NativeExtensionObjectInstanceID p_instance_id) {
	return gobu->object_from_instance_id(p_instance_id);
}

static NativeExtensionObjectPtr compat_global_get_singleton(NativeExtensionConstStringNamePtr p_name) {
	GobuObject *singleton = NULL;
	WITH_CSTR(name, p_name, singleton = gobu->get_singleton(name));
	return singleton;
}

/* A constructor of the binding makes its object with classdb_construct_object(parent) and gives it its
 * data with object_set_instance(). Here ClassDB has already made the object: hand that one over. */
static NativeExtensionObjectPtr compat_classdb_construct_object(NativeExtensionConstStringNamePtr p_classname) {
	if (creating_object && !creating_object_taken) {
		creating_object_taken = 1;
		return creating_object;
	}
	GobuObject *object = NULL;
	WITH_CSTR(name, p_classname, object = gobu->classdb_construct_object(name));
	return object;
}

static void compat_object_set_instance(NativeExtensionObjectPtr p_o, NativeExtensionConstStringNamePtr p_classname, NativeExtensionClassInstancePtr p_instance) {
	(void)p_classname;
	if (p_o == creating_object) {
		created_instance = p_instance;
	}
}

/* Classes. */

typedef struct {
	void *userdata;
	NativeExtensionClassCreateInstance create;
	NativeExtensionClassCreateInstance2 create2;
	NativeExtensionClassFreeInstance free_instance;
} CompatClass;

static void *compat_create_instance(void *p_class_userdata, GobuObject *p_object) {
	CompatClass *cls = (CompatClass *)p_class_userdata;
	GobuObject *outer_object = creating_object; /* A constructor may make another object. */
	const int outer_taken = creating_object_taken;
	void *outer_instance = created_instance;
	creating_object = p_object;
	creating_object_taken = 0;
	created_instance = NULL;

	if (cls->create2) {
		cls->create2(cls->userdata, 0);
	} else if (cls->create) {
		cls->create(cls->userdata);
	}
	void *instance = created_instance;

	creating_object = outer_object;
	creating_object_taken = outer_taken;
	created_instance = outer_instance;
	return instance;
}

static void compat_free_instance(void *p_class_userdata, void *p_instance) {
	CompatClass *cls = (CompatClass *)p_class_userdata;
	if (cls->free_instance) {
		cls->free_instance(cls->userdata, p_instance);
	}
}

static void register_class(NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, void *p_userdata, NativeExtensionClassCreateInstance p_create, NativeExtensionClassCreateInstance2 p_create2, NativeExtensionClassFreeInstance p_free, NativeExtensionBool p_is_abstract, NativeExtensionBool p_is_exposed) {
	CompatClass *cls = (CompatClass *)calloc(1, sizeof(CompatClass)); /* Lives as long as the class. */
	cls->userdata = p_userdata;
	cls->create = p_create;
	cls->create2 = p_create2;
	cls->free_instance = p_free;

	GobuClassInfo info;
	memset(&info, 0, sizeof(info));
	info.flags = (p_is_abstract ? GOBU_CLASS_ABSTRACT : 0) | (p_is_exposed ? 0 : GOBU_CLASS_HIDDEN);
	info.userdata = cls;
	info.create_instance = compat_create_instance;
	info.free_instance = compat_free_instance;
	WITH_CSTR(name, p_class_name, WITH_CSTR(parent, p_parent_class_name, {
		info.name = name;
		info.parent = parent;
		gobu->register_class(gobu_plugin, &info);
	}));
}

static void compat_classdb_register_extension_class2(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo2 *p_info) {
	(void)p_library;
	register_class(p_class_name, p_parent_class_name, p_info->class_userdata, p_info->create_instance_func, NULL, p_info->free_instance_func, p_info->is_abstract, p_info->is_exposed);
}

static void compat_classdb_register_extension_class4(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo4 *p_info) {
	(void)p_library;
	register_class(p_class_name, p_parent_class_name, p_info->class_userdata, NULL, p_info->create_instance_func, p_info->free_instance_func, p_info->is_abstract, p_info->is_exposed);
}

static void compat_classdb_register_extension_class5(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo5 *p_info) {
	(void)p_library;
	register_class(p_class_name, p_parent_class_name, p_info->class_userdata, NULL, p_info->create_instance_func, p_info->free_instance_func, p_info->is_abstract, p_info->is_exposed);
}

/* The names in a NativeExtensionPropertyInfo are StringNames (Variants here); kept until registered. */
typedef struct {
	GobuArgInfo info;
	char name[128];
	char class_name[128];
} CompatArg;

static void to_arg(const NativeExtensionPropertyInfo *p_property, CompatArg *r_arg) {
	memset(r_arg, 0, sizeof(*r_arg));
	r_arg->info.type = (int32_t)p_property->type;
	if (p_property->name) {
		gobu->variant_to_utf8((const GobuVariant *)p_property->name, r_arg->name, sizeof(r_arg->name));
	}
	r_arg->info.name = r_arg->name;
	if (p_property->class_name) {
		gobu->variant_to_utf8((const GobuVariant *)p_property->class_name, r_arg->class_name, sizeof(r_arg->class_name));
	}
	r_arg->info.class_name = r_arg->class_name[0] ? r_arg->class_name : NULL;
	if (p_property->type == NATIVEEXTENSION_VARIANT_TYPE_NIL) {
		r_arg->info.flags = GOBU_ARG_ANY_VARIANT;
	}
}

static void compat_classdb_register_extension_class_method(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, const NativeExtensionClassMethodInfo *p_method_info) {
	(void)p_library;
	const uint32_t count = p_method_info->argument_count;
	CompatArg *args = count ? (CompatArg *)calloc(count, sizeof(CompatArg)) : NULL;
	GobuArgInfo *arg_infos = count ? (GobuArgInfo *)calloc(count, sizeof(GobuArgInfo)) : NULL;
	for (uint32_t i = 0; i < count; i++) {
		to_arg(&p_method_info->arguments_info[i], &args[i]);
		arg_infos[i] = args[i].info;
	}
	CompatArg ret;
	memset(&ret, 0, sizeof(ret));
	if (p_method_info->has_return_value && p_method_info->return_value_info) {
		to_arg(p_method_info->return_value_info, &ret);
	} else if (p_method_info->has_return_value) {
		ret.info.flags = GOBU_ARG_ANY_VARIANT;
	}

	GobuMethodInfo info;
	memset(&info, 0, sizeof(info));
	info.func = (GobuMethodFunc)p_method_info->call_func; /* Same signature: Variant arguments. */
	info.userdata = p_method_info->method_userdata;
	if (p_method_info->method_flags & NATIVEEXTENSION_METHOD_FLAG_VARARG) {
		info.flags |= GOBU_METHOD_VARARG;
	}
	if (p_method_info->method_flags & NATIVEEXTENSION_METHOD_FLAG_STATIC) {
		info.flags |= GOBU_METHOD_STATIC;
	}
	if (p_method_info->method_flags & NATIVEEXTENSION_METHOD_FLAG_CONST) {
		info.flags |= GOBU_METHOD_CONST;
	}
	info.has_return = p_method_info->has_return_value != 0;
	info.return_info = ret.info;
	info.arg_count = (int32_t)count;
	info.args = arg_infos;
	info.default_arg_count = (int32_t)p_method_info->default_argument_count;
	info.default_args = (const GobuVariant *const *)p_method_info->default_arguments;

	WITH_CSTR(method, p_method_info->name, WITH_CSTR(cls, p_class_name, {
		info.name = method;
		gobu->register_method(gobu_plugin, cls, &info);
	}));
	free(arg_infos);
	free(args);
}

static void compat_classdb_register_extension_class_signal(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_signal_name, const NativeExtensionPropertyInfo *p_argument_info, NativeExtensionInt p_argument_count) {
	(void)p_library;
	CompatArg *args = p_argument_count ? (CompatArg *)calloc((size_t)p_argument_count, sizeof(CompatArg)) : NULL;
	GobuArgInfo *arg_infos = p_argument_count ? (GobuArgInfo *)calloc((size_t)p_argument_count, sizeof(GobuArgInfo)) : NULL;
	for (NativeExtensionInt i = 0; i < p_argument_count; i++) {
		to_arg(&p_argument_info[i], &args[i]);
		arg_infos[i] = args[i].info;
	}
	WITH_CSTR(signal, p_signal_name, WITH_CSTR(cls, p_class_name, {
		gobu->register_signal(gobu_plugin, cls, signal, arg_infos, (int32_t)p_argument_count);
	}));
	free(arg_infos);
	free(args);
}

/* The lookup colyseus_sdk_init() and the TLS code ask for their functions. */

typedef struct {
	const char *name;
	NativeExtensionInterfaceFunctionPtr function;
} CompatFunction;

#define F(m_name) { #m_name, (NativeExtensionInterfaceFunctionPtr)compat_##m_name }
static const CompatFunction compat_functions[] = {
	F(classdb_register_extension_class2),
	F(classdb_register_extension_class4),
	F(classdb_register_extension_class5),
	F(classdb_register_extension_class_method),
	F(classdb_register_extension_class_signal),
	F(classdb_construct_object),
	F(object_set_instance),
	F(mem_alloc),
	F(mem_free),
	F(variant_get_ptr_destructor),
	F(variant_get_ptr_constructor),
	F(string_name_new_with_latin1_chars),
	F(string_new_with_utf8_chars),
	F(string_new_with_utf8_chars_and_len),
	F(string_to_utf8_chars),
	F(variant_call),
	F(packed_byte_array_operator_index),
	F(packed_byte_array_operator_index_const),
	F(dictionary_operator_index),
	F(array_operator_index),
	F(array_operator_index_const),
	F(variant_get_type),
	F(variant_set_keyed),
	F(variant_set_indexed),
	F(variant_new_nil),
	F(variant_new_copy),
	F(variant_destroy),
	F(variant_stringify),
	F(object_get_instance_id),
	F(object_get_instance_from_id),
	F(get_variant_from_type_constructor),
	F(get_variant_to_type_constructor),
	F(global_get_singleton),
};
#undef F

NativeExtensionInterfaceFunctionPtr compat_get_proc_address(const char *p_function_name) {
	for (size_t i = 0; i < sizeof(compat_functions) / sizeof(compat_functions[0]); i++) {
		if (strcmp(compat_functions[i].name, p_function_name) == 0) {
			return compat_functions[i].function;
		}
	}
	return NULL; /* Not used by the binding (instance bindings, ref_set_object, custom callables). */
}
