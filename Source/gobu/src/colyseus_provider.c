/*
 * The "colyseus" online provider for Gobu's Online (gobu_plugin.h, version 2): Online.quick_match() joins a
 * Colyseus room of the same mode with room, or makes one (the server's GobuRelayRoom, "gobu_relay"). The
 * first game in a room is the host (peer 1, it runs the world); the room passes the packets along. When the
 * host leaves, the room makes the player with the lowest id the host (Session > Host Migration), or ends.
 * Online.host() and serve() (a dedicated server) make a room of the session's mode; Online.join() only joins
 * one that is open.
 *
 * The server is Project Settings > Online > Session > Address ("localhost", "wss://example.com") and Port.
 *
 * Natively the client works on its own threads: its callbacks queue events, which poll() hands to the engine
 * on the main thread. A connection that is closed is removed from `live` first, so a late callback finds it
 * gone and does nothing.
 */

#include "gobu_plugin.h"
#include "tls_certificates.h"

#include <colyseus/client.h>
#include <colyseus/room.h>
#include <colyseus/settings.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __EMSCRIPTEN__
#ifdef _WIN32
#include <windows.h>
typedef CRITICAL_SECTION provider_mutex_t;
#define MUTEX_INIT(m) InitializeCriticalSection(m)
#define MUTEX_LOCK(m) EnterCriticalSection(m)
#define MUTEX_UNLOCK(m) LeaveCriticalSection(m)
#else
#include <pthread.h>
typedef pthread_mutex_t provider_mutex_t;
#define MUTEX_INIT(m) pthread_mutex_init(m, NULL)
#define MUTEX_LOCK(m) pthread_mutex_lock(m)
#define MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#endif
#else
/* The web has one thread: the callbacks come from the browser's event loop. */
typedef int provider_mutex_t;
#define MUTEX_INIT(m) (void)(m)
#define MUTEX_LOCK(m) (void)(m)
#define MUTEX_UNLOCK(m) (void)(m)
#endif

#define ROOM_NAME "gobu_relay"
#define MAX_LIVE 16

static const GobuPluginApi *gobu;

typedef enum {
	EVENT_CONNECTED,
	EVENT_FAILED,
	EVENT_JOINED,
	EVENT_LEFT,
	EVENT_PACKET,
	EVENT_CLOSED,
	EVENT_HOST_CHANGED,
} EventKind;

typedef struct Event {
	EventKind kind;
	int32_t id;
	uint8_t *data;
	size_t size;
	char *text;
	struct Event *next;
} Event;

typedef struct Connection {
	GobuOnlineEvents events;
	colyseus_settings_t *settings; /* The client's: it does not free them. */
	colyseus_client_t *client;
	colyseus_room_t *room;
	Event *head;
	Event *tail;
} Connection;

/* All the connections that are open; the callbacks only touch those. */
static provider_mutex_t live_mutex;
static Connection *live[MAX_LIVE];

static int find_live(Connection *p_connection) {
	for (int i = 0; i < MAX_LIVE; i++) {
		if (live[i] == p_connection) {
			return i;
		}
	}
	return -1;
}

/* Queues an event of a connection still open (from any thread). */
static void push(Connection *p_connection, EventKind p_kind, int32_t p_id, const uint8_t *p_data, size_t p_size, const char *p_text) {
	MUTEX_LOCK(&live_mutex);
	if (find_live(p_connection) >= 0) {
		Event *event = (Event *)calloc(1, sizeof(Event));
		event->kind = p_kind;
		event->id = p_id;
		if (p_size > 0) {
			event->data = (uint8_t *)malloc(p_size);
			memcpy(event->data, p_data, p_size);
			event->size = p_size;
		}
		event->text = p_text ? strdup(p_text) : NULL;
		if (p_connection->tail) {
			p_connection->tail->next = event;
		} else {
			p_connection->head = event;
		}
		p_connection->tail = event;
	}
	MUTEX_UNLOCK(&live_mutex);
}

static int32_t read_id(const uint8_t *p_data) {
	return (int32_t)((uint32_t)p_data[0] | ((uint32_t)p_data[1] << 8) | ((uint32_t)p_data[2] << 16) | ((uint32_t)p_data[3] << 24));
}

static void write_id(uint8_t *r_data, int32_t p_id) {
	r_data[0] = (uint8_t)(p_id & 0xff);
	r_data[1] = (uint8_t)((p_id >> 8) & 0xff);
	r_data[2] = (uint8_t)((p_id >> 16) & 0xff);
	r_data[3] = (uint8_t)((p_id >> 24) & 0xff);
}

/* The room's messages ("w" welcome, "p" packet, "j"/"l" a peer joined/left: GobuRelayRoom.ts). */

