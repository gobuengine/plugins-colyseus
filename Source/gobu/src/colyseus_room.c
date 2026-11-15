#include "gobu_colyseus.h"
#include "colyseus_state.h"
#include "colyseus_schema_registry.h"
#include "colyseus_schema_definition.h"
#include "msgpack_encoder.h"
#include <colyseus/room.h>
#include <colyseus/schema.h>
#include <colyseus/schema/dynamic_schema.h>
#include <stdlib.h>
#include <string.h>

// A request() waiting for its reply. The SDK's reply callback does not say which request it answers: this does.
struct gobuext_room_request {
    ColyseusRoomWrapper* wrapper;
    uint32_t id;
    struct gobuext_room_request* next;
};

// Takes the request of `p_id` out of the room's list; NULL if it has none.
static gobuext_room_request_t* room_request_take(ColyseusRoomWrapper* p_wrapper, uint32_t p_id) {
    for (gobuext_room_request_t** link = &p_wrapper->requests; *link; link = &(*link)->next) {
        if ((*link)->id == p_id) {
            gobuext_room_request_t* request = *link;
            *link = request->next;
            return request;
        }
    }
    return NULL;
}

void* gobuext_colyseus_room_create_instance(void* p_class_userdata, GobuObject* p_object) {
    (void)p_class_userdata;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)calloc(1, sizeof(ColyseusRoomWrapper));
    if (!wrapper) return NULL;
    wrapper->object_id = gobu->object_get_instance_id(p_object);
    return wrapper;
}

void gobuext_colyseus_room_free_instance(void* p_class_userdata, void* p_instance) {
    (void)p_class_userdata;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    if (wrapper) {
        if (wrapper->native_room) {
            colyseus_room_free(wrapper->native_room);  // Its pending requests go without a reply.
        }
        while (wrapper->requests) {
            gobuext_room_request_t* next = wrapper->requests->next;
            free(wrapper->requests);
            wrapper->requests = next;
        }

        // The schema set with set_state_type(), if any
        colyseus_schema_definition_free(wrapper->schema_definition);

        free(wrapper);
    }
}

ColyseusRoomWrapper* gobuext_colyseus_room_get_wrapper(GobuObject* p_object) {
    if (!p_object || !gobu->object_is_class(p_object, "_ColyseusRoom")) {
        return NULL;
    }
    return (ColyseusRoomWrapper*)gobu->object_get_plugin_instance(gobu_plugin, p_object);
}

// send_message(type: String, data): `data` (any value) goes as msgpack
GOBUEXT_METHOD(gobuext_colyseus_room_send_message) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    if (!wrapper || !wrapper->native_room) {
        return;
    }

    char* type = gobuext_to_utf8(p_args[0]);
    if (!type || type[0] == '\0') {
        free(type);
        return;
    }

    size_t msgpack_len = 0;
    uint8_t* msgpack_data = gobu_variant_to_msgpack(p_args[1], &msgpack_len);
    colyseus_room_send_encoded(wrapper->native_room, type, msgpack_data, msgpack_len);

    free(type);
    free(msgpack_data);
}

// send_message_int(type: int, data)
GOBUEXT_METHOD(gobuext_colyseus_room_send_message_int) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    if (!wrapper || !wrapper->native_room) {
        return;
    }

    const int64_t type = gobu->variant_to_int(p_args[0]);
    size_t msgpack_len = 0;
    uint8_t* msgpack_data = gobu_variant_to_msgpack(p_args[1], &msgpack_len);
    colyseus_room_send_int_encoded(wrapper->native_room, (int)type, msgpack_data, msgpack_len);
    free(msgpack_data);
}

static void on_request_reply(colyseus_request_outcome_t p_outcome, const uint8_t* p_data, size_t p_length,
    const char* p_reason, void* p_userdata) {
    gobuext_room_request_t* request = (gobuext_room_request_t*)p_userdata;
    ColyseusRoomWrapper* wrapper = request->wrapper;
    gobuext_room_queue_reply(wrapper->object_id, request->id, (int)p_outcome, p_data, p_length, p_reason);
    free(room_request_take(wrapper, request->id));
}

