/**************************************************************************/
/*  nativeextension_interface.gen.h                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GOBU ENGINE                               */
/*                        https://gobuengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Gobu Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

/* THIS FILE IS GENERATED. EDITS WILL BE LOST. */

#pragma once

#ifndef __cplusplus
#include <stddef.h>
#include <stdint.h>

typedef uint32_t char32_t;
typedef uint16_t char16_t;
#else
#include <cstddef>
#include <cstdint>

extern "C" {
#endif

typedef enum {
	NATIVEEXTENSION_VARIANT_TYPE_NIL = 0,
	NATIVEEXTENSION_VARIANT_TYPE_BOOL = 1,
	NATIVEEXTENSION_VARIANT_TYPE_INT = 2,
	NATIVEEXTENSION_VARIANT_TYPE_FLOAT = 3,
	NATIVEEXTENSION_VARIANT_TYPE_STRING = 4,
	NATIVEEXTENSION_VARIANT_TYPE_VECTOR2 = 5,
	NATIVEEXTENSION_VARIANT_TYPE_VECTOR2I = 6,
	NATIVEEXTENSION_VARIANT_TYPE_RECT2 = 7,
	NATIVEEXTENSION_VARIANT_TYPE_RECT2I = 8,
	NATIVEEXTENSION_VARIANT_TYPE_VECTOR3 = 9,
	NATIVEEXTENSION_VARIANT_TYPE_VECTOR3I = 10,
	NATIVEEXTENSION_VARIANT_TYPE_TRANSFORM2D = 11,
	NATIVEEXTENSION_VARIANT_TYPE_VECTOR4 = 12,
	NATIVEEXTENSION_VARIANT_TYPE_VECTOR4I = 13,
	NATIVEEXTENSION_VARIANT_TYPE_PLANE = 14,
	NATIVEEXTENSION_VARIANT_TYPE_QUATERNION = 15,
	NATIVEEXTENSION_VARIANT_TYPE_AABB = 16,
	NATIVEEXTENSION_VARIANT_TYPE_BASIS = 17,
	NATIVEEXTENSION_VARIANT_TYPE_TRANSFORM3D = 18,
	NATIVEEXTENSION_VARIANT_TYPE_PROJECTION = 19,
	NATIVEEXTENSION_VARIANT_TYPE_COLOR = 20,
	NATIVEEXTENSION_VARIANT_TYPE_STRING_NAME = 21,
	NATIVEEXTENSION_VARIANT_TYPE_NODE_PATH = 22,
	NATIVEEXTENSION_VARIANT_TYPE_RID = 23,
	NATIVEEXTENSION_VARIANT_TYPE_OBJECT = 24,
	NATIVEEXTENSION_VARIANT_TYPE_CALLABLE = 25,
	NATIVEEXTENSION_VARIANT_TYPE_SIGNAL = 26,
	NATIVEEXTENSION_VARIANT_TYPE_DICTIONARY = 27,
	NATIVEEXTENSION_VARIANT_TYPE_ARRAY = 28,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY = 29,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_INT32_ARRAY = 30,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_INT64_ARRAY = 31,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_FLOAT32_ARRAY = 32,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_FLOAT64_ARRAY = 33,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_STRING_ARRAY = 34,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_VECTOR2_ARRAY = 35,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_VECTOR3_ARRAY = 36,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_COLOR_ARRAY = 37,
	NATIVEEXTENSION_VARIANT_TYPE_PACKED_VECTOR4_ARRAY = 38,
	NATIVEEXTENSION_VARIANT_TYPE_VARIANT_MAX = 39,
} NativeExtensionVariantType;

typedef enum {
	NATIVEEXTENSION_VARIANT_OP_EQUAL = 0,
	NATIVEEXTENSION_VARIANT_OP_NOT_EQUAL = 1,
	NATIVEEXTENSION_VARIANT_OP_LESS = 2,
	NATIVEEXTENSION_VARIANT_OP_LESS_EQUAL = 3,
	NATIVEEXTENSION_VARIANT_OP_GREATER = 4,
	NATIVEEXTENSION_VARIANT_OP_GREATER_EQUAL = 5,
	NATIVEEXTENSION_VARIANT_OP_ADD = 6,
	NATIVEEXTENSION_VARIANT_OP_SUBTRACT = 7,
	NATIVEEXTENSION_VARIANT_OP_MULTIPLY = 8,
	NATIVEEXTENSION_VARIANT_OP_DIVIDE = 9,
	NATIVEEXTENSION_VARIANT_OP_NEGATE = 10,
	NATIVEEXTENSION_VARIANT_OP_POSITIVE = 11,
	NATIVEEXTENSION_VARIANT_OP_MODULE = 12,
	NATIVEEXTENSION_VARIANT_OP_POWER = 13,
	NATIVEEXTENSION_VARIANT_OP_SHIFT_LEFT = 14,
	NATIVEEXTENSION_VARIANT_OP_SHIFT_RIGHT = 15,
	NATIVEEXTENSION_VARIANT_OP_BIT_AND = 16,
	NATIVEEXTENSION_VARIANT_OP_BIT_OR = 17,
	NATIVEEXTENSION_VARIANT_OP_BIT_XOR = 18,
	NATIVEEXTENSION_VARIANT_OP_BIT_NEGATE = 19,
	NATIVEEXTENSION_VARIANT_OP_AND = 20,
	NATIVEEXTENSION_VARIANT_OP_OR = 21,
	NATIVEEXTENSION_VARIANT_OP_XOR = 22,
	NATIVEEXTENSION_VARIANT_OP_NOT = 23,
	NATIVEEXTENSION_VARIANT_OP_IN = 24,
	NATIVEEXTENSION_VARIANT_OP_MAX = 25,
} NativeExtensionVariantOperator;

/* In this API there are multiple functions which expect the caller to pass a pointer
 * on return value as parameter.
 * In order to make it clear if the caller should initialize the return value or not
 * we have two flavor of types:
 * - `NativeExtensionXXXPtr` for pointer on an initialized value
 * - `NativeExtensionUninitializedXXXPtr` for pointer on uninitialized value
 *
 * Notes:
 * - Not respecting those requirements can seems harmless, but will lead to unexpected
 * segfault or memory leak (for instance with a specific compiler/OS, or when two
 * native extensions start doing ptrcall on each other).
 * - Initialization must be done with the function pointer returned by `variant_get_ptr_constructor`,
 * zero-initializing the variable should not be considered a valid initialization method here !
 * - Some types have no destructor (String, StringName, Array, Dictionary... have one; int, float, bool... don't), for
 * them it is always safe to skip the constructor for the return value if you are in a hurry ;-)
 */
typedef void *NativeExtensionVariantPtr;
typedef const void *NativeExtensionConstVariantPtr;
typedef void *NativeExtensionUninitializedVariantPtr;
typedef void *NativeExtensionStringNamePtr;
typedef const void *NativeExtensionConstStringNamePtr;
typedef void *NativeExtensionUninitializedStringNamePtr;
typedef void *NativeExtensionStringPtr;
typedef const void *NativeExtensionConstStringPtr;
typedef void *NativeExtensionUninitializedStringPtr;
typedef void *NativeExtensionObjectPtr;
typedef const void *NativeExtensionConstObjectPtr;
typedef void *NativeExtensionUninitializedObjectPtr;
typedef void *NativeExtensionTypePtr;
typedef const void *NativeExtensionConstTypePtr;
typedef void *NativeExtensionUninitializedTypePtr;
typedef const void *NativeExtensionMethodBindPtr;
typedef int64_t NativeExtensionInt;
typedef uint8_t NativeExtensionBool;
typedef uint64_t NativeExtensionObjectInstanceID;
typedef void *NativeExtensionRefPtr;
typedef const void *NativeExtensionConstRefPtr;
typedef enum {
	NATIVEEXTENSION_CALL_OK = 0,
	NATIVEEXTENSION_CALL_ERROR_INVALID_METHOD = 1,
	/* Expected a different variant type. */
	NATIVEEXTENSION_CALL_ERROR_INVALID_ARGUMENT = 2,
	/* Expected lower number of arguments. */
	NATIVEEXTENSION_CALL_ERROR_TOO_MANY_ARGUMENTS = 3,
	/* Expected higher number of arguments. */
	NATIVEEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS = 4,
	NATIVEEXTENSION_CALL_ERROR_INSTANCE_IS_NULL = 5,
	/* Used for const call. */
	NATIVEEXTENSION_CALL_ERROR_METHOD_NOT_CONST = 6,
} NativeExtensionCallErrorType;

typedef struct {
	NativeExtensionCallErrorType error;
	int32_t argument;
	int32_t expected;
} NativeExtensionCallError;

typedef void (*NativeExtensionVariantFromTypeConstructorFunc)(NativeExtensionUninitializedVariantPtr, NativeExtensionTypePtr);
typedef void (*NativeExtensionTypeFromVariantConstructorFunc)(NativeExtensionUninitializedTypePtr, NativeExtensionVariantPtr);
typedef void *(*NativeExtensionVariantGetInternalPtrFunc)(NativeExtensionVariantPtr);
typedef void (*NativeExtensionPtrOperatorEvaluator)(NativeExtensionConstTypePtr p_left, NativeExtensionConstTypePtr p_right, NativeExtensionTypePtr r_result);
typedef void (*NativeExtensionPtrBuiltInMethod)(NativeExtensionTypePtr p_base, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_return, int32_t p_argument_count);
typedef void (*NativeExtensionPtrConstructor)(NativeExtensionUninitializedTypePtr p_base, const NativeExtensionConstTypePtr *p_args);
typedef void (*NativeExtensionPtrDestructor)(NativeExtensionTypePtr p_base);
typedef void (*NativeExtensionPtrSetter)(NativeExtensionTypePtr p_base, NativeExtensionConstTypePtr p_value);
typedef void (*NativeExtensionPtrGetter)(NativeExtensionConstTypePtr p_base, NativeExtensionTypePtr r_value);
typedef void (*NativeExtensionPtrIndexedSetter)(NativeExtensionTypePtr p_base, NativeExtensionInt p_index, NativeExtensionConstTypePtr p_value);
typedef void (*NativeExtensionPtrIndexedGetter)(NativeExtensionConstTypePtr p_base, NativeExtensionInt p_index, NativeExtensionTypePtr r_value);
typedef void (*NativeExtensionPtrKeyedSetter)(NativeExtensionTypePtr p_base, NativeExtensionConstTypePtr p_key, NativeExtensionConstTypePtr p_value);
typedef void (*NativeExtensionPtrKeyedGetter)(NativeExtensionConstTypePtr p_base, NativeExtensionConstTypePtr p_key, NativeExtensionTypePtr r_value);
typedef uint32_t (*NativeExtensionPtrKeyedChecker)(NativeExtensionConstVariantPtr p_base, NativeExtensionConstVariantPtr p_key);
typedef void (*NativeExtensionPtrUtilityFunction)(NativeExtensionTypePtr r_return, const NativeExtensionConstTypePtr *p_args, int32_t p_argument_count);
typedef NativeExtensionObjectPtr (*NativeExtensionClassConstructor)();
typedef void *(*NativeExtensionInstanceBindingCreateCallback)(void *p_token, void *p_instance);
typedef void (*NativeExtensionInstanceBindingFreeCallback)(void *p_token, void *p_instance, void *p_binding);
typedef NativeExtensionBool (*NativeExtensionInstanceBindingReferenceCallback)(void *p_token, void *p_binding, NativeExtensionBool p_reference);
typedef struct {
	NativeExtensionInstanceBindingCreateCallback create_callback;
	NativeExtensionInstanceBindingFreeCallback free_callback;
	NativeExtensionInstanceBindingReferenceCallback reference_callback;
} NativeExtensionInstanceBindingCallbacks;

typedef void *NativeExtensionClassInstancePtr;
typedef NativeExtensionBool (*NativeExtensionClassSet)(NativeExtensionClassInstancePtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionConstVariantPtr p_value);
typedef NativeExtensionBool (*NativeExtensionClassGet)(NativeExtensionClassInstancePtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionVariantPtr r_ret);
typedef uint64_t (*NativeExtensionClassGetRID)(NativeExtensionClassInstancePtr p_instance);
typedef struct {
	NativeExtensionVariantType type;
	NativeExtensionStringNamePtr name;
	NativeExtensionStringNamePtr class_name;
	/* Bitfield of the engine's `PropertyHint`. */
	uint32_t hint;
	NativeExtensionStringPtr hint_string;
	/* Bitfield of the engine's `PropertyUsageFlags`. */
	uint32_t usage;
} NativeExtensionPropertyInfo;

typedef struct {
	NativeExtensionStringNamePtr name;
	NativeExtensionPropertyInfo return_value;
	/* Bitfield of `NativeExtensionClassMethodFlags`. */
	uint32_t flags;
	int32_t id;
	/* Arguments: `default_arguments` is an array of size `argument_count`. */
	uint32_t argument_count;
	NativeExtensionPropertyInfo *arguments;
	/* Default arguments: `default_arguments` is an array of size `default_argument_count`. */
	uint32_t default_argument_count;
	NativeExtensionVariantPtr *default_arguments;
} NativeExtensionMethodInfo;

typedef const NativeExtensionPropertyInfo *(*NativeExtensionClassGetPropertyList)(NativeExtensionClassInstancePtr p_instance, uint32_t *r_count);
typedef void (*NativeExtensionClassFreePropertyList)(NativeExtensionClassInstancePtr p_instance, const NativeExtensionPropertyInfo *p_list); /* Deprecated in Gobu 4.3. Use `NativeExtensionClassFreePropertyList2` instead. */
typedef void (*NativeExtensionClassFreePropertyList2)(NativeExtensionClassInstancePtr p_instance, const NativeExtensionPropertyInfo *p_list, uint32_t p_count);
typedef NativeExtensionBool (*NativeExtensionClassPropertyCanRevert)(NativeExtensionClassInstancePtr p_instance, NativeExtensionConstStringNamePtr p_name);
typedef NativeExtensionBool (*NativeExtensionClassPropertyGetRevert)(NativeExtensionClassInstancePtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionVariantPtr r_ret);
typedef NativeExtensionBool (*NativeExtensionClassValidateProperty)(NativeExtensionClassInstancePtr p_instance, NativeExtensionPropertyInfo *p_property);
typedef void (*NativeExtensionClassNotification)(NativeExtensionClassInstancePtr p_instance, int32_t p_what); /* Deprecated in Gobu 4.2. Use `NativeExtensionClassNotification2` instead. */
typedef void (*NativeExtensionClassNotification2)(NativeExtensionClassInstancePtr p_instance, int32_t p_what, NativeExtensionBool p_reversed);
typedef void (*NativeExtensionClassToString)(NativeExtensionClassInstancePtr p_instance, NativeExtensionBool *r_is_valid, NativeExtensionStringPtr p_out);
typedef void (*NativeExtensionClassReference)(NativeExtensionClassInstancePtr p_instance);
typedef void (*NativeExtensionClassUnreference)(NativeExtensionClassInstancePtr p_instance);
typedef void (*NativeExtensionClassCallVirtual)(NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret);
/* Called to construct an instance of the class.
 * For classes descending from RefCounted, the reference count should be zero.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionClassCreateInstance)(void *p_class_userdata); /* Deprecated in Gobu 4.4. Use `NativeExtensionClassCreateInstance3` instead. */
/* Called to construct an instance of the class.
 * For classes descending from RefCounted, the reference count should be zero.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionClassCreateInstance2)(void *p_class_userdata, NativeExtensionBool p_notify_postinitialize); /* Deprecated in Gobu 4.7. Use `NativeExtensionClassCreateInstance3` instead. */
/* Called to construct an instance of the class.
 * For classes descending from RefCounted, the reference count should already be incremented by 1.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionClassCreateInstance3)(void *p_class_userdata, NativeExtensionBool p_notify_postinitialize);
typedef void (*NativeExtensionClassFreeInstance)(void *p_class_userdata, NativeExtensionClassInstancePtr p_instance);
typedef NativeExtensionClassInstancePtr (*NativeExtensionClassRecreateInstance)(void *p_class_userdata, NativeExtensionObjectPtr p_object);
typedef NativeExtensionClassCallVirtual (*NativeExtensionClassGetVirtual)(void *p_class_userdata, NativeExtensionConstStringNamePtr p_name); /* Deprecated in Gobu 4.4. Use `NativeExtensionClassGetVirtual2` instead. */
typedef NativeExtensionClassCallVirtual (*NativeExtensionClassGetVirtual2)(void *p_class_userdata, NativeExtensionConstStringNamePtr p_name, uint32_t p_hash);
typedef void *(*NativeExtensionClassGetVirtualCallData)(void *p_class_userdata, NativeExtensionConstStringNamePtr p_name); /* Deprecated in Gobu 4.4. Use `NativeExtensionClassGetVirtualCallData2` instead. */
typedef void *(*NativeExtensionClassGetVirtualCallData2)(void *p_class_userdata, NativeExtensionConstStringNamePtr p_name, uint32_t p_hash);
typedef void (*NativeExtensionClassCallVirtualWithData)(NativeExtensionClassInstancePtr p_instance, NativeExtensionConstStringNamePtr p_name, void *p_virtual_call_userdata, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret);
typedef struct {
	NativeExtensionBool is_virtual;
	NativeExtensionBool is_abstract;
	NativeExtensionClassSet set_func;
	NativeExtensionClassGet get_func;
	NativeExtensionClassGetPropertyList get_property_list_func;
	NativeExtensionClassFreePropertyList free_property_list_func;
	NativeExtensionClassPropertyCanRevert property_can_revert_func;
	NativeExtensionClassPropertyGetRevert property_get_revert_func;
	NativeExtensionClassNotification notification_func;
	NativeExtensionClassToString to_string_func;
	NativeExtensionClassReference reference_func;
	NativeExtensionClassUnreference unreference_func;
	/* Class constructor. Required unless the class is virtual or abstract. */
	NativeExtensionClassCreateInstance create_instance_func;
	/* Destructor; mandatory. */
	NativeExtensionClassFreeInstance free_instance_func;
	/* Queries a virtual function by name and returns a callback to invoke the requested virtual function. */
	NativeExtensionClassGetVirtual get_virtual_func;
	NativeExtensionClassGetRID get_rid_func;
	/* Per-class user data, later accessible in instance bindings. */
	void *class_userdata;
} NativeExtensionClassCreationInfo; /* Deprecated in Gobu 4.2. Use `NativeExtensionClassCreationInfo6` instead. */

typedef struct {
	NativeExtensionBool is_virtual;
	NativeExtensionBool is_abstract;
	NativeExtensionBool is_exposed;
	NativeExtensionClassSet set_func;
	NativeExtensionClassGet get_func;
	NativeExtensionClassGetPropertyList get_property_list_func;
	NativeExtensionClassFreePropertyList free_property_list_func;
	NativeExtensionClassPropertyCanRevert property_can_revert_func;
	NativeExtensionClassPropertyGetRevert property_get_revert_func;
	NativeExtensionClassValidateProperty validate_property_func;
	NativeExtensionClassNotification2 notification_func;
	NativeExtensionClassToString to_string_func;
	NativeExtensionClassReference reference_func;
	NativeExtensionClassUnreference unreference_func;
	/* Class constructor. Required unless the class is virtual or abstract. */
	NativeExtensionClassCreateInstance create_instance_func;
	/* Destructor; mandatory. */
	NativeExtensionClassFreeInstance free_instance_func;
	NativeExtensionClassRecreateInstance recreate_instance_func;
	/* Queries a virtual function by name and returns a callback to invoke the requested virtual function. */
	NativeExtensionClassGetVirtual get_virtual_func;
	/* Paired with `call_virtual_with_data_func`, this is an alternative to `get_virtual_func` for extensions that
	 * need or benefit from extra data when calling virtual functions.
	 * Returns user data that will be passed to `call_virtual_with_data_func`.
	 * Returning `NULL` from this function signals to Gobu that the virtual function is not overridden.
	 * Data returned from this function should be managed by the extension and must be valid until the extension is deinitialized.
	 * You should supply either `get_virtual_func`, or `get_virtual_call_data_func` with `call_virtual_with_data_func`.
	 */
	NativeExtensionClassGetVirtualCallData get_virtual_call_data_func;
	/* Used to call virtual functions when `get_virtual_call_data_func` is not null. */
	NativeExtensionClassCallVirtualWithData call_virtual_with_data_func;
	NativeExtensionClassGetRID get_rid_func;
	/* Per-class user data, later accessible in instance bindings. */
	void *class_userdata;
} NativeExtensionClassCreationInfo2; /* Deprecated in Gobu 4.3. Use `NativeExtensionClassCreationInfo6` instead. */

typedef struct {
	NativeExtensionBool is_virtual;
	NativeExtensionBool is_abstract;
	NativeExtensionBool is_exposed;
	NativeExtensionBool is_runtime;
	NativeExtensionClassSet set_func;
	NativeExtensionClassGet get_func;
	NativeExtensionClassGetPropertyList get_property_list_func;
	NativeExtensionClassFreePropertyList2 free_property_list_func;
	NativeExtensionClassPropertyCanRevert property_can_revert_func;
	NativeExtensionClassPropertyGetRevert property_get_revert_func;
	NativeExtensionClassValidateProperty validate_property_func;
	NativeExtensionClassNotification2 notification_func;
	NativeExtensionClassToString to_string_func;
	NativeExtensionClassReference reference_func;
	NativeExtensionClassUnreference unreference_func;
	/* Class constructor. Required unless the class is virtual or abstract. */
	NativeExtensionClassCreateInstance create_instance_func;
	/* Destructor; mandatory. */
	NativeExtensionClassFreeInstance free_instance_func;
	NativeExtensionClassRecreateInstance recreate_instance_func;
	/* Queries a virtual function by name and returns a callback to invoke the requested virtual function. */
	NativeExtensionClassGetVirtual get_virtual_func;
	/* Paired with `call_virtual_with_data_func`, this is an alternative to `get_virtual_func` for extensions that
	 * need or benefit from extra data when calling virtual functions.
	 * Returns user data that will be passed to `call_virtual_with_data_func`.
	 * Returning `NULL` from this function signals to Gobu that the virtual function is not overridden.
	 * Data returned from this function should be managed by the extension and must be valid until the extension is deinitialized.
	 * You should supply either `get_virtual_func`, or `get_virtual_call_data_func` with `call_virtual_with_data_func`.
	 */
	NativeExtensionClassGetVirtualCallData get_virtual_call_data_func;
	/* Used to call virtual functions when `get_virtual_call_data_func` is not null. */
	NativeExtensionClassCallVirtualWithData call_virtual_with_data_func;
	NativeExtensionClassGetRID get_rid_func;
	/* Per-class user data, later accessible in instance bindings. */
	void *class_userdata;
} NativeExtensionClassCreationInfo3; /* Deprecated in Gobu 4.4. Use `NativeExtensionClassCreationInfo6` instead. */

typedef struct {
	NativeExtensionBool is_virtual;
	NativeExtensionBool is_abstract;
	NativeExtensionBool is_exposed;
	NativeExtensionBool is_runtime;
	NativeExtensionConstStringPtr icon_path;
	NativeExtensionClassSet set_func;
	NativeExtensionClassGet get_func;
	NativeExtensionClassGetPropertyList get_property_list_func;
	NativeExtensionClassFreePropertyList2 free_property_list_func;
	NativeExtensionClassPropertyCanRevert property_can_revert_func;
	NativeExtensionClassPropertyGetRevert property_get_revert_func;
	NativeExtensionClassValidateProperty validate_property_func;
	NativeExtensionClassNotification2 notification_func;
	NativeExtensionClassToString to_string_func;
	NativeExtensionClassReference reference_func;
	NativeExtensionClassUnreference unreference_func;
	/* Class constructor. Required unless the class is virtual or abstract. */
	NativeExtensionClassCreateInstance2 create_instance_func;
	/* Destructor; mandatory. */
	NativeExtensionClassFreeInstance free_instance_func;
	NativeExtensionClassRecreateInstance recreate_instance_func;
	/* Queries a virtual function by name and returns a callback to invoke the requested virtual function. */
	NativeExtensionClassGetVirtual2 get_virtual_func;
	/* Paired with `call_virtual_with_data_func`, this is an alternative to `get_virtual_func` for extensions that
	 * need or benefit from extra data when calling virtual functions.
	 * Returns user data that will be passed to `call_virtual_with_data_func`.
	 * Returning `NULL` from this function signals to Gobu that the virtual function is not overridden.
	 * Data returned from this function should be managed by the extension and must be valid until the extension is deinitialized.
	 * You should supply either `get_virtual_func`, or `get_virtual_call_data_func` with `call_virtual_with_data_func`.
	 */
	NativeExtensionClassGetVirtualCallData2 get_virtual_call_data_func;
	/* Used to call virtual functions when `get_virtual_call_data_func` is not null. */
	NativeExtensionClassCallVirtualWithData call_virtual_with_data_func;
	/* Per-class user data, later accessible in instance bindings. */
	void *class_userdata;
} NativeExtensionClassCreationInfo4; /* Deprecated in Gobu 4.5. Use `NativeExtensionClassCreationInfo6` instead. */

typedef NativeExtensionClassCreationInfo4 NativeExtensionClassCreationInfo5; /* Deprecated in Gobu 4.7. Use `NativeExtensionClassCreationInfo6` instead. */
typedef struct {
	NativeExtensionBool is_virtual;
	NativeExtensionBool is_abstract;
	NativeExtensionBool is_exposed;
	NativeExtensionBool is_runtime;
	NativeExtensionConstStringPtr icon_path;
	NativeExtensionClassSet set_func;
	NativeExtensionClassGet get_func;
	NativeExtensionClassGetPropertyList get_property_list_func;
	NativeExtensionClassFreePropertyList2 free_property_list_func;
	NativeExtensionClassPropertyCanRevert property_can_revert_func;
	NativeExtensionClassPropertyGetRevert property_get_revert_func;
	NativeExtensionClassValidateProperty validate_property_func;
	NativeExtensionClassNotification2 notification_func;
	NativeExtensionClassToString to_string_func;
	NativeExtensionClassReference reference_func;
	NativeExtensionClassUnreference unreference_func;
	/* Class constructor. Required unless the class is virtual or abstract. */
	NativeExtensionClassCreateInstance3 create_instance_func;
	/* Destructor; mandatory. */
	NativeExtensionClassFreeInstance free_instance_func;
	NativeExtensionClassRecreateInstance recreate_instance_func;
	/* Queries a virtual function by name and returns a callback to invoke the requested virtual function. */
	NativeExtensionClassGetVirtual2 get_virtual_func;
	/* Paired with `call_virtual_with_data_func`, this is an alternative to `get_virtual_func` for extensions that
	 * need or benefit from extra data when calling virtual functions.
	 * Returns user data that will be passed to `call_virtual_with_data_func`.
	 * Returning `NULL` from this function signals to Gobu that the virtual function is not overridden.
	 * Data returned from this function should be managed by the extension and must be valid until the extension is deinitialized.
	 * You should supply either `get_virtual_func`, or `get_virtual_call_data_func` with `call_virtual_with_data_func`.
	 */
	NativeExtensionClassGetVirtualCallData2 get_virtual_call_data_func;
	/* Used to call virtual functions when `get_virtual_call_data_func` is not null. */
	NativeExtensionClassCallVirtualWithData call_virtual_with_data_func;
	/* Per-class user data, later accessible in instance bindings. */
	void *class_userdata;
} NativeExtensionClassCreationInfo6;

typedef void *NativeExtensionClassLibraryPtr;
/* Passed a pointer to a PackedStringArray that should be filled with the classes that may be used by the NativeExtension. */
typedef void (*NativeExtensionEditorGetClassesUsedCallback)(NativeExtensionTypePtr p_packed_string_array);
typedef enum {
	NATIVEEXTENSION_METHOD_FLAG_NORMAL = 1,
	NATIVEEXTENSION_METHOD_FLAG_EDITOR = 2,
	NATIVEEXTENSION_METHOD_FLAG_CONST = 4,
	NATIVEEXTENSION_METHOD_FLAG_VIRTUAL = 8,
	NATIVEEXTENSION_METHOD_FLAG_VARARG = 16,
	NATIVEEXTENSION_METHOD_FLAG_STATIC = 32,
	NATIVEEXTENSION_METHOD_FLAG_VIRTUAL_REQUIRED = 128,
	NATIVEEXTENSION_METHOD_FLAGS_DEFAULT = 1,
} NativeExtensionClassMethodFlags;

typedef enum {
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_NONE = 0,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT8 = 1,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT16 = 2,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT32 = 3,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT64 = 4,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT8 = 5,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT16 = 6,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT32 = 7,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_UINT64 = 8,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_REAL_IS_FLOAT = 9,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_REAL_IS_DOUBLE = 10,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_CHAR16 = 11,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_CHAR32 = 12,
	NATIVEEXTENSION_METHOD_ARGUMENT_METADATA_OBJECT_IS_REQUIRED = 13,
} NativeExtensionClassMethodArgumentMetadata;

typedef void (*NativeExtensionClassMethodCall)(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error);
typedef void (*NativeExtensionClassMethodValidatedCall)(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionVariantPtr r_return);
typedef void (*NativeExtensionClassMethodPtrCall)(void *method_userdata, NativeExtensionClassInstancePtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret);
typedef struct {
	NativeExtensionStringNamePtr name;
	void *method_userdata;
	NativeExtensionClassMethodCall call_func;
	NativeExtensionClassMethodPtrCall ptrcall_func;
	/* Bitfield of `NativeExtensionClassMethodFlags`. */
	uint32_t method_flags;
	/* If `has_return_value` is false, `return_value_info` and `return_value_metadata` are ignored.
	 *
	 * @todo Consider dropping `has_return_value` and making the other two properties match `NativeExtensionMethodInfo` and `NativeExtensionClassVirtualMethod` for consistency in future version of this struct.
	 */
	NativeExtensionBool has_return_value;
	NativeExtensionPropertyInfo *return_value_info;
	NativeExtensionClassMethodArgumentMetadata return_value_metadata;
	/* Arguments: `arguments_info` and `arguments_metadata` are array of size `argument_count`.
	 * Name and hint information for the argument can be omitted in release builds. Class name should always be present if it applies.
	 *
	 * @todo Consider renaming `arguments_info` to `arguments` for consistency in future version of this struct.
	 */
	uint32_t argument_count;
	NativeExtensionPropertyInfo *arguments_info;
	NativeExtensionClassMethodArgumentMetadata *arguments_metadata;
	/* Default arguments: `default_arguments` is an array of size `default_argument_count`. */
	uint32_t default_argument_count;
	NativeExtensionVariantPtr *default_arguments;
} NativeExtensionClassMethodInfo;

typedef struct {
	NativeExtensionStringNamePtr name;
	/* Bitfield of `NativeExtensionClassMethodFlags`. */
	uint32_t method_flags;
	NativeExtensionPropertyInfo return_value;
	NativeExtensionClassMethodArgumentMetadata return_value_metadata;
	uint32_t argument_count;
	NativeExtensionPropertyInfo *arguments;
	NativeExtensionClassMethodArgumentMetadata *arguments_metadata;
} NativeExtensionClassVirtualMethodInfo;

typedef void (*NativeExtensionCallableCustomCall)(void *callable_userdata, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error);
typedef NativeExtensionBool (*NativeExtensionCallableCustomIsValid)(void *callable_userdata);
typedef void (*NativeExtensionCallableCustomFree)(void *callable_userdata);
typedef uint32_t (*NativeExtensionCallableCustomHash)(void *callable_userdata);
typedef NativeExtensionBool (*NativeExtensionCallableCustomEqual)(void *callable_userdata_a, void *callable_userdata_b);
typedef NativeExtensionBool (*NativeExtensionCallableCustomLessThan)(void *callable_userdata_a, void *callable_userdata_b);
typedef void (*NativeExtensionCallableCustomToString)(void *callable_userdata, NativeExtensionBool *r_is_valid, NativeExtensionStringPtr r_out);
typedef NativeExtensionInt (*NativeExtensionCallableCustomGetArgumentCount)(void *callable_userdata, NativeExtensionBool *r_is_valid);
/* Only `call_func` and `token` are strictly required, however, `object_id` should be passed if its not a static method.
 *
 * `token` should point to an address that uniquely identifies the NativeExtension (for example, the
 * `NativeExtensionClassLibraryPtr` passed to the entry symbol function.
 *
 * `hash_func`, `equal_func`, and `less_than_func` are optional. If not provided both `call_func` and
 * `callable_userdata` together are used as the identity of the callable for hashing and comparison purposes.
 *
 * The hash returned by `hash_func` is cached, `hash_func` will not be called more than once per callable.
 *
 * `is_valid_func` is necessary if the validity of the callable can change before destruction.
 *
 * `free_func` is necessary if `callable_userdata` needs to be cleaned up when the callable is freed.
 */
typedef struct {
	void *callable_userdata;
	void *token;
	NativeExtensionObjectInstanceID object_id;
	NativeExtensionCallableCustomCall call_func;
	NativeExtensionCallableCustomIsValid is_valid_func;
	NativeExtensionCallableCustomFree free_func;
	NativeExtensionCallableCustomHash hash_func;
	NativeExtensionCallableCustomEqual equal_func;
	NativeExtensionCallableCustomLessThan less_than_func;
	NativeExtensionCallableCustomToString to_string_func;
} NativeExtensionCallableCustomInfo; /* Deprecated in Gobu 4.3. Use `NativeExtensionCallableCustomInfo2` instead. */

/* Only `call_func` and `token` are strictly required, however, `object_id` should be passed if its not a static method.
 *
 * `token` should point to an address that uniquely identifies the NativeExtension (for example, the
 * `NativeExtensionClassLibraryPtr` passed to the entry symbol function.
 *
 * `hash_func`, `equal_func`, and `less_than_func` are optional. If not provided both `call_func` and
 * `callable_userdata` together are used as the identity of the callable for hashing and comparison purposes.
 *
 * The hash returned by `hash_func` is cached, `hash_func` will not be called more than once per callable.
 *
 * `is_valid_func` is necessary if the validity of the callable can change before destruction.
 *
 * `free_func` is necessary if `callable_userdata` needs to be cleaned up when the callable is freed.
 */
typedef struct {
	void *callable_userdata;
	void *token;
	NativeExtensionObjectInstanceID object_id;
	NativeExtensionCallableCustomCall call_func;
	NativeExtensionCallableCustomIsValid is_valid_func;
	NativeExtensionCallableCustomFree free_func;
	NativeExtensionCallableCustomHash hash_func;
	NativeExtensionCallableCustomEqual equal_func;
	NativeExtensionCallableCustomLessThan less_than_func;
	NativeExtensionCallableCustomToString to_string_func;
	NativeExtensionCallableCustomGetArgumentCount get_argument_count_func;
} NativeExtensionCallableCustomInfo2;

/* Pointer to custom ScriptInstance native implementation. */
typedef void *NativeExtensionScriptInstanceDataPtr;
typedef NativeExtensionBool (*NativeExtensionScriptInstanceSet)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionConstVariantPtr p_value);
typedef NativeExtensionBool (*NativeExtensionScriptInstanceGet)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionVariantPtr r_ret);
typedef const NativeExtensionPropertyInfo *(*NativeExtensionScriptInstanceGetPropertyList)(NativeExtensionScriptInstanceDataPtr p_instance, uint32_t *r_count);
typedef void (*NativeExtensionScriptInstanceFreePropertyList)(NativeExtensionScriptInstanceDataPtr p_instance, const NativeExtensionPropertyInfo *p_list); /* Deprecated in Gobu 4.3. Use `NativeExtensionScriptInstanceFreePropertyList2` instead. */
typedef void (*NativeExtensionScriptInstanceFreePropertyList2)(NativeExtensionScriptInstanceDataPtr p_instance, const NativeExtensionPropertyInfo *p_list, uint32_t p_count);
typedef NativeExtensionBool (*NativeExtensionScriptInstanceGetClassCategory)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionPropertyInfo *p_class_category);
typedef NativeExtensionVariantType (*NativeExtensionScriptInstanceGetPropertyType)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionBool *r_is_valid);
typedef NativeExtensionBool (*NativeExtensionScriptInstanceValidateProperty)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionPropertyInfo *p_property);
typedef NativeExtensionBool (*NativeExtensionScriptInstancePropertyCanRevert)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionConstStringNamePtr p_name);
typedef NativeExtensionBool (*NativeExtensionScriptInstancePropertyGetRevert)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionVariantPtr r_ret);
typedef NativeExtensionObjectPtr (*NativeExtensionScriptInstanceGetOwner)(NativeExtensionScriptInstanceDataPtr p_instance);
typedef void (*NativeExtensionScriptInstancePropertyStateAdd)(NativeExtensionConstStringNamePtr p_name, NativeExtensionConstVariantPtr p_value, void *p_userdata);
typedef void (*NativeExtensionScriptInstanceGetPropertyState)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionScriptInstancePropertyStateAdd p_add_func, void *p_userdata);
typedef const NativeExtensionMethodInfo *(*NativeExtensionScriptInstanceGetMethodList)(NativeExtensionScriptInstanceDataPtr p_instance, uint32_t *r_count);
typedef void (*NativeExtensionScriptInstanceFreeMethodList)(NativeExtensionScriptInstanceDataPtr p_instance, const NativeExtensionMethodInfo *p_list); /* Deprecated in Gobu 4.3. Use `NativeExtensionScriptInstanceFreeMethodList2` instead. */
typedef void (*NativeExtensionScriptInstanceFreeMethodList2)(NativeExtensionScriptInstanceDataPtr p_instance, const NativeExtensionMethodInfo *p_list, uint32_t p_count);
typedef NativeExtensionBool (*NativeExtensionScriptInstanceHasMethod)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionConstStringNamePtr p_name);
typedef NativeExtensionInt (*NativeExtensionScriptInstanceGetMethodArgumentCount)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionConstStringNamePtr p_name, NativeExtensionBool *r_is_valid);
typedef void (*NativeExtensionScriptInstanceCall)(NativeExtensionScriptInstanceDataPtr p_self, NativeExtensionConstStringNamePtr p_method, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionVariantPtr r_return, NativeExtensionCallError *r_error);
typedef void (*NativeExtensionScriptInstanceNotification)(NativeExtensionScriptInstanceDataPtr p_instance, int32_t p_what); /* Deprecated in Gobu 4.2. Use `NativeExtensionScriptInstanceNotification2` instead. */
typedef void (*NativeExtensionScriptInstanceNotification2)(NativeExtensionScriptInstanceDataPtr p_instance, int32_t p_what, NativeExtensionBool p_reversed);
typedef void (*NativeExtensionScriptInstanceToString)(NativeExtensionScriptInstanceDataPtr p_instance, NativeExtensionBool *r_is_valid, NativeExtensionStringPtr r_out);
typedef void (*NativeExtensionScriptInstanceRefCountIncremented)(NativeExtensionScriptInstanceDataPtr p_instance);
typedef NativeExtensionBool (*NativeExtensionScriptInstanceRefCountDecremented)(NativeExtensionScriptInstanceDataPtr p_instance);
typedef NativeExtensionObjectPtr (*NativeExtensionScriptInstanceGetScript)(NativeExtensionScriptInstanceDataPtr p_instance);
typedef NativeExtensionBool (*NativeExtensionScriptInstanceIsPlaceholder)(NativeExtensionScriptInstanceDataPtr p_instance);
typedef void *NativeExtensionScriptLanguagePtr;
typedef NativeExtensionScriptLanguagePtr (*NativeExtensionScriptInstanceGetLanguage)(NativeExtensionScriptInstanceDataPtr p_instance);
typedef void (*NativeExtensionScriptInstanceFree)(NativeExtensionScriptInstanceDataPtr p_instance);
/* Pointer to ScriptInstance. */
typedef void *NativeExtensionScriptInstancePtr;
typedef struct {
	NativeExtensionScriptInstanceSet set_func;
	NativeExtensionScriptInstanceGet get_func;
	NativeExtensionScriptInstanceGetPropertyList get_property_list_func;
	NativeExtensionScriptInstanceFreePropertyList free_property_list_func;
	NativeExtensionScriptInstancePropertyCanRevert property_can_revert_func;
	NativeExtensionScriptInstancePropertyGetRevert property_get_revert_func;
	NativeExtensionScriptInstanceGetOwner get_owner_func;
	NativeExtensionScriptInstanceGetPropertyState get_property_state_func;
	NativeExtensionScriptInstanceGetMethodList get_method_list_func;
	NativeExtensionScriptInstanceFreeMethodList free_method_list_func;
	NativeExtensionScriptInstanceGetPropertyType get_property_type_func;
	NativeExtensionScriptInstanceHasMethod has_method_func;
	NativeExtensionScriptInstanceCall call_func;
	NativeExtensionScriptInstanceNotification notification_func;
	NativeExtensionScriptInstanceToString to_string_func;
	NativeExtensionScriptInstanceRefCountIncremented refcount_incremented_func;
	NativeExtensionScriptInstanceRefCountDecremented refcount_decremented_func;
	NativeExtensionScriptInstanceGetScript get_script_func;
	NativeExtensionScriptInstanceIsPlaceholder is_placeholder_func;
	NativeExtensionScriptInstanceSet set_fallback_func;
	NativeExtensionScriptInstanceGet get_fallback_func;
	NativeExtensionScriptInstanceGetLanguage get_language_func;
	NativeExtensionScriptInstanceFree free_func;
} NativeExtensionScriptInstanceInfo; /* Deprecated in Gobu 4.2. Use `NativeExtensionScriptInstanceInfo3` instead. */

