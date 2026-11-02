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
of the engine's), and nothing else: every engine value it holds is a `GobuVariant`, and its classes'
methods take Variant arguments (`gobu/src/gobu_colyseus.h`). The entry point is `gobu_plugin_init`
(`gobu/src/plugin.c`), which registers the classes and the `colyseus` provider of Online
(`gobu/src/colyseus_provider.c`). The Grost side is `../colyseus/colyseus.gro`.

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

Android (arm64-v8a, the ABI of Gobu's Android templates) needs the NDK in `ANDROID_NDK_HOME` (the one `forja`
puts in Gobu's `Engine/Extras/ThirdPartyTools/android-sdk/ndk/<version>`):

```bash
ANDROID_NDK_HOME=<ndk> zig build -Dtarget=aarch64-linux-android -Doptimize=ReleaseFast   # libcolyseus_gobu.android.arm64.release.so
ANDROID_NDK_HOME=<ndk> zig build -Dtarget=aarch64-linux-android -Doptimize=Debug         # libcolyseus_gobu.android.arm64.debug.so
```

The release goes without its debug sections (the NDK's `llvm-strip`). The libraries go to the plugin's folder,
`../colyseus/bin`, which `plugin.json` lists. The sources also build for Linux and macOS
(`-Dtarget=x86_64-linux`, `aarch64-macos`), which Gobu has no export templates for yet.

## The server

`server/` is what a game with **Project Settings > Online > Session > Provider: colyseus** needs: a Colyseus
server with `GobuRelayRoom`, registered as `gobu_relay` and matched by Session > Mode (`npm install`,
`npm start`; port 2567). See `server/README.md`.

## The native classes

What `colyseus.gro` wraps; a game uses the Grost module `Colyseus`.

`_ColyseusClient`: `set_endpoint(endpoint)`, `get_endpoint()`; `join_or_create(room_name, options_json = "{}")`,
`create(room_name, options_json)`, `join(room_name, options_json)`, `join_by_id(room_id, options_json)` and
`reconnect(reconnection_token)` (a seat the server holds with `allowReconnection()`) return a
`_ColyseusRoom` at once, which joins later. `http_get(path)`, `http_post(path, body)`, `http_put`,
`http_delete`, `http_patch` and `auth_request(op, email, password, options_json)` return a request id, answered
by the signals `_http_response(request_id, status_code, body)` and `_http_error(request_id, code, message)`.
`auth_set_token(token)`, `auth_get_token()`, `auth_signout()`. Static `poll()`: runs the network and emits
the signals, on the main thread (`colyseus.gro` calls it every frame).

`_ColyseusRoom`: `send_message(type: String, data)`, `send_message_int(type: int, data)` (`data`: any value,
sent as msgpack), `leave()`, `get_id()`, `get_session_id()`, `get_reconnection_token()`, `get_name()`,
`is_connected()`, `is_reconnecting()`, `set_reconnection_options(options: Dictionary)`, `get_state()` (a
Dictionary) and `set_state_type(type)` (a schema of `Colyseus.schema()`). Signals: `joined()`,
`state_changed()`, `reconnected()`, `message_received(type, data)`, `error(code, message)`,
`left(code, reason)`, `dropped(code, reason)`.

`_ColyseusCallbacks`: static `get(room)`; `listen`, `on_add`, `on_remove`, `on_change` (a property of the
state, or of a nested schema given as its Dictionary) return a handle for `remove(handle)`.

## License

The Colyseus Native SDK: MIT, see `LICENSE`. Changes of the binding: `gobu/CHANGELOG.md`.
