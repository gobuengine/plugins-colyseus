#include "gobu_colyseus.h"
#include "msgpack_variant.h"
#include "tls_certificates.h"
#include <colyseus/auth/auth.h>
#include <colyseus/client.h>
#include <colyseus/http.h>
#include <colyseus/room.h>
#include <colyseus/settings.h>
#include <colyseus/schema/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#ifndef __EMSCRIPTEN__
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#endif

// HTTP event queue — thread-safe, processed in poll()
typedef struct gobuext_http_event {
    int is_error;
    int64_t request_id;
    int status_or_code;
    char* body_or_message;
    GobuObjectId client_id;  // The _ColyseusClient: resolved on the main thread, it may be gone by then
    struct gobuext_http_event* next;
} gobuext_http_event_t;

// Room event queue — thread-safe, processed in poll()
typedef enum {
    ROOM_EVENT_JOIN = 0,
    ROOM_EVENT_STATE_CHANGE,
    ROOM_EVENT_ERROR,
    ROOM_EVENT_LEAVE,
    ROOM_EVENT_MESSAGE,
    ROOM_EVENT_DROP,
    ROOM_EVENT_RECONNECT,
} gobuext_room_event_type_t;

typedef struct gobuext_room_event {
    gobuext_room_event_type_t type;
    GobuObjectId room_id;   // The _ColyseusRoom: resolved on the main thread, it may be gone by then
    int code;               // for error/leave
    char* str_data;         // message (error/leave reason), or message type
    uint8_t* binary_data;   // message payload (msgpack)
    size_t binary_len;
    struct gobuext_room_event* next;
} gobuext_room_event_t;

static void room_event_push(gobuext_room_event_t* event);
static gobuext_room_event_t* room_event_pop(void);

// Helper to parse endpoint string (e.g., "ws://localhost:2567")
static void parse_endpoint(const char* endpoint, char** address, char** port, bool* use_secure) {
    if (!endpoint) return;
    
    // Check protocol
    if (strncmp(endpoint, "wss://", 6) == 0) {
        *use_secure = true;
        endpoint += 6;
    } else if (strncmp(endpoint, "ws://", 5) == 0) {
        *use_secure = false;
        endpoint += 5;
    } else if (strncmp(endpoint, "https://", 8) == 0) {
        *use_secure = true;
        endpoint += 8;
    } else if (strncmp(endpoint, "http://", 7) == 0) {
        *use_secure = false;
        endpoint += 7;
    }
    
    // Find port separator
    const char* colon = strchr(endpoint, ':');
    const char* slash = strchr(endpoint, '/');
    
    if (colon && (!slash || colon < slash)) {
        // Has port
        size_t addr_len = colon - endpoint;
        *address = (char*)malloc(addr_len + 1);
        strncpy(*address, endpoint, addr_len);
        (*address)[addr_len] = '\0';
        
        // Extract port
        const char* port_start = colon + 1;
        const char* port_end = slash ? slash : (port_start + strlen(port_start));
        size_t port_len = port_end - port_start;
        *port = (char*)malloc(port_len + 1);
        strncpy(*port, port_start, port_len);
        (*port)[port_len] = '\0';
    } else {
        // No port, just address
        const char* addr_end = slash ? slash : (endpoint + strlen(endpoint));
        size_t addr_len = addr_end - endpoint;
        *address = (char*)malloc(addr_len + 1);
        strncpy(*address, endpoint, addr_len);
        (*address)[addr_len] = '\0';
        
        *port = strdup(*use_secure ? "443" : "80");
    }
}

// Forward declaration for callbacks
static void on_matchmaking_success(colyseus_room_t* room, void* userdata);
static void on_matchmaking_error(int code, const char* message, void* userdata);

// Room event callbacks
static void on_room_join(void* userdata);
static void on_room_state_change(void* userdata);
static void on_room_error(int code, const char* message, void* userdata);
static void on_room_leave(int code, const char* reason, void* userdata);

void* gobuext_colyseus_client_create_instance(void* p_class_userdata, GobuObject* p_object) {
    (void)p_class_userdata;

    ColyseusClientWrapper* wrapper = (ColyseusClientWrapper*)calloc(1, sizeof(ColyseusClientWrapper));
    if (!wrapper) return NULL;
    wrapper->object_id = gobu->object_get_instance_id(p_object);
    return wrapper;
}