typedef struct {
	NativeExtensionScriptInstanceSet set_func;
	NativeExtensionScriptInstanceGet get_func;
	NativeExtensionScriptInstanceGetPropertyList get_property_list_func;
	NativeExtensionScriptInstanceFreePropertyList free_property_list_func;
	/* Optional. Set to NULL for the default behavior. */
	NativeExtensionScriptInstanceGetClassCategory get_class_category_func;
	NativeExtensionScriptInstancePropertyCanRevert property_can_revert_func;
	NativeExtensionScriptInstancePropertyGetRevert property_get_revert_func;
	NativeExtensionScriptInstanceGetOwner get_owner_func;
	NativeExtensionScriptInstanceGetPropertyState get_property_state_func;
	NativeExtensionScriptInstanceGetMethodList get_method_list_func;
	NativeExtensionScriptInstanceFreeMethodList free_method_list_func;
	NativeExtensionScriptInstanceGetPropertyType get_property_type_func;
	NativeExtensionScriptInstanceValidateProperty validate_property_func;
	NativeExtensionScriptInstanceHasMethod has_method_func;
	NativeExtensionScriptInstanceCall call_func;
	NativeExtensionScriptInstanceNotification2 notification_func;
	NativeExtensionScriptInstanceToString to_string_func;
	NativeExtensionScriptInstanceRefCountIncremented refcount_incremented_func;
	NativeExtensionScriptInstanceRefCountDecremented refcount_decremented_func;
	NativeExtensionScriptInstanceGetScript get_script_func;
	NativeExtensionScriptInstanceIsPlaceholder is_placeholder_func;
	NativeExtensionScriptInstanceSet set_fallback_func;
	NativeExtensionScriptInstanceGet get_fallback_func;
	NativeExtensionScriptInstanceGetLanguage get_language_func;
	NativeExtensionScriptInstanceFree free_func;
} NativeExtensionScriptInstanceInfo2; /* Deprecated in Gobu 4.3. Use `NativeExtensionScriptInstanceInfo3` instead. */

