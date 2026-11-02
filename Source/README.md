# Colyseus plugin: source

The code of the Colyseus plugin for Gobu: the [Colyseus Native SDK](https://github.com/colyseus/native-sdk)
(C), its binding as a Gobu plugin, and the server Gobu's online games use.

```
Source/
├─ gobu/          the binding: a Gobu plugin (gobu_plugin.h) in C, and its build (build.zig)
├─ src/           the Colyseus Native SDK (C, and the Zig of its HTTP, TLS certificates and msgpack)
├─ include/       its headers
├─ third_party/   its dependencies (submodules: sds, uthash, cJSON, wslay, mbedtls)
└─ server/        a Colyseus server with GobuRelayRoom, for the "colyseus" provider of Gobu's Online
```

Gobu loads the binding as an external plugin through its C interface, `gobu/include/gobu_plugin.h` (a copy
of the engine's). The binding was written against NativeExtension, Gobu's former extension API:
`gobu/src/gobu_compat.c` implements the NativeExtension functions it uses on top of `gobu_plugin.h`, and the
entry point is `gobu_plugin_init` (`gobu/src/plugin.c`). It also registers the `colyseus` provider of
Online (`gobu/src/colyseus_provider.c`). The Grost side is `../colyseus/colyseus.gro`.

## Building

Needs **Zig 0.15.x** (verified with 0.15.2; the Zig sources and the `zig_msgpack` dependency don't build on
0.17-dev). Get the submodules first: clone with `--recursive`, or `git submodule update --init`. From `gobu/`:

```bash
zig build -Dtarget=x86_64-windows -Doptimize=ReleaseFast   # colyseus_gobu.windows.x86_64.release.dll
zig build -Dtarget=x86_64-windows -Doptimize=Debug         # colyseus_gobu.windows.x86_64.debug.dll
```

The web (a WebAssembly side module, for Gobu's web templates without threads) needs an Emscripten SDK
(verified with **6.0.8**): make a link `gobu/emsdk` to it (Windows: `mklink /J emsdk C:\emsdk`, in `gobu/`;
it is not in the repository), put `<emsdk>/upstream/emscripten` on `PATH` and:

```bash
zig build -Dtarget=wasm32-emscripten -Doptimize=ReleaseFast   # libcolyseus_gobu.web.wasm32.release.wasm
zig build -Dtarget=wasm32-emscripten -Doptimize=Debug         # libcolyseus_gobu.web.wasm32.debug.wasm
```

The libraries go to the plugin's folder, `../colyseus/bin`, which `plugin.json` lists. The sources also build
for Linux and macOS (`-Dtarget=x86_64-linux`, `aarch64-macos`).

## The server

`server/` is what a game with **Project Settings > Online > Session > Provider: colyseus** needs: a Colyseus
server with `GobuRelayRoom`, registered as `gobu_relay` and matched by Session > Mode (`npm install`,
`npm start`; port 2567). See `server/README.md`.

## The native classes

What `colyseus.gro` wraps; a game uses the Grost module `Colyseus`.

`ColyseusClient`: `connect_to(endpoint)`, `join_or_create(room_name, options = {})`,
`create_room(room_name, options = {})`, `join(room_name, options = {})`, `join_by_id(room_id, options = {})`,
`reconnect(reconnection_token)` (a seat the server holds with `allowReconnection()`), `get_endpoint()`.

`ColyseusRoom`: `send_message(type: String, data: PackedByteArray)`, `send_message_int(type: int, data)`,
`leave(consented = true)`, `get_id()`, `get_session_id()`, `get_reconnection_token()`, `get_name()`,
`is_connected()`. Signals: `joined()`, `state_changed()`, `message_received(data)`, `error(code, message)`,
`left(code, reason)`.

## License

The Colyseus Native SDK: MIT, see `LICENSE`. Changes of the binding: `gobu/CHANGELOG.md`.
