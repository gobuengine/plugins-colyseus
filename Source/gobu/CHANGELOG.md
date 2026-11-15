# Changelog

The Colyseus plugin for Gobu. What came before it (the Colyseus Native SDK's own releases, up to 0.17.12) is in
the repository's history.

## Gobu

### 0.18.4: requests, ping, latency and seat reservations
- `room.request(type, data, callback, timeout_ms = 10000)`: a message the server answers, with the value its
  `onMessage(type)` handler returns. `callback(err, data)` as an HTTP request's; `err["code"]` is one of
  `Colyseus.REQUEST_REJECTED` (`ctx.reject(reason)`, the reason in `err["reason"]`), `REQUEST_FAULTED` (the handler
  failed, or there is none), `REQUEST_CLOSED` (the connection closed first) and `REQUEST_TIMED_OUT`. Native:
  `_ColyseusRoom.request(type, data)`, `cancel_request(id)` and the `_request_reply` signal.
- `room.ping(callback)`: the round trip over the room's connection, `callback(rtt_ms)`; false when not connected.
  Native: `ping()` and the `pong(rtt_ms)` signal.
- `client.get_latency(callback, endpoint = "", ping_count = 1, timeout_ms = 1500)` (the client's server, or an
  address) and `client.select_by_latency(endpoints, callback, ...)` (of several, the nearest), answered as HTTP:
  `data` is `{ "endpoint", "latency_ms" }`.
- `client.consume_seat_reservation(reservation)`: joins with a seat the server reserved (`matchMaker.joinOrCreate()`,
  `reserveSeatFor()`).
- The SDK runs polled (`colyseus_set_polled()`, as its other engines' bindings): sockets, matchmaking answers,
  reconnection and latency probes run in `poll()`, on the main thread, so the state is never decoded while the game
  reads it. `poll()` (and Online's) calls `colyseus_poll()`: on the web, automatic reconnection never retried,
  because nothing ran `colyseus_reconnect_poll()`.
- A room the game dropped before it was matched: the answer wrote into the freed room (its data was the
  matchmaking's userdata), and the connection stayed open. The matchmaking holds the room's id now; a room nobody
  has leaves at once.
- A client is freed when the game ends: the HTTP and Auth reply handlers held the client, and the client held them
  (its signals).
- Tried with a local Colyseus 0.18 server: the module (HTTP, Auth's error, latency of the server, of a closed port
  and the selection of the two, ping before and after joining, the six kinds of request, a seat reservation, a
  dropped room, messages, state, leave) and Online (a host and a game that joins). The web and Android libraries
  build.

### 0.18.3: the Colyseus Native SDK 0.18.15
- `Source/src` and `Source/include` are the SDK's 0.18.15 (it was the copy of 2026-08-21, before its 0.18.0). Its
  fixes: automatic reconnection worked once and `reconnect()` with a token never did; leaving a room while it
  reconnected could come back or deadlock; leaving with an instance in two places of the state crashed; a failed
  join reports the server's text and code (not 500, nor a second error); a signed-in client sends its token to the
  room (`onAuth`), and on the web the auth routes answer; Android resolves hostnames; Windows stops waiting on a
  refused connection; `localhost` works with a server that listens only on IPv6; callbacks as the TypeScript SDK's
  (no `listen` for a value sent unchanged, `on_add` once for primitive maps, `ArraySchema` in the server's order);
  up to 8 callbacks layers per room (`room.callbacks()` used to take the only one). It has the predict layer too
  (not bound yet).
- Gobu's own changes to the SDK are in it now (the tick thread's close, `ArraySchema` order and renumbering, a
  re-sent ADD), except the web's macro: `gobu/build.zig` defines `GDEXTENSION_SIDE_MODULE` (the SDK's name) next
  to `COLYSEUS_SIDE_MODULE`, and lists the SDK's new sources (clock, input, latency, net delay, quantize, predict).
- The room state needs a Colyseus 0.18 server (`@colyseus/schema` 5): `Source/server` is on Colyseus 0.18. Online
  (the relay room, bytes only) plays on 0.17 and 0.18 servers.
- Tried with a local server: the `Colyseus` module (HTTP, Auth's error, join, messages, state, `listen`, `on_add`,
  leave) on 0.18, and Online (a host and a game that joins, host migration) on 0.17 and 0.18. The web and Android
  libraries build; they weren't run.

### 0.18.2: the Grost module for today's Grost
- `colyseus.gro` compiles again: Grost removed `any` and values of unknown type (2026-11-03), and the module
  still had them (151 errors), so a game that used `Colyseus.*` didn't compile. The native objects are typed
  (`_ColyseusClient`, `_ColyseusRoom`, `_ColyseusCallbacks`); the Online provider (`colyseus_provider.c`) never
  used the module and is unchanged.
- Typed where it took any value: `Callbacks.listen/on_add/on_remove(property, callback, target = {})` and
  `on_change(callback, property = "", target = {})` (`target`: a nested schema; none = the root state);
  `room.send_message(type: String, data: Dictionary = {})` and `send_message_int(type: int, data)` (an empty
  `data` sends none); `room.set_state_type(schema)` and `set_state_type_name(name)`; `Colyseus.field(name, type)`,
  `field_of(name, type, schema)` and `field_of_type(name, type, child_type)`; `schema(fields: []Dictionary)`;
  HTTP bodies are Dictionaries.
- HTTP and Auth answer `callback(err: Dictionary, data: Dictionary)`: `err` empty when it went well, else
  `{ "code", "message" }`; `data` the reply's JSON object (other JSON in `data["value"]`, text in `data["text"]`).
- The reply table HTTP and Auth share lives in the module (one per client), not in a Dictionary each struct held:
  a Dictionary is a value in Grost now.
- Out: the predict layer's wrappers (`InputHandle`, `Clock`, `Predict`, `Spawns`, `EventChannel`, `Reconciler`,
  `room.input()/predict()/clock()/set_latency()/net_pump()/drop()`): they called native methods this binding
  never had. Also `get_default_value()`, which had no single type.
- Tried against a local server with a room of its own: HTTP (a reply and a 404), Auth's error, join, messages by
  name and by number, the state, `on_add`, leave.
- `listen()` on a `number` field (the schemas' usual number) gave null values: `native_value_to_variant` had no
  case for `COLYSEUS_FIELD_NUMBER`, which the decoder keeps as a double, as `float64`. The libraries are rebuilt
  with Zig 0.15.2 (Windows, web, Android).

### Plugins window
- `icon.svg` (a colosseum) and `"icon"` in `plugin.json`: its image in Gobu's Edit > Plugins, which installs it
  from this repository's latest release (`forja package plugin colyseus` makes its files).

### Android
- `plugin.json` has the Android libraries (`android.arm64`, `android.arm64.debug`): built with Zig and the NDK
  (`ANDROID_NDK_HOME`). The release goes without its debug sections, with the NDK's `llvm-strip` (2.7 MB instead
  of 15; Zig's own strip makes it build compiler_rt's tests). The export puts it in the package's
  `lib/arm64-v8a`; it needs Gobu's Android templates with plugins.
- `plugin.json` asks for `INTERNET` (`android_permissions`): without it Android doesn't let the game reach the
  server; enabling the plugin ticks it in the project's Android presets (disabling it unticks it). Tried on a phone (Android
  11, arm64): it hosts a room a game on a computer joins.

### Host, Join and a dedicated server
- The `colyseus` provider opens and joins games too, not only matches: `Online.host()` and `Online.serve()`
  (Start: Host, Start: Dedicated Server, the exported server) make a room of Session > Mode where this game is
  peer 1, a host that plays or a dedicated server (its room takes one game more); `Online.join()` (Start: Join)
  joins a room of the mode that is open, and never makes one. The port is the Colyseus server's.
- With Gobu's Online: when a dedicated server leaves its room, the game ends for its players (no migration).

### The room sends players away
- The `colyseus` provider has `kick`: when the host sends a player away (`Online.kick()`, or a login it turned
  away), `GobuRelayRoom` closes that game's connection (code 4003), taking the request (`"k"`) from the host
  only. Before, the player's game left by itself, and one that didn't (a modified game) stayed in the room.
  A server of your own needs this `GobuRelayRoom`: an older one ignores the request.

### Straight on gobu_plugin.h
- The binding used NativeExtension, Gobu's former extension API, through a layer that turned its calls into
  `gobu_plugin.h` ones (`gobu_compat.c`, with `nativeextension_interface.h`). Now it calls `gobu_plugin.h`
  directly: every engine value is a `GobuVariant`, each method is a `GobuMethodFunc` with Variant arguments, and
  `plugin.c` registers the classes with `register_class` / `register_method` / `register_signal` (the
  call/ptrcall wrappers of each method are gone: the engine checks the arguments' count and types).
  `gobu_plugin.h` is the engine's version 3; the plugin still needs only version 1 (2 for the provider).
- A new room or callbacks object is made with `classdb_instantiate()` and its data read with
  `object_get_plugin_instance()`, instead of the "last instance made" and the table of rooms by id.
- Events handed over in `poll()` keep the room's or client's object id, not its pointer: a room the game
  dropped before its event came is skipped (`object_from_instance_id()`), not used after it was freed. The
  client no longer throws away every client's pending HTTP answers when one is freed.
- The state no longer leaks: each nested schema, array and map was a Dictionary or Array copied into its parent
  and never released, and each field's value too.
- `msgpack_variant.c`: a binary value is one `variant_new_packed_byte_array()`; a payload that fails to decode
  releases the containers it had opened.
- The web build's define is `COLYSEUS_SIDE_MODULE` (was `NATIVEEXTENSION_SIDE_MODULE`); `build.zig` no longer
  has the `gobu-double-precision` option (a `GobuVariant` is 24 bytes either way).

### Colyseus.Auth works
- `colyseus.gro`'s `Colyseus.Auth` called a native `auth_request()` (and `auth_signout()`) the binding did not
  have. Now `_ColyseusClient.auth_request(op, email, password, options_json)` runs the SDK's auth operation (user
  data, register, sign in, anonymous, password reset) as an HTTP request is run, and answers through
  `_http_response` (`{"user": {...}, "token": "..."}`; the SDK keeps the token) or `_http_error`;
  `auth_signout()` forgets the token.
- On the web, an HTTP request was freed as soon as it was sent, but the browser answers later: its answer read
  freed memory. It is now freed when it answers.

### Arrays of the state as the server has them
- A child added once came twice: the full state and a patch both carry what changed when a client joined, and an
  ADD at index 0 of a non-empty array is an unshift. The decoder now skips the same value at its index, as
  @colyseus/schema does (`value !== previousValue`).
- Items came out newest first: `colyseus_array_schema_foreach()` walked the list as kept (newest first); it
  now goes in index order.
- After a deletion, the items left kept their old indices, so the server's next change landed on the wrong one
  (deleting the first item, then adding one, overwrote the second). `on_decode_end()` now compacts them, as
  @colyseus/schema does and as its comment said.

### A game quits cleanly with a Colyseus session open
- A game that quit with a room open crashed (an access violation, or a heap corruption) in two ways:
  - The WebSocket transport took any thread for its own tick thread: `ws_close_impl()` checked a shared
    `in_tick_thread` flag, true for every thread while the tick thread runs, so closing from the game deferred
    the close and returned without waiting, and the transport was freed under the running thread. It now
    compares the thread's identity, and from another thread waits for the tick thread before freeing the TLS
    context, the socket and wslay (it freed them first).
  - The `colyseus` provider left its room and client alive when Online closed the session ("a network thread may
    still be using them"), so their threads ran on after Gobu unloaded the library. `provider_close()` now frees
    the client (waiting for the matchmaking), then leaves and frees the room (waiting for its threads), and the
    settings; a room matched after the close is freed in `on_matched()`.

### Only Gobu and Grost
- `set_state_type()` takes the schema a game describes with `Colyseus.schema()` / `Colyseus.field()` (a
  Dictionary, or an Array of fields), nested schemas included (`colyseus_schema_definition.c`). Before, it took a
  script *class* with a `definition()` method, which Grost does not have, and a Dictionary was ignored. The state
  is Dictionaries, as when the server's reflection describes it.
- Gone: the bridge to script classes (typed instances, `_set_field`, `__ref_id` on objects) and the branches of
  `listen()` / `on_add()` / `on_remove()` / `on_change()` that took such objects; state objects are Dictionaries.
- Names: `plugin.c` (the entry and the classes, was `register_types.c`), `msgpack_decoder.zig/.h` (was
  `msgpack_gobu`), `gobuext_` for every internal function, `COLYSEUS_CB_*`, `NativeExtensionObjectInstanceID`;
  the unused `GDE_EXPORT` is gone.

### Source: only what the plugin uses
- `gobu/` (the binding and its build, which puts the libraries in `../colyseus/bin`), the SDK (`src/`,
  `include/`, `third_party/`) and `server/` (a Colyseus server with `GobuRelayRoom` only).

### Online provider "colyseus"
- The plugin registers the `colyseus` provider of Gobu's Online (`gobu_plugin.h` version 2,
  `colyseus_provider.c`): quick match by Session > Mode through the server's `GobuRelayRoom`, which passes the
  games' packets along. When the host leaves, the player with the lowest id becomes the host (host migration).

### Web
- WebAssembly side modules for Gobu's dynamically linked web templates without threads (Emscripten 6.0.8, the
  SDK's web transport and HTTP over the browser).

### A Gobu plugin
- Gobu loads the binding through its C plugin interface, `gobu_plugin.h`: `gobu_compat.c` implements the
  NativeExtension functions the binding uses on top of it, and the entry is `gobu_plugin_init`. Windows x86_64
  debug and release libraries.
- From the Colyseus Native SDK 0.17.12: automatic reconnection (`dropped`, `reconnected`,
  `set_reconnection_options()`), `get_reconnection_token()` and `client.reconnect()`, and one TLS trust store for
  the socket and HTTPS matchmaking (the bundled Mozilla roots, the system's, and
  `network/tls/certificate_bundle_override`).