typedef struct {
	NativeExtensionScriptInstanceSet set_func;
	NativeExtensionScriptInstanceGet get_func;
	NativeExtensionScriptInstanceGetPropertyList get_property_list_func;
	NativeExtensionScriptInstanceFreePropertyList2 free_property_list_func;
	/* Optional. Set to NULL for the default behavior. */
	NativeExtensionScriptInstanceGetClassCategory get_class_category_func;
	NativeExtensionScriptInstancePropertyCanRevert property_can_revert_func;
	NativeExtensionScriptInstancePropertyGetRevert property_get_revert_func;
	NativeExtensionScriptInstanceGetOwner get_owner_func;
	NativeExtensionScriptInstanceGetPropertyState get_property_state_func;
	NativeExtensionScriptInstanceGetMethodList get_method_list_func;
	NativeExtensionScriptInstanceFreeMethodList2 free_method_list_func;
	NativeExtensionScriptInstanceGetPropertyType get_property_type_func;
	NativeExtensionScriptInstanceValidateProperty validate_property_func;
	NativeExtensionScriptInstanceHasMethod has_method_func;
	NativeExtensionScriptInstanceGetMethodArgumentCount get_method_argument_count_func;
	NativeExtensionScriptInstanceCall call_func;
	NativeExtensionScriptInstanceNotification2 notification_func;
	NativeExtensionScriptInstanceToString to_string_func;
	NativeExtensionScriptInstanceRefCountIncremented refcount_incremented_func;
	NativeExtensionScriptInstanceRefCountDecremented refcount_decremented_func;
	NativeExtensionScriptInstanceGetScript get_script_func;
	NativeExtensionScriptInstanceIsPlaceholder is_placeholder_func;
	NativeExtensionScriptInstanceSet set_fallback_func;
	NativeExtensionScriptInstanceGet get_fallback_func;
	NativeExtensionScriptInstanceGetLanguage get_language_func;
	NativeExtensionScriptInstanceFree free_func;
} NativeExtensionScriptInstanceInfo3;

typedef void (*NativeExtensionWorkerThreadPoolGroupTask)(void *, uint32_t);
typedef void (*NativeExtensionWorkerThreadPoolTask)(void *);
typedef enum {
	NATIVEEXTENSION_INITIALIZATION_CORE = 0,
	NATIVEEXTENSION_INITIALIZATION_SERVERS = 1,
	NATIVEEXTENSION_INITIALIZATION_SCENE = 2,
	NATIVEEXTENSION_INITIALIZATION_EDITOR = 3,
	NATIVEEXTENSION_MAX_INITIALIZATION_LEVEL = 4,
} NativeExtensionInitializationLevel;

typedef void (*NativeExtensionInitializeCallback)(void *p_userdata, NativeExtensionInitializationLevel p_level);
typedef void (*NativeExtensionDeinitializeCallback)(void *p_userdata, NativeExtensionInitializationLevel p_level);
typedef struct {
	/* Minimum initialization level required.
	 * If Core or Servers, the extension needs editor or game restart to take effect
	 */
	NativeExtensionInitializationLevel minimum_initialization_level;
	/* Up to the user to supply when initializing */
	void *userdata;
	/* This function will be called multiple times for each initialization level. */
	NativeExtensionInitializeCallback initialize;
	NativeExtensionDeinitializeCallback deinitialize;
} NativeExtensionInitialization;

typedef void (*NativeExtensionInterfaceFunctionPtr)();
typedef NativeExtensionInterfaceFunctionPtr (*NativeExtensionInterfaceGetProcAddress)(const char *p_function_name);
/* Each NativeExtension should define a C function that matches the signature of NativeExtensionInitializationFunction,
 * and export it so that it can be loaded via dlopen() or equivalent for the given platform.
 *
 * For example:
 *
 *   NativeExtensionBool my_extension_init(NativeExtensionInterfaceGetProcAddress p_get_proc_address, NativeExtensionClassLibraryPtr p_library, NativeExtensionInitialization *r_initialization);
 *
 * This function's name must be specified as the 'entry_symbol' in the .nativeextension file.
 *
 * This makes it the entry point of the NativeExtension and will be called on initialization.
 *
 * The NativeExtension can then modify the r_initialization structure, setting the minimum initialization level,
 * and providing pointers to functions that will be called at various stages of initialization/shutdown.
 *
 * The rest of the NativeExtension's interface to Gobu consists of function pointers that can be loaded
 * by calling p_get_proc_address("...") with the name of the function.
 *
 * For example:
 *
 *   NativeExtensionInterfaceGetGobuVersion get_gobu_version = (NativeExtensionInterfaceGetGobuVersion)p_get_proc_address("get_gobu_version");
 *
 * (Note that snippet may cause "cast between incompatible function types" on some compilers, you can
 * silence this by adding an intermediary `void*` cast.)
 *
 * You can then call it like a normal function:
 *
 *   NativeExtensionGobuVersion gobu_version;
 *   get_gobu_version(&gobu_version);
 *   printf("Gobu v%d.%d.%d\n", gobu_version.major, gobu_version.minor, gobu_version.patch);
 *
 * All of these interface functions are described below, together with the name that's used to load it,
 * and the function pointer typedef that shows its signature.
 */
typedef NativeExtensionBool (*NativeExtensionInitializationFunction)(NativeExtensionInterfaceGetProcAddress p_get_proc_address, NativeExtensionClassLibraryPtr p_library, NativeExtensionInitialization *r_initialization);
typedef struct {
	uint32_t major;
	uint32_t minor;
	uint32_t patch;
	const char *string;
} NativeExtensionGobuVersion; /* Deprecated in Gobu 4.5. Use `NativeExtensionGobuVersion2` instead. */

typedef struct {
	uint32_t major;
	uint32_t minor;
	uint32_t patch;
	/* Full version encoded as hexadecimal with one byte (2 hex digits) per number (e.g. for "3.1.12" it would be 0x03010C) */
	uint32_t hex;
	/* (e.g. "stable", "beta", "rc1", "rc2") */
	const char *status;
	/* (e.g. "custom_build") */
	const char *build;
	/* Full Git commit hash. */
	const char *hash;
	/* Git commit date UNIX timestamp in seconds, or 0 if unavailable. */
	uint64_t timestamp;
	/* (e.g. "Gobu v3.1.4.stable.official.mono") */
	const char *string;
} NativeExtensionGobuVersion2;

/* Called when starting the main loop. */
typedef void (*NativeExtensionMainLoopStartupCallback)();
/* Called when shutting down the main loop. */
typedef void (*NativeExtensionMainLoopShutdownCallback)();
/* Called for every frame iteration of the main loop. */
typedef void (*NativeExtensionMainLoopFrameCallback)();
typedef struct {
	/* Will be called after Gobu is started and is fully initialized. */
	NativeExtensionMainLoopStartupCallback startup_func;
	/* Will be called before Gobu is shutdown when it is still fully initialized. */
	NativeExtensionMainLoopShutdownCallback shutdown_func;
	/* Will be called for each process frame. This will run after all `_process()` methods on Node, and before `ScriptServer::frame()`.
	 * This is intended to be the equivalent of `ScriptLanguage::frame()` for NativeExtension language bindings that don't use the script API.
	 */
	NativeExtensionMainLoopFrameCallback frame_func;
} NativeExtensionMainLoopCallbacks;

