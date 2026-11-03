# Colyseus

Colyseus SDK v0.18.0 as a Gobu plugin, and the `colyseus` provider of Gobu's Online.

Install it from Gobu's **Edit > Plugins** (it downloads the latest release of this repository into
`<project>/plugins/colyseus` and enables it), or copy the `colyseus` folder there by hand. Guide:
https://gobuengine.org/plugins/02-colyseus

A release has the three files of `forja package plugin <this repository>/colyseus` (run from Gobu):
`colyseus.zip`, `plugin.json` (with the package's size and SHA-256) and `icon.svg`, the image the Plugins
window shows.

Layout (the standard of every plugin in `ProjectGobu/Plugins/<Name>`):

- `colyseus/`: what a project gets (`plugin.json`, `colyseus.gro`, the built libraries in `bin/`).
- `Source/`: the code: the Colyseus Native SDK, its Gobu binding (`Source/gobu`, built with Zig into
  `colyseus/bin`) and the server the online games use (`Source/server`). See `Source/README.md`.
