# Changelog

The Colyseus plugin for Gobu. What came before it (the Colyseus Native SDK's own releases, up to 0.17.12) is in
the repository's history.

## Gobu

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