/**
 * @name get_gobu_version
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.5. Use `get_gobu_version2` instead.
 *
 * Gets the Gobu version that the NativeExtension was loaded into.
 *
 * @param r_gobu_version A pointer to the structure to write the version information into.
 */
typedef void (*NativeExtensionInterfaceGetGobuVersion)(NativeExtensionGobuVersion *r_gobu_version);

/**
 * @name get_gobu_version2
 * @since 4.5
 *
 * Gets the Gobu version that the NativeExtension was loaded into.
 *
 * @param r_gobu_version A pointer to the structure to write the version information into.
 */
typedef void (*NativeExtensionInterfaceGetGobuVersion2)(NativeExtensionGobuVersion2 *r_gobu_version);

/**
 * @name mem_alloc
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.6. Does not allow explicitly requesting padding. Use `mem_alloc2` instead.
 *
 * Allocates memory.
 *
 * @param p_bytes The amount of memory to allocate in bytes.
 *
 * @return A pointer to the allocated memory, or NULL if unsuccessful.
 */
typedef void *(*NativeExtensionInterfaceMemAlloc)(size_t p_bytes);

/**
 * @name mem_realloc
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.6. Does not allow explicitly requesting padding. Use `mem_realloc2` instead.
 *
 * Reallocates memory.
 *
 * @param p_ptr A pointer to the previously allocated memory.
 * @param p_bytes The number of bytes to resize the memory block to.
 *
 * @return A pointer to the allocated memory, or NULL if unsuccessful.
 */
typedef void *(*NativeExtensionInterfaceMemRealloc)(void *p_ptr, size_t p_bytes);

/**
 * @name mem_free
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.6. Does not allow explicitly requesting padding. Use `mem_free2` instead.
 *
 * Frees memory.
 *
 * @param p_ptr A pointer to the previously allocated memory.
 */
typedef void (*NativeExtensionInterfaceMemFree)(void *p_ptr);

/**
 * @name mem_alloc2
 * @since 4.6
 *
 * Allocates memory.
 *
 * @param p_bytes The amount of memory to allocate in bytes.
 * @param p_pad_align If true, the returned memory will have prepadding of at least 8 bytes.
 *
 * @return A pointer to the allocated memory, or NULL if unsuccessful.
 */
typedef void *(*NativeExtensionInterfaceMemAlloc2)(size_t p_bytes, NativeExtensionBool p_pad_align);

/**
 * @name mem_realloc2
 * @since 4.6
 *
 * Reallocates memory.
 *
 * @param p_ptr A pointer to the previously allocated memory.
 * @param p_bytes The number of bytes to resize the memory block to.
 * @param p_pad_align If true, the returned memory will have prepadding of at least 8 bytes.
 *
 * @return A pointer to the allocated memory, or NULL if unsuccessful.
 */
typedef void *(*NativeExtensionInterfaceMemRealloc2)(void *p_ptr, size_t p_bytes, NativeExtensionBool p_pad_align);

/**
 * @name mem_free2
 * @since 4.6
 *
 * Frees memory.
 *
 * @param p_ptr A pointer to the previously allocated memory.
 * @param p_pad_align If true, the given memory was allocated with prepadding.
 */
typedef void (*NativeExtensionInterfaceMemFree2)(void *p_ptr, NativeExtensionBool p_pad_align);

/**
 * @name print_error
 * @since 4.1
 *
 * Logs an error to Gobu's built-in debugger and to the OS terminal.
 *
 * @param p_description The code triggering the error.
 * @param p_function The function name where the error occurred.
 * @param p_file The file where the error occurred.
 * @param p_line The line where the error occurred.
 * @param p_editor_notify Whether or not to notify the editor.
 */
typedef void (*NativeExtensionInterfacePrintError)(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, NativeExtensionBool p_editor_notify);

/**
 * @name print_error_with_message
 * @since 4.1
 *
 * Logs an error with a message to Gobu's built-in debugger and to the OS terminal.
 *
 * @param p_description The code triggering the error.
 * @param p_message The message to show along with the error.
 * @param p_function The function name where the error occurred.
 * @param p_file The file where the error occurred.
 * @param p_line The line where the error occurred.
 * @param p_editor_notify Whether or not to notify the editor.
 */
typedef void (*NativeExtensionInterfacePrintErrorWithMessage)(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, NativeExtensionBool p_editor_notify);

/**
 * @name print_warning
 * @since 4.1
 *
 * Logs a warning to Gobu's built-in debugger and to the OS terminal.
 *
 * @param p_description The code triggering the warning.
 * @param p_function The function name where the warning occurred.
 * @param p_file The file where the warning occurred.
 * @param p_line The line where the warning occurred.
 * @param p_editor_notify Whether or not to notify the editor.
 */
typedef void (*NativeExtensionInterfacePrintWarning)(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, NativeExtensionBool p_editor_notify);

/**
 * @name print_warning_with_message
 * @since 4.1
 *
 * Logs a warning with a message to Gobu's built-in debugger and to the OS terminal.
 *
 * @param p_description The code triggering the warning.
 * @param p_message The message to show along with the warning.
 * @param p_function The function name where the warning occurred.
 * @param p_file The file where the warning occurred.
 * @param p_line The line where the warning occurred.
 * @param p_editor_notify Whether or not to notify the editor.
 */
typedef void (*NativeExtensionInterfacePrintWarningWithMessage)(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, NativeExtensionBool p_editor_notify);

/**
 * @name print_script_error
 * @since 4.1
 *
 * Logs a script error to Gobu's built-in debugger and to the OS terminal.
 *
 * @param p_description The code triggering the error.
 * @param p_function The function name where the error occurred.
 * @param p_file The file where the error occurred.
 * @param p_line The line where the error occurred.
 * @param p_editor_notify Whether or not to notify the editor.
 */
typedef void (*NativeExtensionInterfacePrintScriptError)(const char *p_description, const char *p_function, const char *p_file, int32_t p_line, NativeExtensionBool p_editor_notify);

/**
 * @name print_script_error_with_message
 * @since 4.1
 *
 * Logs a script error with a message to Gobu's built-in debugger and to the OS terminal.
 *
 * @param p_description The code triggering the error.
 * @param p_message The message to show along with the error.
 * @param p_function The function name where the error occurred.
 * @param p_file The file where the error occurred.
 * @param p_line The line where the error occurred.
 * @param p_editor_notify Whether or not to notify the editor.
 */
typedef void (*NativeExtensionInterfacePrintScriptErrorWithMessage)(const char *p_description, const char *p_message, const char *p_function, const char *p_file, int32_t p_line, NativeExtensionBool p_editor_notify);

/**
 * @name get_native_struct_size
 * @since 4.1
 *
 * Gets the size of a native struct (ex. ObjectID) in bytes.
 *
 * @param p_name A pointer to a StringName identifying the struct name.
 *
 * @return The size in bytes.
 */
typedef uint64_t (*NativeExtensionInterfaceGetNativeStructSize)(NativeExtensionConstStringNamePtr p_name);

/**
 * @name variant_new_copy
 * @since 4.1
 *
 * Copies one Variant into a another.
 *
 * @param r_dest A pointer to the destination Variant.
 * @param p_src A pointer to the source Variant.
 */
typedef void (*NativeExtensionInterfaceVariantNewCopy)(NativeExtensionUninitializedVariantPtr r_dest, NativeExtensionConstVariantPtr p_src);

/**
 * @name variant_new_nil
 * @since 4.1
 *
 * Creates a new Variant containing nil.
 *
 * @param r_dest A pointer to the destination Variant.
 */
typedef void (*NativeExtensionInterfaceVariantNewNil)(NativeExtensionUninitializedVariantPtr r_dest);

/**
 * @name variant_destroy
 * @since 4.1
 *
 * Destroys a Variant.
 *
 * @param p_self A pointer to the Variant to destroy.
 */
typedef void (*NativeExtensionInterfaceVariantDestroy)(NativeExtensionVariantPtr p_self);

/**
 * @name variant_call
 * @since 4.1
 *
 * Calls a method on a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param p_method A pointer to a StringName identifying the method.
 * @param p_args A pointer to a C array of Variant.
 * @param p_argument_count The number of arguments.
 * @param r_return A pointer a Variant which will be assigned the return value.
 * @param r_error A pointer the structure which will hold error information.
 *
 * @see Variant::callp()
 */
typedef void (*NativeExtensionInterfaceVariantCall)(NativeExtensionVariantPtr p_self, NativeExtensionConstStringNamePtr p_method, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionUninitializedVariantPtr r_return, NativeExtensionCallError *r_error);

/**
 * @name variant_call_static
 * @since 4.1
 *
 * Calls a static method on a Variant.
 *
 * @param p_type The variant type.
 * @param p_method A pointer to a StringName identifying the method.
 * @param p_args A pointer to a C array of Variant.
 * @param p_argument_count The number of arguments.
 * @param r_return A pointer a Variant which will be assigned the return value.
 * @param r_error A pointer the structure which will be updated with error information.
 *
 * @see Variant::call_static()
 */
typedef void (*NativeExtensionInterfaceVariantCallStatic)(NativeExtensionVariantType p_type, NativeExtensionConstStringNamePtr p_method, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionUninitializedVariantPtr r_return, NativeExtensionCallError *r_error);

/**
 * @name variant_evaluate
 * @since 4.1
 *
 * Evaluate an operator on two Variants.
 *
 * @param p_op The operator to evaluate.
 * @param p_a The first Variant.
 * @param p_b The second Variant.
 * @param r_return A pointer a Variant which will be assigned the return value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 *
 * @see Variant::evaluate()
 */
typedef void (*NativeExtensionInterfaceVariantEvaluate)(NativeExtensionVariantOperator p_op, NativeExtensionConstVariantPtr p_a, NativeExtensionConstVariantPtr p_b, NativeExtensionUninitializedVariantPtr r_return, NativeExtensionBool *r_valid);

/**
 * @name variant_set
 * @since 4.1
 *
 * Sets a key on a Variant to a value.
 *
 * @param p_self A pointer to the Variant.
 * @param p_key A pointer to a Variant representing the key.
 * @param p_value A pointer to a Variant representing the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 *
 * @see Variant::set()
 */
typedef void (*NativeExtensionInterfaceVariantSet)(NativeExtensionVariantPtr p_self, NativeExtensionConstVariantPtr p_key, NativeExtensionConstVariantPtr p_value, NativeExtensionBool *r_valid);

/**
 * @name variant_set_named
 * @since 4.1
 *
 * Sets a named key on a Variant to a value.
 *
 * @param p_self A pointer to the Variant.
 * @param p_key A pointer to a StringName representing the key.
 * @param p_value A pointer to a Variant representing the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 *
 * @see Variant::set_named()
 */
typedef void (*NativeExtensionInterfaceVariantSetNamed)(NativeExtensionVariantPtr p_self, NativeExtensionConstStringNamePtr p_key, NativeExtensionConstVariantPtr p_value, NativeExtensionBool *r_valid);

/**
 * @name variant_set_keyed
 * @since 4.1
 *
 * Sets a keyed property on a Variant to a value.
 *
 * @param p_self A pointer to the Variant.
 * @param p_key A pointer to a Variant representing the key.
 * @param p_value A pointer to a Variant representing the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 *
 * @see Variant::set_keyed()
 */
typedef void (*NativeExtensionInterfaceVariantSetKeyed)(NativeExtensionVariantPtr p_self, NativeExtensionConstVariantPtr p_key, NativeExtensionConstVariantPtr p_value, NativeExtensionBool *r_valid);

/**
 * @name variant_set_indexed
 * @since 4.1
 *
 * Sets an index on a Variant to a value.
 *
 * @param p_self A pointer to the Variant.
 * @param p_index The index.
 * @param p_value A pointer to a Variant representing the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 * @param r_oob A pointer to a boolean which will be set to true if the index is out of bounds.
 */
typedef void (*NativeExtensionInterfaceVariantSetIndexed)(NativeExtensionVariantPtr p_self, NativeExtensionInt p_index, NativeExtensionConstVariantPtr p_value, NativeExtensionBool *r_valid, NativeExtensionBool *r_oob);

/**
 * @name variant_get
 * @since 4.1
 *
 * Gets the value of a key from a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param p_key A pointer to a Variant representing the key.
 * @param r_ret A pointer to a Variant which will be assigned the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 */
typedef void (*NativeExtensionInterfaceVariantGet)(NativeExtensionConstVariantPtr p_self, NativeExtensionConstVariantPtr p_key, NativeExtensionUninitializedVariantPtr r_ret, NativeExtensionBool *r_valid);

/**
 * @name variant_get_named
 * @since 4.1
 *
 * Gets the value of a named key from a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param p_key A pointer to a StringName representing the key.
 * @param r_ret A pointer to a Variant which will be assigned the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 */
typedef void (*NativeExtensionInterfaceVariantGetNamed)(NativeExtensionConstVariantPtr p_self, NativeExtensionConstStringNamePtr p_key, NativeExtensionUninitializedVariantPtr r_ret, NativeExtensionBool *r_valid);

/**
 * @name variant_get_keyed
 * @since 4.1
 *
 * Gets the value of a keyed property from a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param p_key A pointer to a Variant representing the key.
 * @param r_ret A pointer to a Variant which will be assigned the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 */
typedef void (*NativeExtensionInterfaceVariantGetKeyed)(NativeExtensionConstVariantPtr p_self, NativeExtensionConstVariantPtr p_key, NativeExtensionUninitializedVariantPtr r_ret, NativeExtensionBool *r_valid);

/**
 * @name variant_get_indexed
 * @since 4.1
 *
 * Gets the value of an index from a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param p_index The index.
 * @param r_ret A pointer to a Variant which will be assigned the value.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 * @param r_oob A pointer to a boolean which will be set to true if the index is out of bounds.
 */
typedef void (*NativeExtensionInterfaceVariantGetIndexed)(NativeExtensionConstVariantPtr p_self, NativeExtensionInt p_index, NativeExtensionUninitializedVariantPtr r_ret, NativeExtensionBool *r_valid, NativeExtensionBool *r_oob);

/**
 * @name variant_iter_init
 * @since 4.1
 *
 * Initializes an iterator over a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param r_iter A pointer to a Variant which will be assigned the iterator.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 *
 * @return true if the operation is valid; otherwise false.
 *
 * @see Variant::iter_init()
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantIterInit)(NativeExtensionConstVariantPtr p_self, NativeExtensionUninitializedVariantPtr r_iter, NativeExtensionBool *r_valid);

/**
 * @name variant_iter_next
 * @since 4.1
 *
 * Gets the next value for an iterator over a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param r_iter A pointer to a Variant which will be assigned the iterator.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 *
 * @return true if the operation is valid; otherwise false.
 *
 * @see Variant::iter_next()
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantIterNext)(NativeExtensionConstVariantPtr p_self, NativeExtensionVariantPtr r_iter, NativeExtensionBool *r_valid);

/**
 * @name variant_iter_get
 * @since 4.1
 *
 * Gets the next value for an iterator over a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param r_iter A pointer to a Variant which will be assigned the iterator.
 * @param r_ret A pointer to a Variant which will be assigned false if the operation is invalid.
 * @param r_valid A pointer to a boolean which will be set to false if the operation is invalid.
 *
 * @see Variant::iter_get()
 */
typedef void (*NativeExtensionInterfaceVariantIterGet)(NativeExtensionConstVariantPtr p_self, NativeExtensionVariantPtr r_iter, NativeExtensionUninitializedVariantPtr r_ret, NativeExtensionBool *r_valid);

/**
 * @name variant_hash
 * @since 4.1
 *
 * Gets the hash of a Variant.
 *
 * @param p_self A pointer to the Variant.
 *
 * @return The hash value.
 *
 * @see Variant::hash()
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceVariantHash)(NativeExtensionConstVariantPtr p_self);

/**
 * @name variant_recursive_hash
 * @since 4.1
 *
 * Gets the recursive hash of a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param p_recursion_count The number of recursive loops so far.
 *
 * @return The hash value.
 *
 * @see Variant::recursive_hash()
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceVariantRecursiveHash)(NativeExtensionConstVariantPtr p_self, NativeExtensionInt p_recursion_count);

/**
 * @name variant_hash_compare
 * @since 4.1
 *
 * Compares two Variants by their hash.
 *
 * @param p_self A pointer to the Variant.
 * @param p_other A pointer to the other Variant to compare it to.
 *
 * @return The hash value.
 *
 * @see Variant::hash_compare()
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantHashCompare)(NativeExtensionConstVariantPtr p_self, NativeExtensionConstVariantPtr p_other);

/**
 * @name variant_booleanize
 * @since 4.1
 *
 * Converts a Variant to a boolean.
 *
 * @param p_self A pointer to the Variant.
 *
 * @return The boolean value of the Variant.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantBooleanize)(NativeExtensionConstVariantPtr p_self);

/**
 * @name variant_duplicate
 * @since 4.1
 *
 * Duplicates a Variant.
 *
 * @param p_self A pointer to the Variant.
 * @param r_ret A pointer to a Variant to store the duplicated value.
 * @param p_deep Whether or not to duplicate deeply (when supported by the Variant type).
 */
