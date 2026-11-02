# Server

A [Colyseus](https://docs.colyseus.io/) server for Gobu's online games with the `colyseus` provider
(Project Settings > Online > Session > Provider): `GobuRelayRoom` (`src/rooms/GobuRelayRoom.ts`), registered as
`gobu_relay` and matched by Session > Mode. The first game in a room is the host; the room passes the games'
packets along, and makes another player the host when the host leaves.

```
npm install
npm start          # port 2567, or the PORT environment variable
```

`npm run build` compiles it to `build/` for a deployment (`node build/index.js`).