void gobuext_colyseus_client_free_instance(void* p_class_userdata, void* p_instance) {
    (void)p_class_userdata;

    ColyseusClientWrapper* wrapper = (ColyseusClientWrapper*)p_instance;
    if (wrapper) {
        // Wait for in-flight HTTP requests to complete (their threads use the native client). What they
        // queued finds the client gone, and is dropped.
        int wait_ms = 0;
        while (wrapper->pending_http_requests > 0 && wait_ms < 2000) {
            #ifdef _WIN32
            Sleep(10);
            #else
            usleep(10000);
            #endif
            wait_ms += 10;
        }

        if (wrapper->native_client) {
            colyseus_client_free(wrapper->native_client);
        }
        free(wrapper);
    }
}

// set_endpoint(endpoint: String): "ws://localhost:2567", "wss://example.com"...
GOBUEXT_METHOD(gobuext_colyseus_client_set_endpoint) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return; (void)r_error;

    ColyseusClientWrapper* wrapper = (ColyseusClientWrapper*)p_instance;
    if (!wrapper) return;

    char* endpoint_str = gobuext_to_utf8(p_args[0]);
    if (!endpoint_str) return;

    // Parse endpoint and create settings
    char* address = NULL;
    char* port = NULL;
    bool use_secure = false;
    parse_endpoint(endpoint_str, &address, &port, &use_secure);

    // Create settings
    colyseus_settings_t* settings = colyseus_settings_create();
    colyseus_settings_set_address(settings, address);
    colyseus_settings_set_port(settings, port);
    colyseus_settings_set_secure(settings, use_secure);

    // Set CA certificates for TLS verification (auto-loaded at init time)
    const unsigned char* ca_data = gobuext_tls_get_ca_certificates();
    size_t ca_len = gobuext_tls_get_ca_certificates_len();
    if (ca_data && ca_len > 0) {
        colyseus_settings_set_ca_certificates(settings, ca_data, ca_len);
    }

    // Create native client
    if (wrapper->native_client) {
        colyseus_client_free(wrapper->native_client);
    }
    wrapper->native_client = colyseus_client_create(settings);

    // Cleanup
    free(endpoint_str);
    free(address);
    free(port);
}

// get_endpoint() -> String: the server's address
GOBUEXT_METHOD(gobuext_colyseus_client_get_endpoint) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;

    const ColyseusClientWrapper* self = (const ColyseusClientWrapper*)p_instance;
    gobuext_new_string(r_return, self && self->native_client && self->native_client->settings
        ? self->native_client->settings->server_address : NULL);
}

// =============================================================================
// Room event trampolines — push to queue (processed in poll on main thread)
// =============================================================================

static void on_room_message_with_type(const char* type, const uint8_t* data, size_t length, void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper || !room_wrapper->object_id) return;

    gobuext_room_event_t* event = (gobuext_room_event_t*)calloc(1, sizeof(gobuext_room_event_t));
    if (!event) return;
    event->type = ROOM_EVENT_MESSAGE;
    event->room_id = room_wrapper->object_id;
    event->str_data = type ? strdup(type) : strdup("");
    if (data && length > 0) {
        event->binary_data = (uint8_t*)malloc(length);
        if (event->binary_data) {
            memcpy(event->binary_data, data, length);
            event->binary_len = length;
        }
    }
    room_event_push(event);
}

static void on_room_join(void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper || !room_wrapper->object_id) return;

    gobuext_room_event_t* event = (gobuext_room_event_t*)calloc(1, sizeof(gobuext_room_event_t));
    if (!event) return;
    event->type = ROOM_EVENT_JOIN;
    event->room_id = room_wrapper->object_id;
    room_event_push(event);
}

static void on_room_state_change(void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper || !room_wrapper->object_id) return;

    gobuext_room_event_t* event = (gobuext_room_event_t*)calloc(1, sizeof(gobuext_room_event_t));
    if (!event) return;
    event->type = ROOM_EVENT_STATE_CHANGE;
    event->room_id = room_wrapper->object_id;
    room_event_push(event);
}

static void on_room_error(int code, const char* message, void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper || !room_wrapper->object_id) return;

    gobuext_room_event_t* event = (gobuext_room_event_t*)calloc(1, sizeof(gobuext_room_event_t));
    if (!event) return;
    event->type = ROOM_EVENT_ERROR;
    event->room_id = room_wrapper->object_id;
    event->code = code;
    event->str_data = message ? strdup(message) : strdup("");
    room_event_push(event);
}

