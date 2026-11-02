# Colyseus

Colyseus SDK v0.18.0 as a Gobu plugin, and the `colyseus` provider of Gobu's Online.

Copy the `colyseus` folder into `<project>/plugins/` and enable it in Project Settings > Plugins. Guide:
https://gobuengine.org/plugins/02-colyseus

Layout (the standard of every plugin in `ProjectGobu/Plugins/<Name>`):

- `colyseus/`: what a project gets (`plugin.json`, `colyseus.gro`, the built libraries in `bin/`).
- `Source/`: the code: the Colyseus Native SDK, its Gobu binding (`Source/gobu`, built with Zig into
  `colyseus/bin`) and the server the online games use (`Source/server`). See `Source/README.md`.
