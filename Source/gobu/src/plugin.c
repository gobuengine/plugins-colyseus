#include "gobu_colyseus.h"
#include "colyseus_callbacks.h"
#include "tls_certificates.h"
#include <string.h>

// The engine's interface, from gobu_plugin_init()
const GobuPluginApi *gobu = NULL;
GobuPluginHandle gobu_plugin = NULL;

// =============================================================================
// Registration helpers
// =============================================================================

#define ARG(m_name, m_type) { m_name, GOBU_TYPE_##m_type, 0, NULL }
#define ANY(m_name) { m_name, GOBU_TYPE_NIL, GOBU_ARG_ANY_VARIANT, NULL }
#define NO_RETURN -1

static void add_class(const char *class_name, GobuCreateInstanceFunc create_instance, GobuFreeInstanceFunc free_instance) {
    GobuClassInfo info;
    memset(&info, 0, sizeof(info));
    info.name = class_name;
    info.parent = "Shared"; /* Gobu renamed RefCounted (2026-11-04). */
    info.create_instance = create_instance;
    info.free_instance = free_instance;
    gobu->register_class(gobu_plugin, &info);
}

// `return_type`: a GobuVariantType, or NO_RETURN. `defaults`: the values of the last arguments.
static void add_method_full(const char *class_name, const char *method_name, GobuMethodFunc function, uint32_t flags,
    int32_t return_type, const GobuArgInfo *args, int32_t arg_count, const GobuVariant *const *defaults, int32_t default_count) {
    GobuMethodInfo info;
    memset(&info, 0, sizeof(info));
    info.name = method_name;
    info.func = function;
    info.flags = flags;
    info.has_return = return_type != NO_RETURN;
    if (info.has_return) {
        info.return_info.type = return_type;
    }
    info.arg_count = arg_count;
    info.args = args;
    info.default_arg_count = default_count;
    info.default_args = defaults;
    gobu->register_method(gobu_plugin, class_name, &info);
}

static void add_method(const char *class_name, const char *method_name, GobuMethodFunc function, uint32_t flags,
    int32_t return_type, const GobuArgInfo *args, int32_t arg_count) {
    add_method_full(class_name, method_name, function, flags, return_type, args, arg_count, NULL, 0);
}

#define ARG_COUNT(m_args) ((int32_t)(sizeof(m_args) / sizeof(m_args[0])))

// =============================================================================
// Classes
// =============================================================================