static void on_room_leave(int code, const char* reason, void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper || !room_wrapper->object_id) return;

    gobuext_room_event_t* event = (gobuext_room_event_t*)calloc(1, sizeof(gobuext_room_event_t));
    if (!event) return;
    event->type = ROOM_EVENT_LEAVE;
    event->room_id = room_wrapper->object_id;
    event->code = code;
    event->str_data = reason ? strdup(reason) : strdup("");
    room_event_push(event);
}

static void on_room_drop(int code, const char* reason, void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper || !room_wrapper->object_id) return;

    gobuext_room_event_t* event = (gobuext_room_event_t*)calloc(1, sizeof(gobuext_room_event_t));
    if (!event) return;
    event->type = ROOM_EVENT_DROP;
    event->room_id = room_wrapper->object_id;
    event->code = code;
    event->str_data = reason ? strdup(reason) : strdup("");
    room_event_push(event);
}

static void on_room_reconnect(void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper || !room_wrapper->object_id) return;

    gobuext_room_event_t* event = (gobuext_room_event_t*)calloc(1, sizeof(gobuext_room_event_t));
    if (!event) return;
    event->type = ROOM_EVENT_RECONNECT;
    event->room_id = room_wrapper->object_id;
    room_event_push(event);
}

// Matchmaking success callback (shared by all matchmaking methods)
static void on_matchmaking_success(colyseus_room_t* room, void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (!room_wrapper) return;

    // Apply pending vtable BEFORE assigning native room (so it's set before join message processing)
    if (room_wrapper->pending_vtable) {
        colyseus_room_set_state_type(room, room_wrapper->pending_vtable);
        room_wrapper->pending_vtable = NULL;  // Clear after applying
    }

    room_wrapper->native_room = room;

    // Register room event callbacks
    colyseus_room_on_join(room, on_room_join, room_wrapper);
    colyseus_room_on_state_change(room, on_room_state_change, room_wrapper);
    colyseus_room_on_error(room, on_room_error, room_wrapper);
    colyseus_room_on_leave(room, on_room_leave, room_wrapper);
    colyseus_room_on_drop(room, on_room_drop, room_wrapper);
    colyseus_room_on_reconnect(room, on_room_reconnect, room_wrapper);
    colyseus_room_on_message_any_with_type_encoded(room, on_room_message_with_type, room_wrapper);
}

// Matchmaking error callback (shared by all matchmaking methods)
static void on_matchmaking_error(int code, const char* message, void* userdata) {
    ColyseusRoomWrapper* room_wrapper = (ColyseusRoomWrapper*)userdata;
    if (room_wrapper) {
        on_room_error(code, message, room_wrapper);
    }
}

// =============================================================================
// Common matchmaking helpers
// =============================================================================

// Function pointer type for matchmaking methods that take (client, name, options, callbacks)
typedef void (*matchmaking_with_options_fn)(colyseus_client_t*, const char*, const char*,
    colyseus_client_room_callback_t, colyseus_client_error_callback_t, void*);

// Function pointer type for reconnect (client, token, callbacks — no options)
typedef void (*matchmaking_reconnect_fn)(colyseus_client_t*, const char*,
    colyseus_client_room_callback_t, colyseus_client_error_callback_t, void*);

// A new _ColyseusRoom in `r_return` (which holds it), and its data; NULL if it could not be made.
static ColyseusRoomWrapper* new_room(GobuVariant* r_return) {
    if (!gobu->classdb_instantiate("_ColyseusRoom", r_return)) {
        return NULL;
    }
    return gobuext_colyseus_room_get_wrapper(gobu->variant_to_object(r_return));
}

// The matchmaking methods with options (join_or_create, create, join, join_by_id): the room comes back at
// once, and joins (or fails) later. p_args[0] = room name / room id, p_args[1] = options JSON.
static void matchmaking_with_options(void* p_instance, const GobuVariant* const* p_args, GobuVariant* r_return,
    matchmaking_with_options_fn native_fn) {
    ColyseusClientWrapper* client_wrapper = (ColyseusClientWrapper*)p_instance;
    if (!client_wrapper || !client_wrapper->native_client) {
        return;  // null
    }
    ColyseusRoomWrapper* room_wrapper = new_room(r_return);
    if (!room_wrapper) {
        return;
    }

    char* arg_cstr = gobuext_to_utf8(p_args[0]);
    char* options_cstr = gobuext_to_utf8(p_args[1]);
    native_fn(
        client_wrapper->native_client,
        arg_cstr,
        options_cstr,
        on_matchmaking_success,
        on_matchmaking_error,
        room_wrapper
    );
    free(arg_cstr);
    free(options_cstr);
}