static void on_welcome(const uint8_t *p_data, size_t p_size, void *p_userdata) {
	if (p_size >= 4) {
		push((Connection *)p_userdata, EVENT_CONNECTED, read_id(p_data), NULL, 0, NULL);
	}
}

static void on_packet(const uint8_t *p_data, size_t p_size, void *p_userdata) {
	if (p_size > 4) {
		push((Connection *)p_userdata, EVENT_PACKET, read_id(p_data), p_data + 4, p_size - 4, NULL);
	}
}

static void on_joined(const uint8_t *p_data, size_t p_size, void *p_userdata) {
	if (p_size >= 4) {
		push((Connection *)p_userdata, EVENT_JOINED, read_id(p_data), NULL, 0, NULL);
	}
}

static void on_left(const uint8_t *p_data, size_t p_size, void *p_userdata) {
	if (p_size >= 4) {
		push((Connection *)p_userdata, EVENT_LEFT, read_id(p_data), NULL, 0, NULL);
	}
}

static void on_host_changed(const uint8_t *p_data, size_t p_size, void *p_userdata) {
	if (p_size >= 4) {
		push((Connection *)p_userdata, EVENT_HOST_CHANGED, read_id(p_data), NULL, 0, NULL);
	}
}

static void on_leave(int p_code, const char *p_reason, void *p_userdata) {
	(void)p_code;
	push((Connection *)p_userdata, EVENT_CLOSED, 0, NULL, 0, p_reason && p_reason[0] ? p_reason : "The game ended (the host left, or the connection was lost).");
}

static void on_room_error(int p_code, const char *p_message, void *p_userdata) {
	char text[256];
	snprintf(text, sizeof(text), "Colyseus: %s (%d).", p_message ? p_message : "error", p_code);
	push((Connection *)p_userdata, EVENT_CLOSED, 0, NULL, 0, text);
}

static void on_matched(colyseus_room_t *p_room, void *p_userdata) {
	Connection *connection = (Connection *)p_userdata;
	MUTEX_LOCK(&live_mutex);
	const bool open = find_live(connection) >= 0;
	if (open) {
		connection->room = p_room;
	}
	MUTEX_UNLOCK(&live_mutex);
	if (!open) {
		/* Closed while matching: this room is nobody's. on_success is the last thing the SDK does with it. */
		colyseus_room_leave(p_room, true);
		colyseus_room_free(p_room);
		return;
	}
	colyseus_room_on_message_bytes(p_room, "w", on_welcome, connection);
	colyseus_room_on_message_bytes(p_room, "p", on_packet, connection);
	colyseus_room_on_message_bytes(p_room, "j", on_joined, connection);
	colyseus_room_on_message_bytes(p_room, "l", on_left, connection);
	colyseus_room_on_message_bytes(p_room, "m", on_host_changed, connection);
	colyseus_room_on_leave(p_room, on_leave, connection);
	colyseus_room_on_error(p_room, on_room_error, connection);
	/* The room greets on join; asking again covers a greeting that came before the callbacks. */
	colyseus_room_send_bytes(p_room, "h", (const uint8_t *)"", 0);
}

static void on_match_error(int p_code, const char *p_message, void *p_userdata) {
	char text[256];
	snprintf(text, sizeof(text), "Colyseus could not find or make a game: %s (%d). Is the server running at Session > Address/Port, with the \"" ROOM_NAME "\" room?", p_message ? p_message : "error", p_code);
	push((Connection *)p_userdata, EVENT_FAILED, 0, NULL, 0, text);
}

/* The provider's functions (GobuOnlineProviderInfo). */

static void set_error(GobuVariant *r_error, const char *p_text) {
	gobu->variant_new_string(r_error, p_text, -1);
}

static char *setting_text(const char *p_name, const char *p_default) {
	GobuVariant value;
	char *text = NULL;
	if (gobu->get_setting(p_name, &value)) {
		const int64_t length = gobu->variant_to_utf8(&value, NULL, 0);
		if (length > 0) {
			text = (char *)malloc((size_t)length + 1);
			gobu->variant_to_utf8(&value, text, length + 1);
		}
	}
	gobu->variant_destroy(&value);
	return text ? text : strdup(p_default);
}

/* A connection to the Colyseus server at `p_address` (a host, "localhost", or a URL, "wss://example.com") and
 * `p_port`, unless the address names its own; NULL with `r_error` set when there is no room for another one. */