typedef void (*NativeExtensionInterfaceVariantDuplicate)(NativeExtensionConstVariantPtr p_self, NativeExtensionVariantPtr r_ret, NativeExtensionBool p_deep);

/**
 * @name variant_stringify
 * @since 4.1
 *
 * Converts a Variant to a string.
 *
 * @param p_self A pointer to the Variant.
 * @param r_ret A pointer to a String to store the resulting value.
 */
typedef void (*NativeExtensionInterfaceVariantStringify)(NativeExtensionConstVariantPtr p_self, NativeExtensionStringPtr r_ret);

/**
 * @name variant_get_type
 * @since 4.1
 *
 * Gets the type of a Variant.
 *
 * @param p_self A pointer to the Variant.
 *
 * @return The variant type.
 */
typedef NativeExtensionVariantType (*NativeExtensionInterfaceVariantGetType)(NativeExtensionConstVariantPtr p_self);

/**
 * @name variant_has_method
 * @since 4.1
 *
 * Checks if a Variant has the given method.
 *
 * @param p_self A pointer to the Variant.
 * @param p_method A pointer to a StringName with the method name.
 *
 * @return true if the variant has the given method; otherwise false.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantHasMethod)(NativeExtensionConstVariantPtr p_self, NativeExtensionConstStringNamePtr p_method);

/**
 * @name variant_has_member
 * @since 4.1
 *
 * Checks if a type of Variant has the given member.
 *
 * @param p_type The Variant type.
 * @param p_member A pointer to a StringName with the member name.
 *
 * @return true if the variant has the given method; otherwise false.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantHasMember)(NativeExtensionVariantType p_type, NativeExtensionConstStringNamePtr p_member);

/**
 * @name variant_has_key
 * @since 4.1
 *
 * Checks if a Variant has a key.
 *
 * @param p_self A pointer to the Variant.
 * @param p_key A pointer to a Variant representing the key.
 * @param r_valid A pointer to a boolean which will be set to false if the key doesn't exist.
 *
 * @return true if the key exists; otherwise false.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantHasKey)(NativeExtensionConstVariantPtr p_self, NativeExtensionConstVariantPtr p_key, NativeExtensionBool *r_valid);

/**
 * @name variant_get_object_instance_id
 * @since 4.4
 *
 * Gets the object instance ID from a variant of type NATIVEEXTENSION_VARIANT_TYPE_OBJECT.
 *
 * If the variant isn't of type NATIVEEXTENSION_VARIANT_TYPE_OBJECT, then zero will be returned.
 * The instance ID will be returned even if the object is no longer valid - use `object_get_instance_by_id()` to check if the object is still valid.
 *
 * @param p_self A pointer to the Variant.
 *
 * @return The instance ID for the contained object.
 */
typedef NativeExtensionObjectInstanceID (*NativeExtensionInterfaceVariantGetObjectInstanceId)(NativeExtensionConstVariantPtr p_self);

/**
 * @name variant_get_type_name
 * @since 4.1
 *
 * Gets the name of a Variant type.
 *
 * @param p_type The Variant type.
 * @param r_name A pointer to a String to store the Variant type name.
 */
typedef void (*NativeExtensionInterfaceVariantGetTypeName)(NativeExtensionVariantType p_type, NativeExtensionUninitializedStringPtr r_name);

/**
 * @name variant_get_type_by_name
 * @since 4.7
 *
 * Gets the Variant type by name.
 *
 * @param p_type_name The variant type name.
 *
 * @return The variant type for the given name; otherwise VARIANT_MAX if name is invalid.
 */
typedef NativeExtensionVariantType (*NativeExtensionInterfaceVariantGetTypeByName)(NativeExtensionConstStringPtr p_type_name);

/**
 * @name variant_can_convert
 * @since 4.1
 *
 * Checks if Variants can be converted from one type to another.
 *
 * @param p_from The Variant type to convert from.
 * @param p_to The Variant type to convert to.
 *
 * @return true if the conversion is possible; otherwise false.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantCanConvert)(NativeExtensionVariantType p_from, NativeExtensionVariantType p_to);

/**
 * @name variant_can_convert_strict
 * @since 4.1
 *
 * Checks if Variant can be converted from one type to another using stricter rules.
 *
 * @param p_from The Variant type to convert from.
 * @param p_to The Variant type to convert to.
 *
 * @return true if the conversion is possible; otherwise false.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceVariantCanConvertStrict)(NativeExtensionVariantType p_from, NativeExtensionVariantType p_to);

/**
 * @name get_variant_from_type_constructor
 * @since 4.1
 *
 * Gets a pointer to a function that can create a Variant of the given type from a raw value.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function that can create a Variant of the given type from a raw value.
 */
typedef NativeExtensionVariantFromTypeConstructorFunc (*NativeExtensionInterfaceGetVariantFromTypeConstructor)(NativeExtensionVariantType p_type);

/**
 * @name get_variant_to_type_constructor
 * @since 4.1
 *
 * Gets a pointer to a function that can get the raw value from a Variant of the given type.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function that can get the raw value from a Variant of the given type.
 */
typedef NativeExtensionTypeFromVariantConstructorFunc (*NativeExtensionInterfaceGetVariantToTypeConstructor)(NativeExtensionVariantType p_type);

/**
 * @name variant_get_ptr_internal_getter
 * @since 4.4
 *
 * Provides a function pointer for retrieving a pointer to a variant's internal value.
 *
 * Access to a variant's internal value can be used to modify it in-place, or to retrieve its value without the overhead of variant conversion functions.
 * It is recommended to cache the getter for all variant types in a function table to avoid retrieval overhead upon use.
 *
 * Each function assumes the variant's type has already been determined and matches the function.
 * Invoking the function with a variant of a mismatched type has undefined behavior, and may lead to a segmentation fault.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a type-specific function that returns a pointer to the internal value of a variant. Check the implementation of this function (nativeextension_variant_get_ptr_internal_getter) for pointee type info of each variant type.
 */
typedef NativeExtensionVariantGetInternalPtrFunc (*NativeExtensionInterfaceVariantGetPtrInternalGetter)(NativeExtensionVariantType p_type);

/**
 * @name variant_get_ptr_operator_evaluator
 * @since 4.1
 *
 * Gets a pointer to a function that can evaluate the given Variant operator on the given Variant types.
 *
 * @param p_operator The variant operator.
 * @param p_type_a The type of the first Variant.
 * @param p_type_b The type of the second Variant.
 *
 * @return A pointer to a function that can evaluate the given Variant operator on the given Variant types.
 */
typedef NativeExtensionPtrOperatorEvaluator (*NativeExtensionInterfaceVariantGetPtrOperatorEvaluator)(NativeExtensionVariantOperator p_operator, NativeExtensionVariantType p_type_a, NativeExtensionVariantType p_type_b);

/**
 * @name variant_get_ptr_builtin_method
 * @since 4.1
 *
 * Gets a pointer to a function that can call a builtin method on a type of Variant.
 *
 * @param p_type The Variant type.
 * @param p_method A pointer to a StringName with the method name.
 * @param p_hash A hash representing the method signature.
 *
 * @return A pointer to a function that can call a builtin method on a type of Variant.
 */
typedef NativeExtensionPtrBuiltInMethod (*NativeExtensionInterfaceVariantGetPtrBuiltinMethod)(NativeExtensionVariantType p_type, NativeExtensionConstStringNamePtr p_method, NativeExtensionInt p_hash);

/**
 * @name variant_get_ptr_constructor
 * @since 4.1
 *
 * Gets a pointer to a function that can call one of the constructors for a type of Variant.
 *
 * @param p_type The Variant type.
 * @param p_constructor The index of the constructor.
 *
 * @return A pointer to a function that can call one of the constructors for a type of Variant.
 */
typedef NativeExtensionPtrConstructor (*NativeExtensionInterfaceVariantGetPtrConstructor)(NativeExtensionVariantType p_type, int32_t p_constructor);

/**
 * @name variant_get_ptr_destructor
 * @since 4.1
 *
 * Gets a pointer to a function than can call the destructor for a type of Variant.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function than can call the destructor for a type of Variant.
 */
typedef NativeExtensionPtrDestructor (*NativeExtensionInterfaceVariantGetPtrDestructor)(NativeExtensionVariantType p_type);

/**
 * @name variant_construct
 * @since 4.1
 *
 * Constructs a Variant of the given type, using the first constructor that matches the given arguments.
 *
 * @param p_type The Variant type.
 * @param r_base A pointer to a Variant to store the constructed value.
 * @param p_args A pointer to a C array of Variant pointers representing the arguments for the constructor.
 * @param p_argument_count The number of arguments to pass to the constructor.
 * @param r_error A pointer the structure which will be updated with error information.
 */
typedef void (*NativeExtensionInterfaceVariantConstruct)(NativeExtensionVariantType p_type, NativeExtensionUninitializedVariantPtr r_base, const NativeExtensionConstVariantPtr *p_args, int32_t p_argument_count, NativeExtensionCallError *r_error);

/**
 * @name variant_get_ptr_setter
 * @since 4.1
 *
 * Gets a pointer to a function that can call a member's setter on the given Variant type.
 *
 * @param p_type The Variant type.
 * @param p_member A pointer to a StringName with the member name.
 *
 * @return A pointer to a function that can call a member's setter on the given Variant type.
 */
typedef NativeExtensionPtrSetter (*NativeExtensionInterfaceVariantGetPtrSetter)(NativeExtensionVariantType p_type, NativeExtensionConstStringNamePtr p_member);

/**
 * @name variant_get_ptr_getter
 * @since 4.1
 *
 * Gets a pointer to a function that can call a member's getter on the given Variant type.
 *
 * @param p_type The Variant type.
 * @param p_member A pointer to a StringName with the member name.
 *
 * @return A pointer to a function that can call a member's getter on the given Variant type.
 */
typedef NativeExtensionPtrGetter (*NativeExtensionInterfaceVariantGetPtrGetter)(NativeExtensionVariantType p_type, NativeExtensionConstStringNamePtr p_member);

/**
 * @name variant_get_ptr_indexed_setter
 * @since 4.1
 *
 * Gets a pointer to a function that can set an index on the given Variant type.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function that can set an index on the given Variant type.
 */
typedef NativeExtensionPtrIndexedSetter (*NativeExtensionInterfaceVariantGetPtrIndexedSetter)(NativeExtensionVariantType p_type);

/**
 * @name variant_get_ptr_indexed_getter
 * @since 4.1
 *
 * Gets a pointer to a function that can get an index on the given Variant type.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function that can get an index on the given Variant type.
 */
typedef NativeExtensionPtrIndexedGetter (*NativeExtensionInterfaceVariantGetPtrIndexedGetter)(NativeExtensionVariantType p_type);

/**
 * @name variant_get_ptr_keyed_setter
 * @since 4.1
 *
 * Gets a pointer to a function that can set a key on the given Variant type.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function that can set a key on the given Variant type.
 */
typedef NativeExtensionPtrKeyedSetter (*NativeExtensionInterfaceVariantGetPtrKeyedSetter)(NativeExtensionVariantType p_type);

/**
 * @name variant_get_ptr_keyed_getter
 * @since 4.1
 *
 * Gets a pointer to a function that can get a key on the given Variant type.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function that can get a key on the given Variant type.
 */
typedef NativeExtensionPtrKeyedGetter (*NativeExtensionInterfaceVariantGetPtrKeyedGetter)(NativeExtensionVariantType p_type);

/**
 * @name variant_get_ptr_keyed_checker
 * @since 4.1
 *
 * Gets a pointer to a function that can check a key on the given Variant type.
 *
 * @param p_type The Variant type.
 *
 * @return A pointer to a function that can check a key on the given Variant type.
 */
typedef NativeExtensionPtrKeyedChecker (*NativeExtensionInterfaceVariantGetPtrKeyedChecker)(NativeExtensionVariantType p_type);

/**
 * @name variant_get_constant_value
 * @since 4.1
 *
 * Gets the value of a constant from the given Variant type.
 *
 * @param p_type The Variant type.
 * @param p_constant A pointer to a StringName with the constant name.
 * @param r_ret A pointer to a Variant to store the value.
 */
typedef void (*NativeExtensionInterfaceVariantGetConstantValue)(NativeExtensionVariantType p_type, NativeExtensionConstStringNamePtr p_constant, NativeExtensionUninitializedVariantPtr r_ret);

/**
 * @name variant_get_ptr_utility_function
 * @since 4.1
 *
 * Gets a pointer to a function that can call a Variant utility function.
 *
 * @param p_function A pointer to a StringName with the function name.
 * @param p_hash A hash representing the function signature.
 *
 * @return A pointer to a function that can call a Variant utility function.
 */
typedef NativeExtensionPtrUtilityFunction (*NativeExtensionInterfaceVariantGetPtrUtilityFunction)(NativeExtensionConstStringNamePtr p_function, NativeExtensionInt p_hash);

/**
 * @name string_new_with_latin1_chars
 * @since 4.1
 *
 * Creates a String from a Latin-1 encoded C string.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a Latin-1 encoded C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringNewWithLatin1Chars)(NativeExtensionUninitializedStringPtr r_dest, const char *p_contents);

/**
 * @name string_new_with_utf8_chars
 * @since 4.1
 *
 * Creates a String from a UTF-8 encoded C string.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-8 encoded C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringNewWithUtf8Chars)(NativeExtensionUninitializedStringPtr r_dest, const char *p_contents);

/**
 * @name string_new_with_utf16_chars
 * @since 4.1
 *
 * Creates a String from a UTF-16 encoded C string.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-16 encoded C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringNewWithUtf16Chars)(NativeExtensionUninitializedStringPtr r_dest, const char16_t *p_contents);

/**
 * @name string_new_with_utf32_chars
 * @since 4.1
 *
 * Creates a String from a UTF-32 encoded C string.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-32 encoded C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringNewWithUtf32Chars)(NativeExtensionUninitializedStringPtr r_dest, const char32_t *p_contents);

/**
 * @name string_new_with_wide_chars
 * @since 4.1
 *
 * Creates a String from a wide C string.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a wide C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringNewWithWideChars)(NativeExtensionUninitializedStringPtr r_dest, const wchar_t *p_contents);

/**
 * @name string_new_with_latin1_chars_and_len
 * @since 4.1
 *
 * Creates a String from a Latin-1 encoded C string with the given length.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a Latin-1 encoded C string.
 * @param p_size The number of characters (= number of bytes).
 */
typedef void (*NativeExtensionInterfaceStringNewWithLatin1CharsAndLen)(NativeExtensionUninitializedStringPtr r_dest, const char *p_contents, NativeExtensionInt p_size);

/**
 * @name string_new_with_utf8_chars_and_len
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.3. Use `string_new_with_utf8_chars_and_len2` instead.
 *
 * Creates a String from a UTF-8 encoded C string with the given length.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-8 encoded C string.
 * @param p_size The number of bytes (not code units).
 */
typedef void (*NativeExtensionInterfaceStringNewWithUtf8CharsAndLen)(NativeExtensionUninitializedStringPtr r_dest, const char *p_contents, NativeExtensionInt p_size);

/**
 * @name string_new_with_utf8_chars_and_len2
 * @since 4.3
 *
 * Creates a String from a UTF-8 encoded C string with the given length.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-8 encoded C string.
 * @param p_size The number of bytes (not code units).
 *
 * @return Error code signifying if the operation successful.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringNewWithUtf8CharsAndLen2)(NativeExtensionUninitializedStringPtr r_dest, const char *p_contents, NativeExtensionInt p_size);

/**
 * @name string_new_with_utf16_chars_and_len
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.3. Use `string_new_with_utf16_chars_and_len2` instead.
 *
 * Creates a String from a UTF-16 encoded C string with the given length.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-16 encoded C string.
 * @param p_char_count The number of characters (not bytes).
 */
typedef void (*NativeExtensionInterfaceStringNewWithUtf16CharsAndLen)(NativeExtensionUninitializedStringPtr r_dest, const char16_t *p_contents, NativeExtensionInt p_char_count);

/**
 * @name string_new_with_utf16_chars_and_len2
 * @since 4.3
 *
 * Creates a String from a UTF-16 encoded C string with the given length.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-16 encoded C string.
 * @param p_char_count The number of characters (not bytes).
 * @param p_default_little_endian If true, UTF-16 use little endian.
 *
 * @return Error code signifying if the operation successful.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringNewWithUtf16CharsAndLen2)(NativeExtensionUninitializedStringPtr r_dest, const char16_t *p_contents, NativeExtensionInt p_char_count, NativeExtensionBool p_default_little_endian);

/**
 * @name string_new_with_utf32_chars_and_len
 * @since 4.1
 *
 * Creates a String from a UTF-32 encoded C string with the given length.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a UTF-32 encoded C string.
 * @param p_char_count The number of characters (not bytes).
 */
