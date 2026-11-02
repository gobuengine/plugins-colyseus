import { defineServer, defineRoom } from "colyseus";

import { GobuRelayRoom } from "./rooms/GobuRelayRoom";

/**
 * A Colyseus server for Gobu's online games (Project Settings > Online > Session > Provider: colyseus): one
 * GobuRelayRoom per game, matched by Session > Mode. A game that uses Colyseus itself (its own rooms, schemas
 * and messages, through the Grost SDK) adds its rooms here: https://docs.colyseus.io/
 */
export const server = defineServer({
    rooms: {
        gobu_relay: defineRoom(GobuRelayRoom).filterBy(["mode"]),
    },
});

export default server;