static void register_colyseus_client(void) {
    const char *cls = "_ColyseusClient";
    add_class(cls, gobuext_colyseus_client_create_instance, gobuext_colyseus_client_free_instance);

    const GobuArgInfo endpoint[] = { ARG("endpoint", STRING) };
    add_method(cls, "set_endpoint", gobuext_colyseus_client_set_endpoint, 0, NO_RETURN, endpoint, 1);
    add_method(cls, "get_endpoint", gobuext_colyseus_client_get_endpoint, 0, GOBU_TYPE_STRING, NULL, 0);

    // Matchmaking: (room name or id, options as JSON = "{}") -> _ColyseusRoom
    GobuVariant no_options;
    gobu->variant_new_string(&no_options, "{}", -1);
    const GobuVariant *options_default[] = { &no_options };
    const GobuArgInfo room_name[] = { ARG("room_name", STRING), ARG("options_json", STRING) };
    const GobuArgInfo room_id[] = { ARG("room_id", STRING), ARG("options_json", STRING) };
    add_method_full(cls, "join_or_create", gobuext_colyseus_client_join_or_create, 0, GOBU_TYPE_OBJECT, room_name, 2, options_default, 1);
    add_method_full(cls, "create", gobuext_colyseus_client_create, 0, GOBU_TYPE_OBJECT, room_name, 2, options_default, 1);
    add_method_full(cls, "join", gobuext_colyseus_client_join, 0, GOBU_TYPE_OBJECT, room_name, 2, options_default, 1);
    add_method_full(cls, "join_by_id", gobuext_colyseus_client_join_by_id, 0, GOBU_TYPE_OBJECT, room_id, 2, options_default, 1);
    gobu->variant_destroy(&no_options); // The engine keeps its own copy.
    const GobuArgInfo token[] = { ARG("reconnection_token", STRING) };
    add_method(cls, "reconnect", gobuext_colyseus_client_reconnect, 0, GOBU_TYPE_OBJECT, token, 1);

    // HTTP and auth requests answer with these: (request id, status code or error code, body or message)
    const GobuArgInfo response[] = { ARG("request_id", INT), ARG("status_code", INT), ARG("body", STRING) };
    const GobuArgInfo error[] = { ARG("request_id", INT), ARG("code", INT), ARG("message", STRING) };
    gobu->register_signal(gobu_plugin, cls, "_http_response", response, 3);
    gobu->register_signal(gobu_plugin, cls, "_http_error", error, 3);

    const GobuArgInfo path[] = { ARG("path", STRING) };
    const GobuArgInfo path_body[] = { ARG("path", STRING), ARG("body", STRING) };
    add_method(cls, "http_get", gobuext_colyseus_client_http_get, 0, GOBU_TYPE_INT, path, 1);
    add_method(cls, "http_post", gobuext_colyseus_client_http_post, 0, GOBU_TYPE_INT, path_body, 2);
    add_method(cls, "http_put", gobuext_colyseus_client_http_put, 0, GOBU_TYPE_INT, path_body, 2);
    add_method(cls, "http_delete", gobuext_colyseus_client_http_delete, 0, GOBU_TYPE_INT, path, 1);
    add_method(cls, "http_patch", gobuext_colyseus_client_http_patch, 0, GOBU_TYPE_INT, path_body, 2);

    // auth_request(op, email, password, options_json): an auth operation of colyseus.gro (_AUTH_*)
    const GobuArgInfo auth[] = { ARG("op", INT), ARG("email", STRING), ARG("password", STRING), ARG("options_json", STRING) };
    add_method(cls, "auth_request", gobuext_colyseus_client_auth_request, 0, GOBU_TYPE_INT, auth, ARG_COUNT(auth));
    const GobuArgInfo auth_token[] = { ARG("token", STRING) };
    add_method(cls, "auth_set_token", gobuext_colyseus_client_auth_set_token, 0, NO_RETURN, auth_token, 1);
    add_method(cls, "auth_get_token", gobuext_colyseus_client_auth_get_token, 0, GOBU_TYPE_STRING, NULL, 0);
    add_method(cls, "auth_signout", gobuext_colyseus_client_auth_signout, 0, NO_RETURN, NULL, 0);

    add_method(cls, "poll", gobuext_colyseus_client_poll, GOBU_METHOD_STATIC, NO_RETURN, NULL, 0);
}

static void register_colyseus_room(void) {
    const char *cls = "_ColyseusRoom";
    add_class(cls, gobuext_colyseus_room_create_instance, gobuext_colyseus_room_free_instance);

    const GobuArgInfo message[] = { ANY("type"), ANY("data") };
    const GobuArgInfo code_message[] = { ARG("code", INT), ARG("message", STRING) };
    const GobuArgInfo code_reason[] = { ARG("code", INT), ARG("reason", STRING) };
    gobu->register_signal(gobu_plugin, cls, "joined", NULL, 0);
    gobu->register_signal(gobu_plugin, cls, "state_changed", NULL, 0);
    gobu->register_signal(gobu_plugin, cls, "reconnected", NULL, 0);
    gobu->register_signal(gobu_plugin, cls, "message_received", message, 2);
    gobu->register_signal(gobu_plugin, cls, "error", code_message, 2);
    gobu->register_signal(gobu_plugin, cls, "left", code_reason, 2);
    gobu->register_signal(gobu_plugin, cls, "dropped", code_reason, 2);

    // The data of a message: any value (it goes as msgpack)
    const GobuArgInfo send[] = { ARG("type", STRING), ANY("data") };
    const GobuArgInfo send_int[] = { ARG("type", INT), ANY("data") };
    add_method(cls, "send_message", gobuext_colyseus_room_send_message, 0, NO_RETURN, send, 2);
    add_method(cls, "send_message_int", gobuext_colyseus_room_send_message_int, 0, NO_RETURN, send_int, 2);
    add_method(cls, "leave", gobuext_colyseus_room_leave, 0, NO_RETURN, NULL, 0);

    add_method(cls, "get_id", gobuext_colyseus_room_get_id, 0, GOBU_TYPE_STRING, NULL, 0);
    add_method(cls, "get_session_id", gobuext_colyseus_room_get_session_id, 0, GOBU_TYPE_STRING, NULL, 0);
    add_method(cls, "get_reconnection_token", gobuext_colyseus_room_get_reconnection_token, 0, GOBU_TYPE_STRING, NULL, 0);
    add_method(cls, "get_name", gobuext_colyseus_room_get_name, 0, GOBU_TYPE_STRING, NULL, 0);
    add_method(cls, "is_connected", gobuext_colyseus_room_is_connected, 0, GOBU_TYPE_BOOL, NULL, 0);
    add_method(cls, "is_reconnecting", gobuext_colyseus_room_is_reconnecting, 0, GOBU_TYPE_BOOL, NULL, 0);

    const GobuArgInfo options[] = { ARG("options", DICTIONARY) };
    add_method(cls, "set_reconnection_options", gobuext_colyseus_room_set_reconnection_options, 0, NO_RETURN, options, 1);
    add_method(cls, "get_state", gobuext_colyseus_room_get_state, 0, GOBU_TYPE_DICTIONARY, NULL, 0);
    // A registered name (String) or a schema definition of Colyseus.schema() (Dictionary or Array)
    const GobuArgInfo state_type[] = { ANY("state_type") };
    add_method(cls, "set_state_type", gobuext_colyseus_room_set_state_type, 0, NO_RETURN, state_type, 1);
}