// request(type: String, data) -> int: the request's id (0 when it was not sent), answered by _request_reply. `data`
// goes as msgpack, as send_message()'s; the reply is what the server's onMessage handler returns.
GOBUEXT_METHOD(gobuext_colyseus_room_request) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    gobu->variant_new_int(r_return, 0);
    char* type = wrapper && wrapper->native_room ? gobuext_to_utf8(p_args[0]) : NULL;
    gobuext_room_request_t* request = type && type[0] ? (gobuext_room_request_t*)calloc(1, sizeof(gobuext_room_request_t)) : NULL;
    if (!request) {
        free(type);
        return;
    }
    // Its id is the room's next one; it is listed first, in case the reply came during the call.
    const uint32_t next_id = wrapper->native_room->next_request_id + 1;
    request->wrapper = wrapper;
    request->id = next_id;
    request->next = wrapper->requests;
    wrapper->requests = request;

    size_t length = 0;
    uint8_t* data = gobu_variant_to_msgpack(p_args[1], &length);
    const uint32_t id = colyseus_room_request_encoded_reply(wrapper->native_room, type, data, length, on_request_reply, request);
    if (id == 0) {
        free(room_request_take(wrapper, next_id));  // Not sent: no reply will come.
    }
    gobu->variant_destroy(r_return);
    gobu->variant_new_int(r_return, id);
    free(type);
    free(data);
}

// cancel_request(request_id: int): its reply, if it comes, is dropped.
GOBUEXT_METHOD(gobuext_colyseus_room_cancel_request) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    const uint32_t id = (uint32_t)gobu->variant_to_int(p_args[0]);
    gobuext_room_request_t* request = wrapper ? room_request_take(wrapper, id) : NULL;
    if (request) {
        colyseus_room_cancel_request(wrapper->native_room, id);
        free(request);
    }
}

static void on_pong(int p_rtt_ms, void* p_userdata) {
    gobuext_room_queue_pong(((ColyseusRoomWrapper*)p_userdata)->object_id, p_rtt_ms);
}

// ping() -> bool: whether a ping went (the room is connected), answered by pong(rtt_ms). A new ping replaces one
// that is still waiting.
GOBUEXT_METHOD(gobuext_colyseus_room_ping) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    const bool connected = wrapper && wrapper->native_room && colyseus_room_is_connected(wrapper->native_room);
    if (connected) {
        colyseus_room_ping(wrapper->native_room, on_pong, wrapper);
    }
    gobu->variant_new_bool(r_return, connected);
}

GOBUEXT_METHOD(gobuext_colyseus_room_leave) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_return; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    if (wrapper && wrapper->native_room) {
        colyseus_room_leave(wrapper->native_room, false);
    }
}

// The getters of the room's text: "" until it is joined.
static void return_room_text(void* p_instance, const char* (*p_get)(const colyseus_room_t*), GobuVariant* r_return) {
    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    gobuext_new_string(r_return, wrapper && wrapper->native_room ? p_get(wrapper->native_room) : NULL);
}

GOBUEXT_METHOD(gobuext_colyseus_room_get_id) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;
    return_room_text(p_instance, colyseus_room_get_id, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_room_get_session_id) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;
    return_room_text(p_instance, colyseus_room_get_session_id, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_room_get_reconnection_token) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;
    return_room_text(p_instance, colyseus_room_get_reconnection_token, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_room_get_name) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;
    return_room_text(p_instance, colyseus_room_get_name, r_return);
}

GOBUEXT_METHOD(gobuext_colyseus_room_is_connected) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    gobu->variant_new_bool(r_return, wrapper && wrapper->native_room && colyseus_room_is_connected(wrapper->native_room));
}

GOBUEXT_METHOD(gobuext_colyseus_room_is_reconnecting) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    gobu->variant_new_bool(r_return, wrapper && wrapper->native_room && colyseus_room_is_reconnecting(wrapper->native_room));
}

/*
 * set_reconnection_options(options: Dictionary)
 *
 * Accepts a Dictionary with any subset of the following keys (omitted keys
 * keep their current value):
 *   enabled: bool
 *   max_retries: int
 *   min_delay_ms: int
 *   max_delay_ms: int
 *   min_uptime_ms: int
 *   delay_ms: int
 *   max_enqueued_messages: int
 */