GOBUEXT_METHOD(gobuext_colyseus_client_join_or_create) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    matchmaking_with_options(p_instance, p_args, r_return, colyseus_client_join_or_create);
}

GOBUEXT_METHOD(gobuext_colyseus_client_create) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    matchmaking_with_options(p_instance, p_args, r_return, colyseus_client_create_room);
}

GOBUEXT_METHOD(gobuext_colyseus_client_join) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    matchmaking_with_options(p_instance, p_args, r_return, colyseus_client_join);
}

GOBUEXT_METHOD(gobuext_colyseus_client_join_by_id) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    matchmaking_with_options(p_instance, p_args, r_return, colyseus_client_join_by_id);
}

// reconnect(reconnection_token: String): the seat the server holds with allowReconnection()
GOBUEXT_METHOD(gobuext_colyseus_client_reconnect) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;

    ColyseusClientWrapper* client_wrapper = (ColyseusClientWrapper*)p_instance;
    if (!client_wrapper || !client_wrapper->native_client) {
        return;  // null
    }
    ColyseusRoomWrapper* room_wrapper = new_room(r_return);
    if (!room_wrapper) {
        return;
    }

    char* token_cstr = gobuext_to_utf8(p_args[0]);
    matchmaking_reconnect_fn native_fn = colyseus_client_reconnect;
    native_fn(
        client_wrapper->native_client,
        token_cstr,
        on_matchmaking_success,
        on_matchmaking_error,
        room_wrapper
    );
    free(token_cstr);
}

// =============================================================================
// HTTP methods — background-threaded requests with signal-based responses
// =============================================================================

static int64_t g_http_request_counter = 0;

// =============================================================================
// HTTP response queue — thread-safe queue polled from main thread via poll()
// =============================================================================

#ifndef __EMSCRIPTEN__
#ifdef _WIN32
// Windows critical section
static CRITICAL_SECTION g_http_queue_lock;
static int g_http_queue_lock_init = 0;
#define HTTP_QUEUE_LOCK() do { if (!g_http_queue_lock_init) { InitializeCriticalSection(&g_http_queue_lock); g_http_queue_lock_init = 1; } EnterCriticalSection(&g_http_queue_lock); } while(0)
#define HTTP_QUEUE_UNLOCK() LeaveCriticalSection(&g_http_queue_lock)
#else
#include <pthread.h>
static pthread_mutex_t g_http_queue_lock = PTHREAD_MUTEX_INITIALIZER;
#define HTTP_QUEUE_LOCK() pthread_mutex_lock(&g_http_queue_lock)
#define HTTP_QUEUE_UNLOCK() pthread_mutex_unlock(&g_http_queue_lock)
#endif
#else
#define HTTP_QUEUE_LOCK()
#define HTTP_QUEUE_UNLOCK()
#endif

static gobuext_http_event_t* g_http_event_head = NULL;
static gobuext_http_event_t* g_http_event_tail = NULL;

// Room event queue (shares same lock macros as HTTP)
static gobuext_room_event_t* g_room_event_head = NULL;
static gobuext_room_event_t* g_room_event_tail = NULL;

static void http_event_push(gobuext_http_event_t* event) {
    event->next = NULL;
    HTTP_QUEUE_LOCK();
    if (g_http_event_tail) {
        g_http_event_tail->next = event;
    } else {
        g_http_event_head = event;
    }
    g_http_event_tail = event;
    HTTP_QUEUE_UNLOCK();
}

static gobuext_http_event_t* http_event_pop(void) {
    HTTP_QUEUE_LOCK();
    gobuext_http_event_t* event = g_http_event_head;
    if (event) {
        g_http_event_head = event->next;
        if (!g_http_event_head) g_http_event_tail = NULL;
    }
    HTTP_QUEUE_UNLOCK();
    return event;
}

static void room_event_push(gobuext_room_event_t* event) {
    event->next = NULL;
    HTTP_QUEUE_LOCK();
    if (g_room_event_tail) {
        g_room_event_tail->next = event;
    } else {
        g_room_event_head = event;
    }
    g_room_event_tail = event;
    HTTP_QUEUE_UNLOCK();
}

