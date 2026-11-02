# Changelog

The Colyseus plugin for Gobu. What came before it (the Colyseus Native SDK's own releases, up to 0.17.12) is in
the repository's history.

## Gobu

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
