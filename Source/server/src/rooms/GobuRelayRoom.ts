import { Room, Client } from "@colyseus/core";

/**
 * A Gobu online game (Online.quick_match() with the "colyseus" provider): Colyseus finds a room of the same
 * mode with room, or makes one, and the room passes the games' packets along. The first game to join is the
 * host (peer 1, it runs the world); the others get ids from 2. When the host leaves, the player with the lowest
 * id becomes the host (peer 1) and the game goes on; with migration off (Session > Host Migration), it ends.
 *
 * Bytes messages, each starting with a peer id (int32, little endian):
 * - "s" from a game: [to][packet], `to` a peer or 0 for every other one.
 * - "p" to a game: [from][packet].
 * - "w" to a game that joined: [its id].
 * - "j" / "l" to the host: [the peer that joined / left].
 * - "h" from a game (empty): asks for its "w" again.
 * - "m" to every game, when the host left: [the previous id of the new host].
 * - "k" from the host: [a peer]: the room closes that game's connection (Online.kick(), a login it turned away),
 *   so a game that does not leave by itself goes too.
 */
export class GobuRelayRoom extends Room {
  // The close code of a game the host sent away.
  static readonly KICKED = 4003;

  private ids = new Map<string, number>();
  private games = new Map<number, Client>();
  private nextId = 2;
  private migration = true;

  onCreate(options: any) {
    this.maxClients = Math.max(2, Math.min(256, Number(options?.maxClients) || 8));
    this.setMetadata({ mode: String(options?.mode ?? "") });
    this.migration = options?.migration !== false;
    this.onMessageBytes("s", (client: Client, data: Uint8Array) => this.relay(client, data));
    // A game asks for its id again (its greeting may have come before it listened).
    this.onMessageBytes("h", (client: Client) => {
      const id = this.ids.get(client.sessionId);
      if (id !== undefined) {
        client.sendBytes("w", GobuRelayRoom.withId(id));
      }
    });
    // Only the host sends players away.
    this.onMessageBytes("k", (client: Client, data: Uint8Array) => {
      if (this.ids.get(client.sessionId) !== 1 || !(data instanceof Uint8Array) || data.length < 4) {
        return;
      }
      const bytes = new Uint8Array(data);
      const id = new DataView(bytes.buffer, bytes.byteOffset, 4).getInt32(0, true);
      if (id !== 1) {
        this.games.get(id)?.leave(GobuRelayRoom.KICKED);
      }
    });
  }

  private static withId(id: number, payload?: Uint8Array): Uint8Array {
    const message = new Uint8Array(4 + (payload?.length ?? 0));
    new DataView(message.buffer).setInt32(0, id, true);
    if (payload) {
      message.set(payload, 4);
    }
    return message;
  }

  private relay(client: Client, data: Uint8Array) {
    const from = this.ids.get(client.sessionId);
    if (from === undefined || !(data instanceof Uint8Array) || data.length < 4) {
      return;
    }
    const bytes = new Uint8Array(data);
    const to = new DataView(bytes.buffer, bytes.byteOffset, 4).getInt32(0, true);
    const message = GobuRelayRoom.withId(from, bytes.subarray(4));
    if (to === 0) {
      for (const [id, game] of this.games) {
        if (id !== from) {
          game.sendBytes("p", message);
        }
      }
    } else {
      this.games.get(to)?.sendBytes("p", message);
    }
  }

  onJoin(client: Client) {
    const id = this.games.size === 0 && !this.games.has(1) ? 1 : this.nextId++;
    this.ids.set(client.sessionId, id);
    this.games.set(id, client);
    client.sendBytes("w", GobuRelayRoom.withId(id));
    if (id !== 1) {
      this.games.get(1)?.sendBytes("j", GobuRelayRoom.withId(id));
    }
  }

  onLeave(client: Client) {
    const id = this.ids.get(client.sessionId);
    this.ids.delete(client.sessionId);
    if (id === undefined) {
      return;
    }
    this.games.delete(id);
    if (id === 1) {
      const next = Math.min(...this.games.keys());
      if (!this.migration || !Number.isFinite(next)) {
        // The host ran the world: without it (and nobody to take its place) there is no game.
        this.lock();
        this.disconnect(4000);
        return;
      }
      // The player with the lowest id is the host now: it becomes peer 1 for everyone.
      const host = this.games.get(next)!;
      this.games.delete(next);
      this.games.set(1, host);
      this.ids.set(host.sessionId, 1);
      for (const game of this.games.values()) {
        game.sendBytes("m", GobuRelayRoom.withId(next));
      }
      // The new host learns who plays (it only knew the old host).
      for (const other of this.games.keys()) {
        if (other !== 1) {
          host.sendBytes("j", GobuRelayRoom.withId(other));
        }
      }
      return;
    }
    this.games.get(1)?.sendBytes("l", GobuRelayRoom.withId(id));
  }
}