static gobuext_room_event_t* room_event_pop(void) {
    HTTP_QUEUE_LOCK();
    gobuext_room_event_t* event = g_room_event_head;
    if (event) {
        g_room_event_head = event->next;
        if (!g_room_event_head) g_room_event_tail = NULL;
    }
    HTTP_QUEUE_UNLOCK();
    return event;
}

static void room_event_free(gobuext_room_event_t* event) {
    if (event->str_data) free(event->str_data);
    if (event->binary_data) free(event->binary_data);
    free(event);
}

typedef struct {
    colyseus_http_t* http;
    char* path;
    char* body;
    int method;  // 0=GET, 1=POST, 2=PUT, 3=DELETE, 4=PATCH, 5=an auth operation (auth_op)
    int64_t request_id;
    GobuObjectId client_id;
    ColyseusClientWrapper* wrapper;  // For decrementing pending count
    // An auth operation (colyseus.gro's _AUTH_*: the order is the ABI); `body` is its options' JSON.
    colyseus_auth_t* auth;
    int auth_op;
    char* email;
    char* password;
} gobuext_http_request_t;

enum {
    GOBUEXT_AUTH_GET_USER_DATA,
    GOBUEXT_AUTH_REGISTER,
    GOBUEXT_AUTH_SIGN_IN,
    GOBUEXT_AUTH_SIGN_IN_ANONYMOUS,
    GOBUEXT_AUTH_SEND_PASSWORD_RESET,
};

static void gobuext_http_request_free(gobuext_http_request_t* req) {
    if (req->wrapper) {
        __sync_fetch_and_sub(&req->wrapper->pending_http_requests, 1);
    }
    free(req->path);
    free(req->body);
    free(req->email);
    free(req->password);
    free(req);
}

// Its answer is queued. On the web a request answers later (an asynchronous fetch), so it is freed here; elsewhere
// the call blocks and its thread frees it afterwards.
static void gobuext_http_request_answered(gobuext_http_request_t* req) {
#ifdef __EMSCRIPTEN__
    gobuext_http_request_free(req);
#else
    (void)req;
#endif
}

// HTTP success trampoline — pushes to event queue (processed in poll)
static void on_gobuext_http_success(const colyseus_http_response_t* response, void* userdata) {
    gobuext_http_request_t* req = (gobuext_http_request_t*)userdata;

    gobuext_http_event_t* event = (gobuext_http_event_t*)calloc(1, sizeof(gobuext_http_event_t));
    if (!event) return;
    event->is_error = 0;
    event->request_id = req->request_id;
    event->status_or_code = response->status_code;
    event->body_or_message = response->body ? strdup(response->body) : strdup("");
    event->client_id = req->client_id;
    http_event_push(event);
    gobuext_http_request_answered(req);
}

// HTTP error trampoline — pushes to event queue (processed in poll)
static void on_gobuext_http_error(const colyseus_http_error_t* error, void* userdata) {
    gobuext_http_request_t* req = (gobuext_http_request_t*)userdata;

    gobuext_http_event_t* event = (gobuext_http_event_t*)calloc(1, sizeof(gobuext_http_event_t));
    if (!event) return;
    event->is_error = 1;
    event->request_id = req->request_id;
    event->status_or_code = error->code;
    event->body_or_message = error->message ? strdup(error->message) : strdup("");
    event->client_id = req->client_id;
    http_event_push(event);
    gobuext_http_request_answered(req);
}

// An auth operation answers as an HTTP request does: `{"user": {...}, "token": "..."}`, the server's answer (the SDK
// keeps the token itself).
static void on_gobuext_auth_success(const colyseus_auth_data_t* data, void* userdata) {
    gobuext_http_request_t* req = (gobuext_http_request_t*)userdata;
    const char* user = data && data->user_json && data->user_json[0] ? data->user_json : "null";
    const char* token = data && data->token ? data->token : "";
    size_t size = strlen(user) + strlen(token) + 32;
    char* body = (char*)malloc(size);
    gobuext_http_event_t* event = (gobuext_http_event_t*)calloc(1, sizeof(gobuext_http_event_t));
    if (body && event) {
        // The token is a JWT (base64url and dots): nothing to escape.
        snprintf(body, size, "{\"user\":%s,\"token\":\"%s\"}", user, token);
        event->is_error = 0;
        event->request_id = req->request_id;
        event->status_or_code = 200;
        event->body_or_message = body;
        event->client_id = req->client_id;
        http_event_push(event);
    } else {
        free(body);
        free(event);
    }
    gobuext_http_request_answered(req);
}

