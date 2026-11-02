#ifndef GOBU_COLYSEUS_H
#define GOBU_COLYSEUS_H

/*
 * The binding of the Colyseus SDK as a Gobu plugin, on Gobu's plugin interface (gobu_plugin.h). Every engine
 * value it holds is a GobuVariant, and every method of its classes takes Variant arguments (GOBUEXT_METHOD):
 * the engine checks their count and converts them to the declared types first. `gobu` is the interface the
 * engine gives gobu_plugin_init() (plugin.c).
 */

#include <gobu_plugin.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <colyseus/client.h>

extern const GobuPluginApi *gobu;
extern GobuPluginHandle gobu_plugin;

// A method of a class of the plugin (a GobuMethodFunc).
#define GOBUEXT_METHOD(m_name) \
    void m_name(void* p_method_userdata, void* p_instance, const GobuVariant* const* p_args, \
        int64_t p_argument_count, GobuVariant* r_return, GobuCallError* r_error)

// Values (gobu_values.c).

// A String of `p_text`; NULL gives an empty one.
void gobuext_new_string(GobuVariant* r_dest, const char* p_text);
// The UTF-8 text of a String (of anything else, its text form), malloc'd: the caller frees it. NULL only when
// out of memory.
char* gobuext_to_utf8(const GobuVariant* p_value);
// The value of a String key in a Dictionary, or NULL if it has none.
const GobuVariant* gobuext_dictionary_find(const GobuVariant* p_dictionary, const char* p_key);
// dictionary[key] = value, with a String key. Takes `p_value`: it is destroyed.
void gobuext_dictionary_set(GobuVariant* p_dictionary, const char* p_key, GobuVariant* p_value);
// array.push_back(value). Takes `p_value`: it is destroyed.
void gobuext_array_append(GobuVariant* p_array, GobuVariant* p_value);
// Calls `p_method` of a value and drops what it returns; true when the call succeeded.
bool gobuext_call(GobuVariant* p_self, const char* p_method, const GobuVariant* const* p_args, int64_t p_arg_count);

// The data of a _ColyseusClient
typedef struct ColyseusClientWrapper {
    colyseus_client_t* native_client;
    GobuObjectId object_id;              // The engine object: its events resolve it on the main thread
    volatile int pending_http_requests;  // Track in-flight HTTP requests
} ColyseusClientWrapper;

// A schema definition of Colyseus.schema() (colyseus_schema_definition.h)
typedef struct colyseus_schema_definition colyseus_schema_definition_t;

// The data of a _ColyseusRoom
typedef struct {
    colyseus_room_t* native_room;
    GobuObjectId object_id;              // The engine object: its events resolve it on the main thread
    const colyseus_schema_vtable_t* pending_vtable;  // Vtable to set when native room is assigned
    colyseus_schema_definition_t* schema_definition;  // The one of set_state_type(), if any
} ColyseusRoomWrapper;

// _ColyseusClient
void* gobuext_colyseus_client_create_instance(void* p_class_userdata, GobuObject* p_object);
void gobuext_colyseus_client_free_instance(void* p_class_userdata, void* p_instance);
GOBUEXT_METHOD(gobuext_colyseus_client_set_endpoint);
GOBUEXT_METHOD(gobuext_colyseus_client_get_endpoint);
// Matchmaking: (room_name or room_id, options_json) -> _ColyseusRoom; reconnect(reconnection_token)
GOBUEXT_METHOD(gobuext_colyseus_client_join_or_create);
GOBUEXT_METHOD(gobuext_colyseus_client_create);
GOBUEXT_METHOD(gobuext_colyseus_client_join);
GOBUEXT_METHOD(gobuext_colyseus_client_join_by_id);
GOBUEXT_METHOD(gobuext_colyseus_client_reconnect);
// HTTP: (path[, body]) -> request id, answered by the _http_response / _http_error signals
GOBUEXT_METHOD(gobuext_colyseus_client_http_get);
GOBUEXT_METHOD(gobuext_colyseus_client_http_post);
GOBUEXT_METHOD(gobuext_colyseus_client_http_put);
GOBUEXT_METHOD(gobuext_colyseus_client_http_delete);
GOBUEXT_METHOD(gobuext_colyseus_client_http_patch);
// Auth
GOBUEXT_METHOD(gobuext_colyseus_client_auth_set_token);
GOBUEXT_METHOD(gobuext_colyseus_client_auth_get_token);
GOBUEXT_METHOD(gobuext_colyseus_client_auth_request);
GOBUEXT_METHOD(gobuext_colyseus_client_auth_signout);
// Static poll(): runs the network and hands the queued events to the engine
GOBUEXT_METHOD(gobuext_colyseus_client_poll);

// _ColyseusRoom
void* gobuext_colyseus_room_create_instance(void* p_class_userdata, GobuObject* p_object);
void gobuext_colyseus_room_free_instance(void* p_class_userdata, void* p_instance);
// The data of a _ColyseusRoom, or NULL when `p_object` is not one.
ColyseusRoomWrapper* gobuext_colyseus_room_get_wrapper(GobuObject* p_object);
GOBUEXT_METHOD(gobuext_colyseus_room_send_message);
GOBUEXT_METHOD(gobuext_colyseus_room_send_message_int);
GOBUEXT_METHOD(gobuext_colyseus_room_leave);
GOBUEXT_METHOD(gobuext_colyseus_room_get_id);
GOBUEXT_METHOD(gobuext_colyseus_room_get_session_id);
GOBUEXT_METHOD(gobuext_colyseus_room_get_reconnection_token);
GOBUEXT_METHOD(gobuext_colyseus_room_get_name);
GOBUEXT_METHOD(gobuext_colyseus_room_is_connected);
GOBUEXT_METHOD(gobuext_colyseus_room_is_reconnecting);
GOBUEXT_METHOD(gobuext_colyseus_room_set_reconnection_options);
// get_state() -> the room state, as a Dictionary
GOBUEXT_METHOD(gobuext_colyseus_room_get_state);
// set_state_type(type) - sets the schema vtable for state decoding
// Accepts either:
//   - String: a vtable registered by name
//   - Dictionary or Array: a schema definition of Colyseus.schema() (colyseus.gro)
GOBUEXT_METHOD(gobuext_colyseus_room_set_state_type);

#endif // GOBU_COLYSEUS_H