static void register_colyseus_callbacks(void) {
    const char *cls = "_ColyseusCallbacks";
    add_class(cls, gobuext_colyseus_callbacks_create_instance, gobuext_colyseus_callbacks_free_instance);

    // get(room) -> _ColyseusCallbacks
    const GobuArgInfo room[] = { ARG("room", OBJECT) };
    add_method(cls, "get", gobuext_colyseus_callbacks_get, GOBU_METHOD_STATIC, GOBU_TYPE_OBJECT, room, 1);

    // Overloaded by their arguments' types (see colyseus_callbacks.h): they take any, and say which are wrong
    add_method(cls, "listen", gobuext_colyseus_callbacks_listen, GOBU_METHOD_VARARG, GOBU_TYPE_INT, NULL, 0);
    add_method(cls, "on_add", gobuext_colyseus_callbacks_on_add, GOBU_METHOD_VARARG, GOBU_TYPE_INT, NULL, 0);
    add_method(cls, "on_remove", gobuext_colyseus_callbacks_on_remove, GOBU_METHOD_VARARG, GOBU_TYPE_INT, NULL, 0);
    add_method(cls, "on_change", gobuext_colyseus_callbacks_on_change, GOBU_METHOD_VARARG, GOBU_TYPE_INT, NULL, 0);

    const GobuArgInfo handle[] = { ARG("handle", INT) };
    add_method(cls, "remove", gobuext_colyseus_callbacks_remove, 0, NO_RETURN, handle, 1);
}

// =============================================================================
// Entry point
// =============================================================================

bool colyseus_register_online_provider(const GobuPluginApi *p_api, GobuPluginHandle p_plugin);

static void colyseus_plugin_deinitialize(void *userdata) {
    (void)userdata;
    gobuext_tls_certificates_cleanup();
}

// Gobu's plugin entry point (gobu_plugin.h).
GOBU_PLUGIN_EXPORT bool gobu_plugin_init(const GobuPluginApi *p_api, GobuPluginHandle p_plugin, GobuPluginInfo *r_info) {
    // Version 1 is enough; the online provider needs version 2 (colyseus_provider.c checks it).
    if (p_api->version < 1 || p_api->size < offsetof(GobuPluginApi, register_online_provider)) {
        return false;
    }
    gobu = p_api;
    gobu_plugin = p_plugin;

    // TLS certificates: an override from Project Settings, if any
    gobuext_tls_certificates_init();

    register_colyseus_client();
    register_colyseus_room();
    register_colyseus_callbacks();
    if (!colyseus_register_online_provider(p_api, p_plugin)) {
        return false;
    }

    r_info->userdata = NULL;
    r_info->deinitialize = colyseus_plugin_deinitialize;
    return true;
}