static void on_gobuext_auth_error(const char* message, void* userdata) {
    gobuext_http_request_t* req = (gobuext_http_request_t*)userdata;
    gobuext_http_event_t* event = (gobuext_http_event_t*)calloc(1, sizeof(gobuext_http_event_t));
    if (event) {
        event->is_error = 1;
        event->request_id = req->request_id;
        event->status_or_code = 0;  // The SDK's auth reports only a message.
        event->body_or_message = strdup(message ? message : "");
        event->client_id = req->client_id;
        http_event_push(event);
    }
    gobuext_http_request_answered(req);
}

static void gobuext_auth_run(gobuext_http_request_t* req) {
    switch (req->auth_op) {
        case GOBUEXT_AUTH_GET_USER_DATA:
            colyseus_auth_get_user_data(req->auth, on_gobuext_auth_success, on_gobuext_auth_error, req);
            break;
        case GOBUEXT_AUTH_REGISTER:
            colyseus_auth_register_email_password(req->auth, req->email, req->password, req->body, on_gobuext_auth_success, on_gobuext_auth_error, req);
            break;
        case GOBUEXT_AUTH_SIGN_IN:
            colyseus_auth_signin_email_password(req->auth, req->email, req->password, on_gobuext_auth_success, on_gobuext_auth_error, req);
            break;
        case GOBUEXT_AUTH_SIGN_IN_ANONYMOUS:
            colyseus_auth_signin_anonymous(req->auth, req->body, on_gobuext_auth_success, on_gobuext_auth_error, req);
            break;
        case GOBUEXT_AUTH_SEND_PASSWORD_RESET:
            colyseus_auth_send_password_reset(req->auth, req->email, on_gobuext_auth_success, on_gobuext_auth_error, req);
            break;
        default:
            on_gobuext_auth_error("Unknown auth operation", req);
            break;
    }
}

// Thread function — runs the blocking HTTP call
static void gobuext_http_thread_func(gobuext_http_request_t* req) {
    switch (req->method) {
        case 0: colyseus_http_get(req->http, req->path, on_gobuext_http_success, on_gobuext_http_error, req); break;
        case 1: colyseus_http_post(req->http, req->path, req->body, on_gobuext_http_success, on_gobuext_http_error, req); break;
        case 2: colyseus_http_put(req->http, req->path, req->body, on_gobuext_http_success, on_gobuext_http_error, req); break;
        case 3: colyseus_http_delete(req->http, req->path, on_gobuext_http_success, on_gobuext_http_error, req); break;
        case 4: colyseus_http_patch(req->http, req->path, req->body, on_gobuext_http_success, on_gobuext_http_error, req); break;
        case 5: gobuext_auth_run(req); break;
    }
#ifndef __EMSCRIPTEN__
    gobuext_http_request_free(req);  // The call blocked: its answer is queued.
#endif
}

#ifndef __EMSCRIPTEN__
#ifdef _WIN32
static DWORD WINAPI gobuext_http_thread_wrapper(LPVOID arg) {
    gobuext_http_thread_func((gobuext_http_request_t*)arg);
    return 0;
}
#else
static void* gobuext_http_thread_wrapper(void* arg) {
    gobuext_http_thread_func((gobuext_http_request_t*)arg);
    return NULL;
}
#endif
#endif

// Dispatch HTTP request (method 5: the auth operation `auth_op`), returns request_id
static int64_t gobuext_http_dispatch_full(ColyseusClientWrapper* wrapper, int method, const char* path, const char* body,
    int auth_op, const char* email, const char* password) {
    if (!wrapper || !wrapper->native_client || !wrapper->native_client->http) return 0;

    gobuext_http_request_t* req = (gobuext_http_request_t*)calloc(1, sizeof(gobuext_http_request_t));
    if (!req) return 0;

    g_http_request_counter++;
    req->http = wrapper->native_client->http;
    req->path = strdup(path ? path : "");
    req->body = body && body[0] ? strdup(body) : NULL;
    req->method = method;
    req->request_id = g_http_request_counter;
    req->client_id = wrapper->object_id;
    req->wrapper = wrapper;
    req->auth = wrapper->native_client->auth;
    req->auth_op = auth_op;
    req->email = email ? strdup(email) : NULL;
    req->password = password ? strdup(password) : NULL;
    __sync_fetch_and_add(&wrapper->pending_http_requests, 1);

#ifdef __EMSCRIPTEN__
    gobuext_http_thread_func(req);
#elif defined(_WIN32)
    HANDLE thread = CreateThread(NULL, 0, gobuext_http_thread_wrapper, req, 0, NULL);
    if (thread) CloseHandle(thread);
#else
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, &attr, gobuext_http_thread_wrapper, req);
    pthread_attr_destroy(&attr);