typedef void (*NativeExtensionInterfaceStringNewWithUtf32CharsAndLen)(NativeExtensionUninitializedStringPtr r_dest, const char32_t *p_contents, NativeExtensionInt p_char_count);

/**
 * @name string_new_with_wide_chars_and_len
 * @since 4.1
 *
 * Creates a String from a wide C string with the given length.
 *
 * @param r_dest A pointer to a Variant to hold the newly created String.
 * @param p_contents A pointer to a wide C string.
 * @param p_char_count The number of characters (not bytes).
 */
typedef void (*NativeExtensionInterfaceStringNewWithWideCharsAndLen)(NativeExtensionUninitializedStringPtr r_dest, const wchar_t *p_contents, NativeExtensionInt p_char_count);

/**
 * @name string_to_latin1_chars
 * @since 4.1
 *
 * Converts a String to a Latin-1 encoded C string.
 *
 * It doesn't write a null terminator.
 *
 * @param p_self A pointer to the String.
 * @param r_text A pointer to the buffer to hold the resulting data. If NULL is passed in, only the length will be computed.
 * @param p_max_write_length The maximum number of characters that can be written to r_text. It has no affect on the return value.
 *
 * @return The resulting encoded string length in characters, not including a null terminator. Characters that cannot be converted to Latin-1 are replaced with a space.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringToLatin1Chars)(NativeExtensionConstStringPtr p_self, char *r_text, NativeExtensionInt p_max_write_length);

/**
 * @name string_to_utf8_chars
 * @since 4.1
 *
 * Converts a String to a UTF-8 encoded C string.
 *
 * It doesn't write a null terminator.
 *
 * @param p_self A pointer to the String.
 * @param r_text A pointer to the buffer to hold the resulting data. If NULL is passed in, only the length will be computed.
 * @param p_max_write_length The maximum number of characters that can be written to r_text. It has no affect on the return value.
 *
 * @return The resulting encoded string length in bytes (not characters), not including a null terminator.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringToUtf8Chars)(NativeExtensionConstStringPtr p_self, char *r_text, NativeExtensionInt p_max_write_length);

/**
 * @name string_to_utf16_chars
 * @since 4.1
 *
 * Converts a String to a UTF-16 encoded C string.
 *
 * It doesn't write a null terminator.
 *
 * @param p_self A pointer to the String.
 * @param r_text A pointer to the buffer to hold the resulting data. If NULL is passed in, only the length will be computed.
 * @param p_max_write_length The maximum number of characters that can be written to r_text. It has no affect on the return value.
 *
 * @return The resulting encoded string length in 16-bit code units (not bytes or characters), not including a null terminator.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringToUtf16Chars)(NativeExtensionConstStringPtr p_self, char16_t *r_text, NativeExtensionInt p_max_write_length);

/**
 * @name string_to_utf32_chars
 * @since 4.1
 *
 * Converts a String to a UTF-32 encoded C string.
 *
 * It doesn't write a null terminator.
 *
 * @param p_self A pointer to the String.
 * @param r_text A pointer to the buffer to hold the resulting data. If NULL is passed in, only the length will be computed.
 * @param p_max_write_length The maximum number of characters that can be written to r_text. It has no affect on the return value.
 *
 * @return The resulting encoded string length in characters (not bytes), not including a null terminator.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringToUtf32Chars)(NativeExtensionConstStringPtr p_self, char32_t *r_text, NativeExtensionInt p_max_write_length);

/**
 * @name string_to_wide_chars
 * @since 4.1
 *
 * Converts a String to a wide C string.
 *
 * It doesn't write a null terminator.
 *
 * @param p_self A pointer to the String.
 * @param r_text A pointer to the buffer to hold the resulting data. If NULL is passed in, only the length will be computed.
 * @param p_max_write_length The maximum number of characters that can be written to r_text. It has no affect on the return value.
 *
 * @return The resulting encoded string length in characters (for UTF-32) or 16-bit code units (for UTF-16), depending on the wchar_t representation. Does not include a null terminator.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringToWideChars)(NativeExtensionConstStringPtr p_self, wchar_t *r_text, NativeExtensionInt p_max_write_length);

/**
 * @name string_operator_index
 * @since 4.1
 *
 * Gets a pointer to the character at the given index from a String.
 *
 * @param p_self A pointer to the String.
 * @param p_index The index.
 *
 * @return A pointer to the requested character.
 */
typedef char32_t *(*NativeExtensionInterfaceStringOperatorIndex)(NativeExtensionStringPtr p_self, NativeExtensionInt p_index);

/**
 * @name string_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to the character at the given index from a String.
 *
 * @param p_self A pointer to the String.
 * @param p_index The index.
 *
 * @return A const pointer to the requested character.
 */
typedef const char32_t *(*NativeExtensionInterfaceStringOperatorIndexConst)(NativeExtensionConstStringPtr p_self, NativeExtensionInt p_index);

/**
 * @name string_operator_plus_eq_string
 * @since 4.1
 *
 * Appends another String to a String.
 *
 * @param p_self A pointer to the String.
 * @param p_b A pointer to the other String to append.
 */
typedef void (*NativeExtensionInterfaceStringOperatorPlusEqString)(NativeExtensionStringPtr p_self, NativeExtensionConstStringPtr p_b);

/**
 * @name string_operator_plus_eq_char
 * @since 4.1
 *
 * Appends a character to a String.
 *
 * @param p_self A pointer to the String.
 * @param p_b A pointer to the character to append.
 */
typedef void (*NativeExtensionInterfaceStringOperatorPlusEqChar)(NativeExtensionStringPtr p_self, char32_t p_b);

/**
 * @name string_operator_plus_eq_cstr
 * @since 4.1
 *
 * Appends a Latin-1 encoded C string to a String.
 *
 * @param p_self A pointer to the String.
 * @param p_b A pointer to a Latin-1 encoded C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringOperatorPlusEqCstr)(NativeExtensionStringPtr p_self, const char *p_b);

/**
 * @name string_operator_plus_eq_wcstr
 * @since 4.1
 *
 * Appends a wide C string to a String.
 *
 * @param p_self A pointer to the String.
 * @param p_b A pointer to a wide C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringOperatorPlusEqWcstr)(NativeExtensionStringPtr p_self, const wchar_t *p_b);

/**
 * @name string_operator_plus_eq_c32str
 * @since 4.1
 *
 * Appends a UTF-32 encoded C string to a String.
 *
 * @param p_self A pointer to the String.
 * @param p_b A pointer to a UTF-32 encoded C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceStringOperatorPlusEqC32str)(NativeExtensionStringPtr p_self, const char32_t *p_b);

/**
 * @name string_resize
 * @since 4.2
 *
 * Resizes the underlying string data to the given number of characters.
 *
 * Space needs to be allocated for the null terminating character ('\0') which
 * also must be added manually, in order for all string functions to work correctly.
 *
 * Warning: This is an error-prone operation - only use it if there's no other
 * efficient way to accomplish your goal.
 *
 * @param p_self A pointer to the String.
 * @param p_resize The new length for the String.
 *
 * @return Error code signifying if the operation successful.
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceStringResize)(NativeExtensionStringPtr p_self, NativeExtensionInt p_resize);

/**
 * @name string_name_new_with_latin1_chars
 * @since 4.2
 *
 * Creates a StringName from a Latin-1 encoded C string.
 *
 * If `p_is_static` is true, then:
 * - The StringName will reuse the `p_contents` buffer instead of copying it.
 * - You must guarantee that the buffer remains valid for the duration of the application (e.g. string literal).
 * - You must not call a destructor for this StringName. Incrementing the initial reference once should achieve this.
 *
 * `p_is_static` is purely an optimization and can easily introduce undefined behavior if used wrong. In case of doubt, set it to false.
 *
 * @param r_dest A pointer to uninitialized storage, into which the newly created StringName is constructed.
 * @param p_contents A pointer to a C string (null terminated and Latin-1 or ASCII encoded).
 * @param p_is_static Whether the StringName reuses the buffer directly (see above).
 */
typedef void (*NativeExtensionInterfaceStringNameNewWithLatin1Chars)(NativeExtensionUninitializedStringNamePtr r_dest, const char *p_contents, NativeExtensionBool p_is_static);

/**
 * @name string_name_new_with_utf8_chars
 * @since 4.2
 *
 * Creates a StringName from a UTF-8 encoded C string.
 *
 * @param r_dest A pointer to uninitialized storage, into which the newly created StringName is constructed.
 * @param p_contents A pointer to a C string (null terminated and UTF-8 encoded).
 */
typedef void (*NativeExtensionInterfaceStringNameNewWithUtf8Chars)(NativeExtensionUninitializedStringNamePtr r_dest, const char *p_contents);

/**
 * @name string_name_new_with_utf8_chars_and_len
 * @since 4.2
 *
 * Creates a StringName from a UTF-8 encoded string with a given number of characters.
 *
 * @param r_dest A pointer to uninitialized storage, into which the newly created StringName is constructed.
 * @param p_contents A pointer to a C string (null terminated and UTF-8 encoded).
 * @param p_size The number of bytes (not UTF-8 code points).
 */
typedef void (*NativeExtensionInterfaceStringNameNewWithUtf8CharsAndLen)(NativeExtensionUninitializedStringNamePtr r_dest, const char *p_contents, NativeExtensionInt p_size);

/**
 * @name xml_parser_open_buffer
 * @since 4.1
 *
 * Opens a raw XML buffer on an XMLParser instance.
 *
 * @param p_instance A pointer to an XMLParser object.
 * @param p_buffer A pointer to the buffer.
 * @param p_size The size of the buffer.
 *
 * @return A Gobu error code (ex. OK, ERR_INVALID_DATA, etc).
 *
 * @see XMLParser::open_buffer()
 */
typedef NativeExtensionInt (*NativeExtensionInterfaceXmlParserOpenBuffer)(NativeExtensionObjectPtr p_instance, const uint8_t *p_buffer, size_t p_size);

/**
 * @name file_access_store_buffer
 * @since 4.1
 *
 * Stores the given buffer using an instance of FileAccess.
 *
 * @param p_instance A pointer to a FileAccess object.
 * @param p_src A pointer to the buffer.
 * @param p_length The size of the buffer.
 *
 * @see FileAccess::store_buffer()
 */
typedef void (*NativeExtensionInterfaceFileAccessStoreBuffer)(NativeExtensionObjectPtr p_instance, const uint8_t *p_src, uint64_t p_length);

/**
 * @name file_access_get_buffer
 * @since 4.1
 *
 * Reads the next p_length bytes into the given buffer using an instance of FileAccess.
 *
 * @param p_instance A pointer to a FileAccess object.
 * @param p_dst A pointer to the buffer to store the data.
 * @param p_length The requested number of bytes to read.
 *
 * @return The actual number of bytes read (may be less than requested).
 */
typedef uint64_t (*NativeExtensionInterfaceFileAccessGetBuffer)(NativeExtensionConstObjectPtr p_instance, uint8_t *p_dst, uint64_t p_length);

/**
 * @name image_ptrw
 * @since 4.3
 *
 * Returns writable pointer to internal Image buffer.
 *
 * @param p_instance A pointer to a Image object.
 *
 * @return Pointer to internal Image buffer.
 *
 * @see Image::ptrw()
 */
typedef uint8_t *(*NativeExtensionInterfaceImagePtrw)(NativeExtensionObjectPtr p_instance);

/**
 * @name image_ptr
 * @since 4.3
 *
 * Returns read only pointer to internal Image buffer.
 *
 * @param p_instance A pointer to a Image object.
 *
 * @return Pointer to internal Image buffer.
 *
 * @see Image::ptr()
 */
typedef const uint8_t *(*NativeExtensionInterfaceImagePtr)(NativeExtensionObjectPtr p_instance);

/**
 * @name worker_thread_pool_add_native_group_task
 * @since 4.1
 *
 * Adds a group task to an instance of WorkerThreadPool.
 *
 * @param p_instance A pointer to a WorkerThreadPool object.
 * @param p_func A pointer to a function to run in the thread pool.
 * @param p_userdata A pointer to arbitrary data which will be passed to p_func.
 * @param p_elements The number of element needed in the group.
 * @param p_tasks The number of tasks needed in the group.
 * @param p_high_priority Whether or not this is a high priority task.
 * @param p_description A pointer to a String with the task description.
 *
 * @return The task group ID.
 *
 * @see WorkerThreadPool::add_group_task()
 */
typedef int64_t (*NativeExtensionInterfaceWorkerThreadPoolAddNativeGroupTask)(NativeExtensionObjectPtr p_instance, NativeExtensionWorkerThreadPoolGroupTask p_func, void *p_userdata, int32_t p_elements, int32_t p_tasks, NativeExtensionBool p_high_priority, NativeExtensionConstStringPtr p_description);

/**
 * @name worker_thread_pool_add_native_task
 * @since 4.1
 *
 * Adds a task to an instance of WorkerThreadPool.
 *
 * @param p_instance A pointer to a WorkerThreadPool object.
 * @param p_func A pointer to a function to run in the thread pool.
 * @param p_userdata A pointer to arbitrary data which will be passed to p_func.
 * @param p_high_priority Whether or not this is a high priority task.
 * @param p_description A pointer to a String with the task description.
 *
 * @return The task ID.
 */
typedef int64_t (*NativeExtensionInterfaceWorkerThreadPoolAddNativeTask)(NativeExtensionObjectPtr p_instance, NativeExtensionWorkerThreadPoolTask p_func, void *p_userdata, NativeExtensionBool p_high_priority, NativeExtensionConstStringPtr p_description);

/**
 * @name packed_byte_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a byte in a PackedByteArray.
 *
 * @param p_self A pointer to a PackedByteArray object.
 * @param p_index The index of the byte to get.
 *
 * @return A pointer to the requested byte.
 */