static Connection *new_connection(const GobuOnlineEvents *p_events, const char *p_address, int32_t p_port, GobuVariant *r_error) {
	Connection *connection = (Connection *)calloc(1, sizeof(Connection));
	connection->events = *p_events;

	MUTEX_LOCK(&live_mutex);
	int slot = find_live(NULL);
	if (slot >= 0) {
		live[slot] = connection;
	}
	MUTEX_UNLOCK(&live_mutex);
	if (slot < 0) {
		free(connection);
		set_error(r_error, "Colyseus: too many games at once.");
		return NULL;
	}

	const char *host = p_address && p_address[0] ? p_address : "localhost";
	bool secure = false;
	if (strncmp(host, "wss://", 6) == 0 || strncmp(host, "https://", 8) == 0) {
		secure = true;
	}
	const char *scheme = strstr(host, "://");
	if (scheme) {
		host = scheme + 3;
	}
	char host_only[256];
	snprintf(host_only, sizeof(host_only), "%s", host);
	char *slash = strchr(host_only, '/');
	if (slash) {
		*slash = '\0';
	}
	char *colon = strchr(host_only, ':');
	char port[16];
	if (colon) {
		snprintf(port, sizeof(port), "%s", colon + 1);
		*colon = '\0';
	} else {
		snprintf(port, sizeof(port), "%d", (int)p_port);
	}

	colyseus_settings_t *settings = colyseus_settings_create();
	colyseus_settings_set_address(settings, host_only);
	colyseus_settings_set_port(settings, port);
	colyseus_settings_set_secure(settings, secure);
	const unsigned char *ca_data = gobuext_tls_get_ca_certificates();
	const size_t ca_size = gobuext_tls_get_ca_certificates_len();
	if (ca_data && ca_size > 0) {
		colyseus_settings_set_ca_certificates(settings, ca_data, ca_size);
	}
	connection->settings = settings;
	connection->client = colyseus_client_create(settings);
	return connection;
}

/* The room's options as JSON: the mode (`p_mode`, or Session > Mode when NULL), how many games it takes, and
 * Session > Host Migration. */
static void room_options(char *r_options, size_t p_size, const char *p_mode, int32_t p_max_clients) {
	char *session_mode = p_mode ? NULL : setting_text("online/session/mode", "");
	/* Only letters, digits and a few signs are kept (it is a game's name). */
	char mode[64];
	size_t length = 0;
	for (const char *c = p_mode ? p_mode : session_mode; *c && length < sizeof(mode) - 1; c++) {
		if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9') || *c == '_' || *c == '-' || *c == ' ') {
			mode[length++] = *c;
		}
	}
	mode[length] = '\0';
	free(session_mode);
	/* On unless set to false. */
	char *migration = setting_text("online/session/host_migration", "true");
	const bool migrate = strcmp(migration, "false") != 0;
	free(migration);
	snprintf(r_options, p_size, "{\"mode\":\"%s\",\"maxClients\":%d,\"migration\":%s}", mode, (int)p_max_clients, migrate ? "true" : "false");
}

/* Session > Address and Port. */
static Connection *new_session_connection(const GobuOnlineEvents *p_events, GobuVariant *r_error) {
	char *address = setting_text("online/session/address", "localhost");
	char *port_text = setting_text("online/session/port", "2567");
	Connection *connection = new_connection(p_events, address, atoi(port_text), r_error);
	free(address);
	free(port_text);
	return connection;
}

/* A room of `p_mode` with room, or a new one. */
static void *provider_match(void *p_userdata, const char *p_mode, int32_t p_max_players, const GobuOnlineEvents *p_events, GobuVariant *r_error) {
	(void)p_userdata;
	Connection *connection = new_session_connection(p_events, r_error);
	if (!connection) {
		return NULL;
	}
	char options[192];
	room_options(options, sizeof(options), p_mode ? p_mode : "", p_max_players);
	colyseus_client_join_or_create(connection->client, ROOM_NAME, options, on_matched, on_match_error, connection);
	return connection;
}

/* A new room of Session > Mode, made by this game: peer 1, a host that plays or a dedicated server (which is not
 * a player: its room takes one game more). Its port is the Colyseus server's. */
static void *provider_open(void *p_userdata, int32_t p_port, int32_t p_max_players, const GobuOnlineEvents *p_events, GobuVariant *r_error) {
	(void)p_userdata;
	char *address = setting_text("online/session/address", "localhost");
	Connection *connection = new_connection(p_events, address, p_port, r_error);
	free(address);
	if (!connection) {
		return NULL;
	}
	char options[192];
	room_options(options, sizeof(options), NULL, p_max_players + 1);
	colyseus_client_create_room(connection->client, ROOM_NAME, options, on_matched, on_match_error, connection);
	return connection;
}

/* A room of Session > Mode that is open (a dedicated server's, or a host's), on the Colyseus server at
 * `p_address`: never a new one. */
static void *provider_join(void *p_userdata, const char *p_address, int32_t p_port, const GobuOnlineEvents *p_events, GobuVariant *r_error) {
	(void)p_userdata;
	Connection *connection = new_connection(p_events, p_address, p_port, r_error);
	if (!connection) {
		return NULL;
	}
	char options[192];
	room_options(options, sizeof(options), NULL, 0);
	colyseus_client_join(connection->client, ROOM_NAME, options, on_matched, on_match_error, connection);
	return connection;
}