#endif

    return g_http_request_counter;
}

static int64_t gobuext_http_dispatch(ColyseusClientWrapper* wrapper, int method, const char* path, const char* body) {
    return gobuext_http_dispatch_full(wrapper, method, path, body, -1, NULL, NULL);
}

// =============================================================================
// HTTP event processing — called from poll() on main thread
// =============================================================================

// Emits a signal of the object of `p_id`, if it is still alive. It is kept alive while the signal's handlers
// run (they may drop the game's last reference).
static void emit_signal_of(GobuObjectId p_id, const char* p_signal, const GobuVariant* const* p_args, int64_t p_arg_count) {
    GobuObject* object = gobu->object_from_instance_id(p_id);
    if (!object) return;

    GobuVariant keep;
    gobu->variant_new_object(&keep, object);
    gobu->object_emit_signal(object, p_signal, p_args, p_arg_count);
    gobu->variant_destroy(&keep);
}

// A signal (code: int, text: String): error, left, dropped, and the HTTP ones' last two
static void emit_code_and_text(GobuObjectId p_id, const char* p_signal, const GobuVariant* p_first, int64_t p_code, const char* p_text) {
    GobuVariant code;
    GobuVariant text;
    gobu->variant_new_int(&code, p_code);
    gobuext_new_string(&text, p_text);
    if (p_first) {
        const GobuVariant* args[3] = { p_first, &code, &text };
        emit_signal_of(p_id, p_signal, args, 3);
    } else {
        const GobuVariant* args[2] = { &code, &text };
        emit_signal_of(p_id, p_signal, args, 2);
    }
    gobu->variant_destroy(&code);
    gobu->variant_destroy(&text);
}

static void gobuext_http_process_events(void) {
    gobuext_http_event_t* event;
    while ((event = http_event_pop()) != NULL) {
        GobuVariant request_id;
        gobu->variant_new_int(&request_id, event->request_id);
        emit_code_and_text(event->client_id, event->is_error ? "_http_error" : "_http_response", &request_id,
            event->status_or_code, event->body_or_message);
        gobu->variant_destroy(&request_id);

        free(event->body_or_message);
        free(event);
    }
}

static void gobuext_room_process_events(void) {
    gobuext_room_event_t* event;
    while ((event = room_event_pop()) != NULL) {
        switch (event->type) {
            case ROOM_EVENT_JOIN:
                emit_signal_of(event->room_id, "joined", NULL, 0);
                break;

            case ROOM_EVENT_STATE_CHANGE:
                emit_signal_of(event->room_id, "state_changed", NULL, 0);
                break;

            case ROOM_EVENT_ERROR:
                emit_code_and_text(event->room_id, "error", NULL, event->code, event->str_data);
                break;

            case ROOM_EVENT_LEAVE:
                emit_code_and_text(event->room_id, "left", NULL, event->code, event->str_data);
                break;

            case ROOM_EVENT_DROP:
                emit_code_and_text(event->room_id, "dropped", NULL, event->code, event->str_data);
                break;

            case ROOM_EVENT_RECONNECT:
                emit_signal_of(event->room_id, "reconnected", NULL, 0);
                break;

            case ROOM_EVENT_MESSAGE: {
                // Decode msgpack on main thread
                GobuVariant type;
                gobuext_new_string(&type, event->str_data);

                GobuVariant data;
                bool decoded = false;
                if (event->binary_data && event->binary_len > 0) {
                    decoded = msgpack_to_gobu_variant(event->binary_data, event->binary_len, &data);
                    if (!decoded) {
                        gobu->variant_destroy(&data);
                    }
                }
                if (!decoded) {
                    gobu->variant_new_int(&data, 0);
                }

                const GobuVariant* args[2] = { &type, &data };
                emit_signal_of(event->room_id, "message_received", args, 2);

                gobu->variant_destroy(&type);
                gobu->variant_destroy(&data);
                break;
            }
        }

        room_event_free(event);
    }
}

// The SDK's network (websocket_transport.c; on the web, its polling of the browser's requests)
extern void colyseus_http_poll(void);
extern void colyseus_ws_poll(void);