GOBUEXT_METHOD(gobuext_colyseus_room_set_reconnection_options) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    if (!wrapper || !wrapper->native_room) {
        return;
    }

    colyseus_reconnection_options_t opts;
    colyseus_room_get_reconnection_options(wrapper->native_room, &opts);

    const GobuVariant* options = p_args[0];
    const GobuVariant* value = NULL;
    #define READ_INT_FIELD(KEY, FIELD) \
        if ((value = gobuext_dictionary_find(options, KEY)) && gobu->variant_get_type(value) != GOBU_TYPE_NIL) { \
            FIELD = (int)gobu->variant_to_int(value); \
        }

    if ((value = gobuext_dictionary_find(options, "enabled")) && gobu->variant_get_type(value) != GOBU_TYPE_NIL) {
        opts.enabled = gobu->variant_to_bool(value);
    }
    READ_INT_FIELD("max_retries", opts.max_retries);
    READ_INT_FIELD("min_delay_ms", opts.min_delay_ms);
    READ_INT_FIELD("max_delay_ms", opts.max_delay_ms);
    READ_INT_FIELD("min_uptime_ms", opts.min_uptime_ms);
    READ_INT_FIELD("delay_ms", opts.delay_ms);
    READ_INT_FIELD("max_enqueued_messages", opts.max_enqueued_messages);

    #undef READ_INT_FIELD

    colyseus_room_set_reconnection_options(wrapper->native_room, &opts);
}

/*
 * get_state() - Returns the room state, as a Dictionary (empty before the first one).
 */
GOBUEXT_METHOD(gobuext_colyseus_room_get_state) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    gobu->dictionary_new(r_return);
    if (!wrapper || !wrapper->native_room) {
        return;
    }

    void* state = colyseus_room_get_state(wrapper->native_room);
    const colyseus_schema_vtable_t* vtable = wrapper->native_room->state_vtable;
    if (state && vtable) {
        if (colyseus_vtable_is_dynamic(vtable)) {
            colyseus_dynamic_schema_to_dictionary((colyseus_dynamic_schema_t*)state, r_return);
        } else {
            colyseus_schema_to_dictionary((colyseus_schema_t*)state, vtable, r_return);
        }
    }
}

/*
 * set_state_type(state_type) accepts either:
 *   - String: a vtable registered by name (colyseus_schema_register())
 *   - Dictionary or Array: a schema definition of Colyseus.schema() (colyseus.gro)
 */
GOBUEXT_METHOD(gobuext_colyseus_room_set_state_type) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return;

    ColyseusRoomWrapper* wrapper = (ColyseusRoomWrapper*)p_instance;
    if (!wrapper) {
        return;
    }

    const int32_t arg_type = gobu->variant_get_type(p_args[0]);
    const colyseus_schema_vtable_t* vtable = NULL;

    if (arg_type == GOBU_TYPE_STRING) {
        // A vtable registered by name
        char* name = gobuext_to_utf8(p_args[0]);
        if (name && name[0] != '\0') {
            vtable = colyseus_schema_lookup(name);
        }
        free(name);
    } else if (arg_type == GOBU_TYPE_DICTIONARY || arg_type == GOBU_TYPE_ARRAY) {
        // A schema definition (Colyseus.schema()): a dynamic vtable
        colyseus_schema_definition_t* definition = colyseus_schema_definition_create(p_args[0]);
        if (!definition) {
            r_error->error = GOBU_CALL_ERROR_INVALID_ARGUMENT;
            r_error->argument = 0;
            r_error->expected = GOBU_TYPE_DICTIONARY;
            return;
        }
        // The room may still decode with the previous one: it goes with the room.
        colyseus_schema_definition_free(wrapper->schema_definition);
        wrapper->schema_definition = definition;
        vtable = (const colyseus_schema_vtable_t*)definition->vtable;
    }

    if (!vtable) {
        return;
    }
    if (wrapper->native_room) {
        colyseus_room_set_state_type(wrapper->native_room, vtable);
    } else {
        wrapper->pending_vtable = vtable;
    }
}