static void provider_send(void *p_connection, int32_t p_to, const uint8_t *p_data, int64_t p_size, int32_t p_mode) {
	(void)p_mode; /* The room is reliable (WebSocket). */
	Connection *connection = (Connection *)p_connection;
	if (!connection->room || p_size <= 0) {
		return;
	}
	uint8_t *message = (uint8_t *)malloc((size_t)p_size + 4);
	write_id(message, p_to);
	memcpy(message + 4, p_data, (size_t)p_size);
	colyseus_room_send_bytes(connection->room, "s", message, (size_t)p_size + 4);
	free(message);
}

/* The server sends a player away: the room closes that game's connection (it takes "k" from the host only). */
static void provider_kick(void *p_connection, int32_t p_peer) {
	Connection *connection = (Connection *)p_connection;
	if (!connection->room || p_peer < 2) {
		return;
	}
	uint8_t message[4];
	write_id(message, p_peer);
	colyseus_room_send_bytes(connection->room, "k", message, 4);
}

/* The client's own work on the web (its HTTP and WebSocket answers wait to be polled); natively nothing. */
extern void colyseus_http_poll(void);
extern void colyseus_ws_poll(void);

static void provider_poll(void *p_connection) {
	Connection *connection = (Connection *)p_connection;
	colyseus_http_poll();
	colyseus_ws_poll();
	MUTEX_LOCK(&live_mutex);
	Event *event = connection->head;
	connection->head = NULL;
	connection->tail = NULL;
	MUTEX_UNLOCK(&live_mutex);
	const GobuOnlineEvents *events = &connection->events;
	while (event) {
		switch (event->kind) {
			case EVENT_CONNECTED:
				events->connected(events->connection, event->id);
				break;
			case EVENT_FAILED:
				events->failed(events->connection, event->text);
				break;
			case EVENT_JOINED:
				events->peer_joined(events->connection, event->id);
				break;
			case EVENT_LEFT:
				events->peer_left(events->connection, event->id);
				break;
			case EVENT_PACKET:
				events->packet(events->connection, event->id, event->data, (int64_t)event->size);
				break;
			case EVENT_CLOSED:
				events->closed(events->connection, event->text);
				break;
			case EVENT_HOST_CHANGED:
				if (events->host_changed) {
					events->host_changed(events->connection, event->id);
				}
				break;
		}
		Event *next = event->next;
		free(event->data);
		free(event->text);
		free(event);
		event = next;
	}
}

static void provider_close(void *p_connection) {
	Connection *connection = (Connection *)p_connection;
	MUTEX_LOCK(&live_mutex);
	const int slot = find_live(connection);
	if (slot >= 0) {
		live[slot] = NULL; /* From now on the callbacks ignore it. */
	}
	Event *event = connection->head;
	MUTEX_UNLOCK(&live_mutex);
	while (event) {
		Event *next = event->next;
		free(event->data);
		free(event->text);
		free(event);
		event = next;
	}
	/* Nothing of the plugin may run once the game is done with it (Gobu unloads its library at the end): every
	 * network thread is waited for. First the matchmaking's: once the client is freed, on_matched has run or
	 * never will (a room it made after this close is freed there: the connection is no longer live). */
	colyseus_client_free(connection->client);
	/* Then the room: leaving queues its last message, freeing it sends that and waits for its threads. Its
	 * callbacks (on_leave) find the connection closed. */
	if (connection->room) {
		colyseus_room_leave(connection->room, true);
		colyseus_room_free(connection->room);
	}
	colyseus_settings_free(connection->settings);
	free(connection);
}

/* Called from gobu_plugin_init (plugin.c). */
bool colyseus_register_online_provider(const GobuPluginApi *p_api, GobuPluginHandle p_plugin) {
	if (p_api->version < 2 || p_api->size < offsetof(GobuPluginApi, register_online_provider) + sizeof(void *)) {
		return true; /* An engine without online providers: the plugin still works. */
	}
	gobu = p_api;
	MUTEX_INIT(&live_mutex);
	GobuOnlineProviderInfo info;
	memset(&info, 0, sizeof(info));
	info.name = "colyseus";
	info.flags = GOBU_ONLINE_CAN_MATCH | GOBU_ONLINE_CAN_OPEN | GOBU_ONLINE_CAN_JOIN;
	info.match = provider_match;
	info.open = provider_open;
	info.join = provider_join;
	info.send = provider_send;
	info.kick = provider_kick;
	info.poll = provider_poll;
	info.close = provider_close;
	return p_api->register_online_provider(p_plugin, &info);
}