// Static poll(): runs the network and emits the queued events' signals, on the main thread
GOBUEXT_METHOD(gobuext_colyseus_client_poll) {
    (void)p_method_userdata; (void)p_instance; (void)p_args;
    (void)p_argument_count; (void)r_return; (void)r_error;
    colyseus_http_poll();
    colyseus_ws_poll();
    gobuext_http_process_events();
    gobuext_room_process_events();
}

// =============================================================================
// HTTP methods (path, [body]) -> request id
// =============================================================================

// An HTTP request of `p_method` (0=GET, 1=POST, 2=PUT, 3=DELETE, 4=PATCH): its id, or 0 without a client.
static void http_request(void* p_instance, const GobuVariant* const* p_args, bool p_has_body, int p_method, GobuVariant* r_return) {
    char* path = gobuext_to_utf8(p_args[0]);
    char* body = p_has_body ? gobuext_to_utf8(p_args[1]) : NULL;
    gobu->variant_new_int(r_return, gobuext_http_dispatch((ColyseusClientWrapper*)p_instance, p_method, path, body));
    free(path);
    free(body);
}

// http_get(path: String) -> int (request_id)
GOBUEXT_METHOD(gobuext_colyseus_client_http_get) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    http_request(p_instance, p_args, false, 0, r_return);
}

// http_post(path: String, body: String) -> int
GOBUEXT_METHOD(gobuext_colyseus_client_http_post) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    http_request(p_instance, p_args, true, 1, r_return);
}

// http_put(path: String, body: String) -> int
GOBUEXT_METHOD(gobuext_colyseus_client_http_put) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    http_request(p_instance, p_args, true, 2, r_return);
}

// http_delete(path: String) -> int
GOBUEXT_METHOD(gobuext_colyseus_client_http_delete) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    http_request(p_instance, p_args, false, 3, r_return);
}

// http_patch(path: String, body: String) -> int
GOBUEXT_METHOD(gobuext_colyseus_client_http_patch) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;
    http_request(p_instance, p_args, true, 4, r_return);
}

// =============================================================================
// Auth token methods
// =============================================================================

// auth_set_token(token: String)
GOBUEXT_METHOD(gobuext_colyseus_client_auth_set_token) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_return; (void)r_error;
    ColyseusClientWrapper* wrapper = (ColyseusClientWrapper*)p_instance;
    if (!wrapper || !wrapper->native_client || !wrapper->native_client->http) return;

    char* token = gobuext_to_utf8(p_args[0]);
    colyseus_http_set_auth_token(wrapper->native_client->http, token);
    free(token);
}

// auth_request(op: int, email: String, password: String, options_json: String) -> int: an auth operation of
// colyseus.gro (_AUTH_*), answered by _http_response / _http_error as an HTTP request.
GOBUEXT_METHOD(gobuext_colyseus_client_auth_request) {
    (void)p_method_userdata; (void)p_argument_count; (void)r_error;

    const int64_t op = gobu->variant_to_int(p_args[0]);
    char* email = gobuext_to_utf8(p_args[1]);
    char* password = gobuext_to_utf8(p_args[2]);
    char* options = gobuext_to_utf8(p_args[3]);

    const int64_t rid = gobuext_http_dispatch_full((ColyseusClientWrapper*)p_instance, 5, "", options, (int)op, email, password);
    gobu->variant_new_int(r_return, rid);

    free(email);
    free(password);
    free(options);
}

// auth_signout(): forgets the token, here and in the secure storage. No request.
GOBUEXT_METHOD(gobuext_colyseus_client_auth_signout) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_return; (void)r_error;
    ColyseusClientWrapper* wrapper = (ColyseusClientWrapper*)p_instance;
    if (wrapper && wrapper->native_client && wrapper->native_client->auth) {
        colyseus_auth_signout(wrapper->native_client->auth);
    }
}

// auth_get_token() -> String: "" when signed out
GOBUEXT_METHOD(gobuext_colyseus_client_auth_get_token) {
    (void)p_method_userdata; (void)p_args; (void)p_argument_count; (void)r_error;
    ColyseusClientWrapper* wrapper = (ColyseusClientWrapper*)p_instance;

    const char* token = NULL;
    if (wrapper && wrapper->native_client && wrapper->native_client->http) {
        token = colyseus_http_get_auth_token(wrapper->native_client->http);
    }
    gobuext_new_string(r_return, token);
}