typedef uint8_t *(*NativeExtensionInterfacePackedByteArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_byte_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a byte in a PackedByteArray.
 *
 * @param p_self A const pointer to a PackedByteArray object.
 * @param p_index The index of the byte to get.
 *
 * @return A const pointer to the requested byte.
 */
typedef const uint8_t *(*NativeExtensionInterfacePackedByteArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_float32_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a 32-bit float in a PackedFloat32Array.
 *
 * @param p_self A pointer to a PackedFloat32Array object.
 * @param p_index The index of the float to get.
 *
 * @return A pointer to the requested 32-bit float.
 */
typedef float *(*NativeExtensionInterfacePackedFloat32ArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_float32_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a 32-bit float in a PackedFloat32Array.
 *
 * @param p_self A const pointer to a PackedFloat32Array object.
 * @param p_index The index of the float to get.
 *
 * @return A const pointer to the requested 32-bit float.
 */
typedef const float *(*NativeExtensionInterfacePackedFloat32ArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_float64_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a 64-bit float in a PackedFloat64Array.
 *
 * @param p_self A pointer to a PackedFloat64Array object.
 * @param p_index The index of the float to get.
 *
 * @return A pointer to the requested 64-bit float.
 */
typedef double *(*NativeExtensionInterfacePackedFloat64ArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_float64_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a 64-bit float in a PackedFloat64Array.
 *
 * @param p_self A const pointer to a PackedFloat64Array object.
 * @param p_index The index of the float to get.
 *
 * @return A const pointer to the requested 64-bit float.
 */
typedef const double *(*NativeExtensionInterfacePackedFloat64ArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_int32_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a 32-bit integer in a PackedInt32Array.
 *
 * @param p_self A pointer to a PackedInt32Array object.
 * @param p_index The index of the integer to get.
 *
 * @return A pointer to the requested 32-bit integer.
 */
typedef int32_t *(*NativeExtensionInterfacePackedInt32ArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_int32_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a 32-bit integer in a PackedInt32Array.
 *
 * @param p_self A const pointer to a PackedInt32Array object.
 * @param p_index The index of the integer to get.
 *
 * @return A const pointer to the requested 32-bit integer.
 */
typedef const int32_t *(*NativeExtensionInterfacePackedInt32ArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_int64_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a 64-bit integer in a PackedInt64Array.
 *
 * @param p_self A pointer to a PackedInt64Array object.
 * @param p_index The index of the integer to get.
 *
 * @return A pointer to the requested 64-bit integer.
 */
typedef int64_t *(*NativeExtensionInterfacePackedInt64ArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_int64_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a 64-bit integer in a PackedInt64Array.
 *
 * @param p_self A const pointer to a PackedInt64Array object.
 * @param p_index The index of the integer to get.
 *
 * @return A const pointer to the requested 64-bit integer.
 */
typedef const int64_t *(*NativeExtensionInterfacePackedInt64ArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_string_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a string in a PackedStringArray.
 *
 * @param p_self A pointer to a PackedStringArray object.
 * @param p_index The index of the String to get.
 *
 * @return A pointer to the requested String.
 */
typedef NativeExtensionStringPtr (*NativeExtensionInterfacePackedStringArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_string_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a string in a PackedStringArray.
 *
 * @param p_self A const pointer to a PackedStringArray object.
 * @param p_index The index of the String to get.
 *
 * @return A const pointer to the requested String.
 */
typedef NativeExtensionStringPtr (*NativeExtensionInterfacePackedStringArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_vector2_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a Vector2 in a PackedVector2Array.
 *
 * @param p_self A pointer to a PackedVector2Array object.
 * @param p_index The index of the Vector2 to get.
 *
 * @return A pointer to the requested Vector2.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedVector2ArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_vector2_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a Vector2 in a PackedVector2Array.
 *
 * @param p_self A const pointer to a PackedVector2Array object.
 * @param p_index The index of the Vector2 to get.
 *
 * @return A const pointer to the requested Vector2.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedVector2ArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_vector3_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a Vector3 in a PackedVector3Array.
 *
 * @param p_self A pointer to a PackedVector3Array object.
 * @param p_index The index of the Vector3 to get.
 *
 * @return A pointer to the requested Vector3.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedVector3ArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_vector3_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a Vector3 in a PackedVector3Array.
 *
 * @param p_self A const pointer to a PackedVector3Array object.
 * @param p_index The index of the Vector3 to get.
 *
 * @return A const pointer to the requested Vector3.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedVector3ArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_vector4_array_operator_index
 * @since 4.3
 *
 * Gets a pointer to a Vector4 in a PackedVector4Array.
 *
 * @param p_self A pointer to a PackedVector4Array object.
 * @param p_index The index of the Vector4 to get.
 *
 * @return A pointer to the requested Vector4.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedVector4ArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_vector4_array_operator_index_const
 * @since 4.3
 *
 * Gets a const pointer to a Vector4 in a PackedVector4Array.
 *
 * @param p_self A const pointer to a PackedVector4Array object.
 * @param p_index The index of the Vector4 to get.
 *
 * @return A const pointer to the requested Vector4.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedVector4ArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_color_array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a color in a PackedColorArray.
 *
 * @param p_self A pointer to a PackedColorArray object.
 * @param p_index The index of the Color to get.
 *
 * @return A pointer to the requested Color.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedColorArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name packed_color_array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a color in a PackedColorArray.
 *
 * @param p_self A const pointer to a PackedColorArray object.
 * @param p_index The index of the Color to get.
 *
 * @return A const pointer to the requested Color.
 */
typedef NativeExtensionTypePtr (*NativeExtensionInterfacePackedColorArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name array_operator_index
 * @since 4.1
 *
 * Gets a pointer to a Variant in an Array.
 *
 * @param p_self A pointer to an Array object.
 * @param p_index The index of the Variant to get.
 *
 * @return A pointer to the requested Variant.
 */
typedef NativeExtensionVariantPtr (*NativeExtensionInterfaceArrayOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name array_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a Variant in an Array.
 *
 * @param p_self A const pointer to an Array object.
 * @param p_index The index of the Variant to get.
 *
 * @return A const pointer to the requested Variant.
 */
typedef NativeExtensionVariantPtr (*NativeExtensionInterfaceArrayOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionInt p_index);

/**
 * @name array_ref
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.5. Removed from interface. Use copy constructor instead.
 *
 * Sets an Array to be a reference to another Array object.
 *
 * @param p_self A pointer to the Array object to update.
 * @param p_from A pointer to the Array object to reference.
 */
typedef void (*NativeExtensionInterfaceArrayRef)(NativeExtensionTypePtr p_self, NativeExtensionConstTypePtr p_from);

/**
 * @name array_set_typed
 * @since 4.1
 *
 * Makes an Array into a typed Array.
 *
 * @param p_self A pointer to the Array.
 * @param p_type The type of Variant the Array will store.
 * @param p_class_name A pointer to a StringName with the name of the object (if p_type is NATIVEEXTENSION_VARIANT_TYPE_OBJECT).
 * @param p_script A pointer to a Script object (if p_type is NATIVEEXTENSION_VARIANT_TYPE_OBJECT and the base class is extended by a script).
 */
typedef void (*NativeExtensionInterfaceArraySetTyped)(NativeExtensionTypePtr p_self, NativeExtensionVariantType p_type, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstVariantPtr p_script);

/**
 * @name dictionary_operator_index
 * @since 4.1
 *
 * Gets a pointer to a Variant in a Dictionary with the given key.
 *
 * @param p_self A pointer to a Dictionary object.
 * @param p_key A pointer to a Variant representing the key.
 *
 * @return A pointer to a Variant representing the value at the given key.
 */
typedef NativeExtensionVariantPtr (*NativeExtensionInterfaceDictionaryOperatorIndex)(NativeExtensionTypePtr p_self, NativeExtensionConstVariantPtr p_key);

/**
 * @name dictionary_operator_index_const
 * @since 4.1
 *
 * Gets a const pointer to a Variant in a Dictionary with the given key.
 *
 * @param p_self A const pointer to a Dictionary object.
 * @param p_key A pointer to a Variant representing the key.
 *
 * @return A const pointer to a Variant representing the value at the given key.
 */
typedef NativeExtensionVariantPtr (*NativeExtensionInterfaceDictionaryOperatorIndexConst)(NativeExtensionConstTypePtr p_self, NativeExtensionConstVariantPtr p_key);

/**
 * @name dictionary_set_typed
 * @since 4.4
 *
 * Makes a Dictionary into a typed Dictionary.
 *
 * @param p_self A pointer to the Dictionary.
 * @param p_key_type The type of Variant the Dictionary key will store.
 * @param p_key_class_name A pointer to a StringName with the name of the object (if p_key_type is NATIVEEXTENSION_VARIANT_TYPE_OBJECT).
 * @param p_key_script A pointer to a Script object (if p_key_type is NATIVEEXTENSION_VARIANT_TYPE_OBJECT and the base class is extended by a script).
 * @param p_value_type The type of Variant the Dictionary value will store.
 * @param p_value_class_name A pointer to a StringName with the name of the object (if p_value_type is NATIVEEXTENSION_VARIANT_TYPE_OBJECT).
 * @param p_value_script A pointer to a Script object (if p_value_type is NATIVEEXTENSION_VARIANT_TYPE_OBJECT and the base class is extended by a script).
 */
typedef void (*NativeExtensionInterfaceDictionarySetTyped)(NativeExtensionTypePtr p_self, NativeExtensionVariantType p_key_type, NativeExtensionConstStringNamePtr p_key_class_name, NativeExtensionConstVariantPtr p_key_script, NativeExtensionVariantType p_value_type, NativeExtensionConstStringNamePtr p_value_class_name, NativeExtensionConstVariantPtr p_value_script);

/**
 * @name object_method_bind_call
 * @since 4.1
 *
 * Calls a method on an Object.
 *
 * @param p_method_bind A pointer to the MethodBind representing the method on the Object's class.
 * @param p_instance A pointer to the Object.
 * @param p_args A pointer to a C array of Variants representing the arguments.
 * @param p_arg_count The number of arguments.
 * @param r_ret A pointer to Variant which will receive the return value.
 * @param r_error A pointer to a NativeExtensionCallError struct that will receive error information.
 */
typedef void (*NativeExtensionInterfaceObjectMethodBindCall)(NativeExtensionMethodBindPtr p_method_bind, NativeExtensionObjectPtr p_instance, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_arg_count, NativeExtensionUninitializedVariantPtr r_ret, NativeExtensionCallError *r_error);

/**
 * @name object_method_bind_ptrcall
 * @since 4.1
 *
 * Calls a method on an Object (using a "ptrcall").
 *
 * @param p_method_bind A pointer to the MethodBind representing the method on the Object's class.
 * @param p_instance A pointer to the Object.
 * @param p_args A pointer to a C array representing the arguments.
 * @param r_ret A pointer to the Object that will receive the return value.
 */
typedef void (*NativeExtensionInterfaceObjectMethodBindPtrcall)(NativeExtensionMethodBindPtr p_method_bind, NativeExtensionObjectPtr p_instance, const NativeExtensionConstTypePtr *p_args, NativeExtensionTypePtr r_ret);

/**
 * @name object_destroy
 * @since 4.1
 *
 * Destroys an Object.
 *
 * @param p_o A pointer to the Object.
 */
typedef void (*NativeExtensionInterfaceObjectDestroy)(NativeExtensionObjectPtr p_o);

/**
 * @name global_get_singleton
 * @since 4.1
 *
 * Gets a global singleton by name.
 *
 * @param p_name A pointer to a StringName with the singleton name.
 *
 * @return A pointer to the singleton Object.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionInterfaceGlobalGetSingleton)(NativeExtensionConstStringNamePtr p_name);

/**
 * @name object_get_instance_binding
 * @since 4.1
 *
 * Gets a pointer representing an Object's instance binding.
 *
 * @param p_o A pointer to the Object.
 * @param p_token A token the library received by the NativeExtension's entry point function.
 * @param p_callbacks A pointer to a NativeExtensionInstanceBindingCallbacks struct.
 *
 * @return A pointer to the instance binding.
 */
typedef void *(*NativeExtensionInterfaceObjectGetInstanceBinding)(NativeExtensionObjectPtr p_o, void *p_token, const NativeExtensionInstanceBindingCallbacks *p_callbacks);

/**
 * @name object_set_instance_binding
 * @since 4.1
 *
 * Sets an Object's instance binding.
 *
 * @param p_o A pointer to the Object.
 * @param p_token A token the library received by the NativeExtension's entry point function.
 * @param p_binding A pointer to the instance binding.
 * @param p_callbacks A pointer to a NativeExtensionInstanceBindingCallbacks struct.
 */
typedef void (*NativeExtensionInterfaceObjectSetInstanceBinding)(NativeExtensionObjectPtr p_o, void *p_token, void *p_binding, const NativeExtensionInstanceBindingCallbacks *p_callbacks);

/**
 * @name object_free_instance_binding
 * @since 4.2
 *
 * Free an Object's instance binding.
 *
 * @param p_o A pointer to the Object.
 * @param p_token A token the library received by the NativeExtension's entry point function.
 */
typedef void (*NativeExtensionInterfaceObjectFreeInstanceBinding)(NativeExtensionObjectPtr p_o, void *p_token);

/**
 * @name object_set_instance
 * @since 4.1
 *
 * Sets an extension class instance on a Object.
 *
 * `p_classname` should be a registered extension class and should extend the `p_o` Object's class.
 *
 * @param p_o A pointer to the Object.
 * @param p_classname A pointer to a StringName with the registered extension class's name.
 * @param p_instance A pointer to the extension class instance.
 */
typedef void (*NativeExtensionInterfaceObjectSetInstance)(NativeExtensionObjectPtr p_o, NativeExtensionConstStringNamePtr p_classname, NativeExtensionClassInstancePtr p_instance);

/**
 * @name object_get_class_name
 * @since 4.1
 *
 * Gets the class name of an Object.
 *
 * If the NativeExtension wraps the Gobu object in an abstraction specific to its class, this is the
 * function that should be used to determine which wrapper to use.
 *
 * @param p_object A pointer to the Object.
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param r_class_name A pointer to a String to receive the class name.
 *
 * @return true if successful in getting the class name; otherwise false.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceObjectGetClassName)(NativeExtensionConstObjectPtr p_object, NativeExtensionClassLibraryPtr p_library, NativeExtensionUninitializedStringNamePtr r_class_name);

/**
 * @name object_cast_to
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.7. Use the `is_class` method on `Object` to check if an object can be cast instead. If true, the previous pointer can be reinterpreted as a pointer to the target type.
 *
 * Casts an Object to a different type.
 *
 * @param p_object A pointer to the Object.
 * @param p_class_tag A pointer uniquely identifying a built-in class in the ClassDB.
 *
 * @return Returns a pointer to the Object, or NULL if it can't be cast to the requested type.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionInterfaceObjectCastTo)(NativeExtensionConstObjectPtr p_object, void *p_class_tag);

/**
 * @name object_get_instance_from_id
 * @since 4.1
 *
 * Gets an Object by its instance ID.
 *
 * @param p_instance_id The instance ID.
 *
 * @return A pointer to the Object.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionInterfaceObjectGetInstanceFromId)(NativeExtensionObjectInstanceID p_instance_id);

/**
 * @name object_get_instance_id
 * @since 4.1
 *
 * Gets the instance ID from an Object.
 *
 * @param p_object A pointer to the Object.
 *
 * @return The instance ID.
 */
typedef NativeExtensionObjectInstanceID (*NativeExtensionInterfaceObjectGetInstanceId)(NativeExtensionConstObjectPtr p_object);

/**
 * @name object_has_script_method
 * @since 4.3
 *
 * Checks if this object has a script with the given method.
 *
 * @param p_object A pointer to the Object.
 * @param p_method A pointer to a StringName identifying the method.
 *
 * @return true if the object has a script and that script has a method with the given name. Returns false if the object has no script.
 */
typedef NativeExtensionBool (*NativeExtensionInterfaceObjectHasScriptMethod)(NativeExtensionConstObjectPtr p_object, NativeExtensionConstStringNamePtr p_method);

/**
 * @name object_call_script_method
 * @since 4.3
 *
 * Call the given script method on this object.
 *
 * @param p_object A pointer to the Object.
 * @param p_method A pointer to a StringName identifying the method.
 * @param p_args A pointer to a C array of Variant.
 * @param p_argument_count The number of arguments.
 * @param r_return A pointer a Variant which will be assigned the return value.
 * @param r_error A pointer the structure which will hold error information.
 */
typedef void (*NativeExtensionInterfaceObjectCallScriptMethod)(NativeExtensionObjectPtr p_object, NativeExtensionConstStringNamePtr p_method, const NativeExtensionConstVariantPtr *p_args, NativeExtensionInt p_argument_count, NativeExtensionUninitializedVariantPtr r_return, NativeExtensionCallError *r_error);

/**
 * @name ref_get_object
 * @since 4.1
 *
 * Gets the Object from a reference.
 *
 * @param p_ref A pointer to the reference.
 *
 * @return A pointer to the Object from the reference or NULL.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionInterfaceRefGetObject)(NativeExtensionConstRefPtr p_ref);

/**
 * @name ref_set_object
 * @since 4.1
 *
 * Sets the Object referred to by a reference.
 *
 * @param p_ref A pointer to the reference.
 * @param p_object A pointer to the Object to refer to.
 */
typedef void (*NativeExtensionInterfaceRefSetObject)(NativeExtensionRefPtr p_ref, NativeExtensionObjectPtr p_object);

/**
 * @name script_instance_create
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.2. Use `script_instance_create3` instead.
 *
 * Creates a script instance that contains the given info and instance data.
 *
 * @param p_info A pointer to a NativeExtensionScriptInstanceInfo struct.
 * @param p_instance_data A pointer to a data representing the script instance in the NativeExtension. This will be passed to all the function pointers on p_info.
 *
 * @return A pointer to a ScriptInstanceExtension object.
 */
typedef NativeExtensionScriptInstancePtr (*NativeExtensionInterfaceScriptInstanceCreate)(const NativeExtensionScriptInstanceInfo *p_info, NativeExtensionScriptInstanceDataPtr p_instance_data);

/**
 * @name script_instance_create2
 * @since 4.2
 * @deprecated Deprecated in Gobu 4.3. Use `script_instance_create3` instead.
 *
 * Creates a script instance that contains the given info and instance data.
 *
 * @param p_info A pointer to a NativeExtensionScriptInstanceInfo2 struct.
 * @param p_instance_data A pointer to a data representing the script instance in the NativeExtension. This will be passed to all the function pointers on p_info.
 *
 * @return A pointer to a ScriptInstanceExtension object.
 */
typedef NativeExtensionScriptInstancePtr (*NativeExtensionInterfaceScriptInstanceCreate2)(const NativeExtensionScriptInstanceInfo2 *p_info, NativeExtensionScriptInstanceDataPtr p_instance_data);

/**
 * @name script_instance_create3
 * @since 4.3
 *
 * Creates a script instance that contains the given info and instance data.
 *
 * @param p_info A pointer to a NativeExtensionScriptInstanceInfo3 struct.
 * @param p_instance_data A pointer to a data representing the script instance in the NativeExtension. This will be passed to all the function pointers on p_info.
 *
 * @return A pointer to a ScriptInstanceExtension object.
 */
typedef NativeExtensionScriptInstancePtr (*NativeExtensionInterfaceScriptInstanceCreate3)(const NativeExtensionScriptInstanceInfo3 *p_info, NativeExtensionScriptInstanceDataPtr p_instance_data);

/**
 * @name placeholder_script_instance_create
 * @since 4.2
 *
 * Creates a placeholder script instance for a given script and instance.
 *
 * This interface is optional as a custom placeholder could also be created with script_instance_create().
 *
 * @param p_language A pointer to a ScriptLanguage.
 * @param p_script A pointer to a Script.
 * @param p_owner A pointer to an Object.
 *
 * @return A pointer to a PlaceHolderScriptInstance object.
 */
typedef NativeExtensionScriptInstancePtr (*NativeExtensionInterfacePlaceholderScriptInstanceCreate)(NativeExtensionObjectPtr p_language, NativeExtensionObjectPtr p_script, NativeExtensionObjectPtr p_owner);

/**
 * @name placeholder_script_instance_update
 * @since 4.2
 *
 * Updates a placeholder script instance with the given properties and values.
 *
 * The passed in placeholder must be an instance of PlaceHolderScriptInstance
 * such as the one returned by placeholder_script_instance_create().
 *
 * @param p_placeholder A pointer to a PlaceHolderScriptInstance.
 * @param p_properties A pointer to an Array of Dictionary representing PropertyInfo.
 * @param p_values A pointer to a Dictionary mapping StringName to Variant values.
 */
typedef void (*NativeExtensionInterfacePlaceholderScriptInstanceUpdate)(NativeExtensionScriptInstancePtr p_placeholder, NativeExtensionConstTypePtr p_properties, NativeExtensionConstTypePtr p_values);

/**
 * @name object_get_script_instance
 * @since 4.2
 *
 * Get the script instance data attached to this object.
 *
 * @param p_object A pointer to the Object.
 * @param p_language A pointer to the language expected for this script instance.
 *
 * @return A NativeExtensionScriptInstanceDataPtr that was attached to this object as part of script_instance_create.
 */
typedef NativeExtensionScriptInstanceDataPtr (*NativeExtensionInterfaceObjectGetScriptInstance)(NativeExtensionConstObjectPtr p_object, NativeExtensionObjectPtr p_language);

/**
 * @name object_set_script_instance
 * @since 4.5
 *
 * Set the script instance data attached to this object.
 *
 * @param p_object A pointer to the Object.
 * @param p_script_instance A pointer to the script instance data to attach to this object.
 */
typedef void (*NativeExtensionInterfaceObjectSetScriptInstance)(NativeExtensionObjectPtr p_object, NativeExtensionScriptInstanceDataPtr p_script_instance);

/**
 * @name callable_custom_create
 * @since 4.2
 * @deprecated Deprecated in Gobu 4.3. Use `callable_custom_create2` instead.
 *
 * Creates a custom Callable object from a function pointer.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param r_callable A pointer that will receive the new Callable.
 * @param p_callable_custom_info The info required to construct a Callable.
 */
typedef void (*NativeExtensionInterfaceCallableCustomCreate)(NativeExtensionUninitializedTypePtr r_callable, NativeExtensionCallableCustomInfo *p_callable_custom_info);

/**
 * @name callable_custom_create2
 * @since 4.3
 *
 * Creates a custom Callable object from a function pointer.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param r_callable A pointer that will receive the new Callable.
 * @param p_callable_custom_info The info required to construct a Callable.
 */
typedef void (*NativeExtensionInterfaceCallableCustomCreate2)(NativeExtensionUninitializedTypePtr r_callable, NativeExtensionCallableCustomInfo2 *p_callable_custom_info);

/**
 * @name callable_custom_get_userdata
 * @since 4.2
 *
 * Retrieves the userdata pointer from a custom Callable.
 *
 * If the Callable is not a custom Callable or the token does not match the one provided to callable_custom_create() via NativeExtensionCallableCustomInfo then NULL will be returned.
 *
 * @param p_callable A pointer to a Callable.
 * @param p_token A pointer to an address that uniquely identifies the NativeExtension.
 *
 * @return The userdata pointer given when creating this custom Callable.
 */
typedef void *(*NativeExtensionInterfaceCallableCustomGetUserdata)(NativeExtensionConstTypePtr p_callable, void *p_token);

/**
 * @name classdb_construct_object
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.4. Use `classdb_construct_object3` instead.
 *
 * Constructs an Object of the requested class.
 *
 * The passed class must be a built-in gobu class, or an already-registered extension class. In both cases, object_set_instance() should be called to fully initialize the object.
 *
 * @param p_classname A pointer to a StringName with the class name.
 *
 * @return A pointer to the newly created Object.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionInterfaceClassdbConstructObject)(NativeExtensionConstStringNamePtr p_classname);

/**
 * @name classdb_construct_object2
 * @since 4.4
 * @deprecated Deprecated in Gobu 4.7. Use `classdb_construct_object3` instead.
 *
 * Constructs an Object of the requested class.
 *
 * The passed class must be a built-in gobu class, or an already-registered extension class. In both cases, object_set_instance() should be called to fully initialize the object.
 *
 * "NOTIFICATION_POSTINITIALIZE" must be sent after construction.
 *
 * @param p_classname A pointer to a StringName with the class name.
 *
 * @return A pointer to the newly created Object.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionInterfaceClassdbConstructObject2)(NativeExtensionConstStringNamePtr p_classname);

/**
 * @name classdb_construct_object3
 * @since 4.7
 *
 * Constructs an Object of the requested class.
 *
 * The passed class must be a built-in gobu class, or an already-registered extension class. In both cases, object_set_instance() should be called to fully initialize the object.
 * If the type is a subtype of RefCounted, it already has a refcount of 1. The caller must take ownership the refcount and is responsible for decrementing it again when the object is no longer needed.
 *
 * "NOTIFICATION_POSTINITIALIZE" must be sent after construction.
 *
 * @param p_classname A pointer to a StringName with the class name.
 *
 * @return A pointer to the newly created Object.
 */
typedef NativeExtensionObjectPtr (*NativeExtensionInterfaceClassdbConstructObject3)(NativeExtensionConstStringNamePtr p_classname);

/**
 * @name classdb_get_method_bind
 * @since 4.1
 *
 * Gets a pointer to the MethodBind in ClassDB for the given class, method and hash.
 *
 * @param p_classname A pointer to a StringName with the class name.
 * @param p_methodname A pointer to a StringName with the method name.
 * @param p_hash A hash representing the function signature.
 *
 * @return A pointer to the MethodBind from ClassDB.
 */
typedef NativeExtensionMethodBindPtr (*NativeExtensionInterfaceClassdbGetMethodBind)(NativeExtensionConstStringNamePtr p_classname, NativeExtensionConstStringNamePtr p_methodname, NativeExtensionInt p_hash);

/**
 * @name classdb_get_class_tag
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.7. No longer needed. Use the `is_class` method on `Object` instead.
 *
 * Gets a pointer uniquely identifying the given built-in class in the ClassDB.
 *
 * @param p_classname A pointer to a StringName with the class name.
 *
 * @return A pointer uniquely identifying the built-in class in the ClassDB.
 */
typedef void *(*NativeExtensionInterfaceClassdbGetClassTag)(NativeExtensionConstStringNamePtr p_classname);

/**
 * @name classdb_register_extension_class
 * @since 4.1
 * @deprecated Deprecated in Gobu 4.2. Use `classdb_register_extension_class6` instead.
 *
 * Registers an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_parent_class_name A pointer to a StringName with the parent class name.
 * @param p_extension_funcs A pointer to a NativeExtensionClassCreationInfo struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClass)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo *p_extension_funcs);

/**
 * @name classdb_register_extension_class2
 * @since 4.2
 * @deprecated Deprecated in Gobu 4.3. Use `classdb_register_extension_class6` instead.
 *
 * Registers an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_parent_class_name A pointer to a StringName with the parent class name.
 * @param p_extension_funcs A pointer to a NativeExtensionClassCreationInfo2 struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClass2)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo2 *p_extension_funcs);

/**
 * @name classdb_register_extension_class3
 * @since 4.3
 * @deprecated Deprecated in Gobu 4.4. Use `classdb_register_extension_class6` instead.
 *
 * Registers an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_parent_class_name A pointer to a StringName with the parent class name.
 * @param p_extension_funcs A pointer to a NativeExtensionClassCreationInfo3 struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClass3)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo3 *p_extension_funcs);

/**
 * @name classdb_register_extension_class4
 * @since 4.4
 * @deprecated Deprecated in Gobu 4.5. Use `classdb_register_extension_class6` instead.
 *
 * Registers an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_parent_class_name A pointer to a StringName with the parent class name.
 * @param p_extension_funcs A pointer to a NativeExtensionClassCreationInfo4 struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClass4)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo4 *p_extension_funcs);

/**
 * @name classdb_register_extension_class5
 * @since 4.5
 * @deprecated Deprecated in Gobu 4.7. Use `classdb_register_extension_class6` instead.
 *
 * Registers an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_parent_class_name A pointer to a StringName with the parent class name.
 * @param p_extension_funcs A pointer to a NativeExtensionClassCreationInfo5 struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClass5)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo5 *p_extension_funcs);

/**
 * @name classdb_register_extension_class6
 * @since 4.7
 *
 * Registers an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_parent_class_name A pointer to a StringName with the parent class name.
 * @param p_extension_funcs A pointer to a NativeExtensionClassCreationInfo6 struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClass6)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_parent_class_name, const NativeExtensionClassCreationInfo6 *p_extension_funcs);

/**
 * @name classdb_register_extension_class_method
 * @since 4.1
 *
 * Registers a method on an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_method_info A pointer to a NativeExtensionClassMethodInfo struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassMethod)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, const NativeExtensionClassMethodInfo *p_method_info);

/**
 * @name classdb_register_extension_class_virtual_method
 * @since 4.3
 *
 * Registers a virtual method on an extension class in ClassDB, that can be implemented by scripts or other extensions.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_method_info A pointer to a NativeExtensionClassMethodInfo struct.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassVirtualMethod)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, const NativeExtensionClassVirtualMethodInfo *p_method_info);

/**
 * @name classdb_register_extension_class_integer_constant
 * @since 4.1
 *
 * Registers an integer constant on an extension class in the ClassDB.
 *
 * Note about registering bitfield values (if p_is_bitfield is true): even though p_constant_value is signed, language bindings are
 * advised to treat bitfields as uint64_t, since this is generally clearer and can prevent mistakes like using -1 for setting all bits.
 * Language APIs should thus provide an abstraction that registers bitfields (uint64_t) separately from regular constants (int64_t).
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_enum_name A pointer to a StringName with the enum name.
 * @param p_constant_name A pointer to a StringName with the constant name.
 * @param p_constant_value The constant value.
 * @param p_is_bitfield Whether or not this constant is part of a bitfield.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassIntegerConstant)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_enum_name, NativeExtensionConstStringNamePtr p_constant_name, NativeExtensionInt p_constant_value, NativeExtensionBool p_is_bitfield);

/**
 * @name classdb_register_extension_class_property
 * @since 4.1
 *
 * Registers a property on an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_info A pointer to a NativeExtensionPropertyInfo struct.
 * @param p_setter A pointer to a StringName with the name of the setter method.
 * @param p_getter A pointer to a StringName with the name of the getter method.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassProperty)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, const NativeExtensionPropertyInfo *p_info, NativeExtensionConstStringNamePtr p_setter, NativeExtensionConstStringNamePtr p_getter);

/**
 * @name classdb_register_extension_class_property_indexed
 * @since 4.2
 *
 * Registers an indexed property on an extension class in the ClassDB.
 *
 * Provided struct can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_info A pointer to a NativeExtensionPropertyInfo struct.
 * @param p_setter A pointer to a StringName with the name of the setter method.
 * @param p_getter A pointer to a StringName with the name of the getter method.
 * @param p_index The index to pass as the first argument to the getter and setter methods.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassPropertyIndexed)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, const NativeExtensionPropertyInfo *p_info, NativeExtensionConstStringNamePtr p_setter, NativeExtensionConstStringNamePtr p_getter, NativeExtensionInt p_index);

/**
 * @name classdb_register_extension_class_property_group
 * @since 4.1
 *
 * Registers a property group on an extension class in the ClassDB.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_group_name A pointer to a String with the group name.
 * @param p_prefix A pointer to a String with the prefix used by properties in this group.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassPropertyGroup)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringPtr p_group_name, NativeExtensionConstStringPtr p_prefix);

/**
 * @name classdb_register_extension_class_property_subgroup
 * @since 4.1
 *
 * Registers a property subgroup on an extension class in the ClassDB.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_subgroup_name A pointer to a String with the subgroup name.
 * @param p_prefix A pointer to a String with the prefix used by properties in this subgroup.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassPropertySubgroup)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringPtr p_subgroup_name, NativeExtensionConstStringPtr p_prefix);

/**
 * @name classdb_register_extension_class_signal
 * @since 4.1
 *
 * Registers a signal on an extension class in the ClassDB.
 *
 * Provided structs can be safely freed once the function returns.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 * @param p_signal_name A pointer to a StringName with the signal name.
 * @param p_argument_info A pointer to a NativeExtensionPropertyInfo struct.
 * @param p_argument_count The number of arguments the signal receives.
 */
typedef void (*NativeExtensionInterfaceClassdbRegisterExtensionClassSignal)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name, NativeExtensionConstStringNamePtr p_signal_name, const NativeExtensionPropertyInfo *p_argument_info, NativeExtensionInt p_argument_count);

/**
 * @name classdb_unregister_extension_class
 * @since 4.1
 *
 * Unregisters an extension class in the ClassDB.
 *
 * Unregistering a parent class before a class that inherits it will result in failure. Inheritors must be unregistered first.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_class_name A pointer to a StringName with the class name.
 */
typedef void (*NativeExtensionInterfaceClassdbUnregisterExtensionClass)(NativeExtensionClassLibraryPtr p_library, NativeExtensionConstStringNamePtr p_class_name);

/**
 * @name get_library_path
 * @since 4.1
 *
 * Gets the path to the current NativeExtension library.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param r_path A pointer to a String which will receive the path.
 */
typedef void (*NativeExtensionInterfaceGetLibraryPath)(NativeExtensionClassLibraryPtr p_library, NativeExtensionUninitializedStringPtr r_path);

/**
 * @name editor_add_plugin
 * @since 4.1
 *
 * Adds an editor plugin.
 *
 * It's safe to call during initialization.
 *
 * @param p_class_name A pointer to a StringName with the name of a class (descending from EditorPlugin) which is already registered with ClassDB.
 */
typedef void (*NativeExtensionInterfaceEditorAddPlugin)(NativeExtensionConstStringNamePtr p_class_name);

/**
 * @name editor_remove_plugin
 * @since 4.1
 *
 * Removes an editor plugin.
 *
 * @param p_class_name A pointer to a StringName with the name of a class that was previously added as an editor plugin.
 */
typedef void (*NativeExtensionInterfaceEditorRemovePlugin)(NativeExtensionConstStringNamePtr p_class_name);

/**
 * @name editor_help_load_xml_from_utf8_chars
 * @since 4.3
 *
 * Loads new XML-formatted documentation data in the editor.
 *
 * The provided pointer can be immediately freed once the function returns.
 *
 * @param p_data A pointer to a UTF-8 encoded C string (null terminated).
 */
typedef void (*NativeExtensionInterfaceEditorHelpLoadXmlFromUtf8Chars)(const char *p_data);

/**
 * @name editor_help_load_xml_from_utf8_chars_and_len
 * @since 4.3
 *
 * Loads new XML-formatted documentation data in the editor.
 *
 * The provided pointer can be immediately freed once the function returns.
 *
 * @param p_data A pointer to a UTF-8 encoded C string.
 * @param p_size The number of bytes (not code units).
 */
typedef void (*NativeExtensionInterfaceEditorHelpLoadXmlFromUtf8CharsAndLen)(const char *p_data, NativeExtensionInt p_size);

/**
 * @name editor_register_get_classes_used_callback
 * @since 4.5
 *
 * Registers a callback that Gobu can call to get the list of all classes (from ClassDB) that may be used by the calling NativeExtension.
 *
 * This is used by the editor to generate a build profile (in "Tools" > "Engine Compilation Configuration Editor..." > "Detect from project"),
 * in order to recompile Gobu with only the classes used.
 * In the provided callback, the NativeExtension should provide the list of classes that _may_ be used statically, thus the time of invocation shouldn't matter.
 * If a NativeExtension doesn't register a callback, Gobu will assume that it could be using any classes.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_callback The callback to retrieve the list of classes used.
 */
typedef void (*NativeExtensionInterfaceEditorRegisterGetClassesUsedCallback)(NativeExtensionClassLibraryPtr p_library, NativeExtensionEditorGetClassesUsedCallback p_callback);

/**
 * @name register_main_loop_callbacks
 * @since 4.5
 *
 * Registers callbacks to be called at different phases of the main loop.
 *
 * @param p_library A pointer the library received by the NativeExtension's entry point function.
 * @param p_callbacks A pointer to the structure that contains the callbacks.
 */
typedef void (*NativeExtensionInterfaceRegisterMainLoopCallbacks)(NativeExtensionClassLibraryPtr p_library, const NativeExtensionMainLoopCallbacks *p_callbacks);

#ifdef __cplusplus
}
#endif
