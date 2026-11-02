/*
 * Engine values for the binding (gobu_colyseus.h): the few operations it repeats on top of gobu_plugin.h.
 */

#include "gobu_colyseus.h"

#include <stdlib.h>

void gobuext_new_string(GobuVariant* r_dest, const char* p_text) {
    gobu->variant_new_string(r_dest, p_text ? p_text : "", -1);
}

char* gobuext_to_utf8(const GobuVariant* p_value) {
    const int64_t length = gobu->variant_to_utf8(p_value, NULL, 0);
    char* text = (char*)malloc((size_t)length + 1);
    if (!text) {
        return NULL;
    }
    gobu->variant_to_utf8(p_value, text, length + 1);
    return text;
}

const GobuVariant* gobuext_dictionary_find(const GobuVariant* p_dictionary, const char* p_key) {
    GobuVariant key;
    gobu->variant_new_string(&key, p_key, -1);
    const GobuVariant* value = gobu->dictionary_find(p_dictionary, &key);
    gobu->variant_destroy(&key);
    return value;
}

void gobuext_dictionary_set(GobuVariant* p_dictionary, const char* p_key, GobuVariant* p_value) {
    GobuVariant key;
    gobu->variant_new_string(&key, p_key, -1);
    gobu->variant_set(p_dictionary, &key, p_value);
    gobu->variant_destroy(&key);
    gobu->variant_destroy(p_value);
}

void gobuext_array_append(GobuVariant* p_array, GobuVariant* p_value) {
    gobu->array_push_back(p_array, p_value);
    gobu->variant_destroy(p_value);
}

bool gobuext_call(GobuVariant* p_self, const char* p_method, const GobuVariant* const* p_args, int64_t p_arg_count) {
    GobuVariant ignored;
    GobuCallError error = { 0, 0, 0 };
    gobu->variant_call(p_self, p_method, p_args, p_arg_count, &ignored, &error);
    gobu->variant_destroy(&ignored);
    return error.error == GOBU_CALL_OK;
}
