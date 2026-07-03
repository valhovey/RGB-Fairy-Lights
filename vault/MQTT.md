# MQTT

Command/state bus for this project. Broker lives on the user's LAN; the ESP32 connects as a client using [[Libraries|PubSubClient]] over plain TCP on port 1883 — no TLS, no auth.

## Configuration (in `main.cpp`)
- Broker IP: `mqtt_server` (currently `192.168.100.250`; was `192.168.100.76` on `main` — the working-tree diff shows the user has moved brokers).
- Port: `1883` (hard-coded in `setup()` via `client.setServer(mqtt_server, 1883)`).
- Client id: `"Hannah Fairy Lights"` — hard-coded in `reconnect()`. See gotcha below.
- No username/password; the broker is trusted-LAN-only.

Topic tree lives in [[MQTT Topics]]. The initial HA teach-in is a separate manual step — see [[Home Assistant Discovery]].

## Connection lifecycle
1. WiFi comes up in `setup()` — see [[WiFi Config]].
2. `client.setServer(mqtt_server, 1883)`.
3. `reconnect()` loops with `delay(1000)` until `client.connect(clientId)` returns true. Blocking — nothing else runs meanwhile.
4. `init_mqtt()` subscribes to the four command topics, immediately publishes current state on the four status topics, and registers `onCommand` as the callback.
5. Main `loop()` re-invokes `reconnect()` if the client drops, then calls `client.loop()` every iteration to drive keepalives + inbound message dispatch.

## Gotchas
- **PubSubClient's default max packet size is small** (256 bytes in stock versions). That's why HA discovery is published from a shell script, not the ESP32. See [[Payload size gotcha]].
- **Shared `clientId` across units** — if you ever run two boards on one broker, one will kick the other off in a loop. Change the string per unit.
- **Blocking reconnect at boot** — the strand won't animate until MQTT connects. If the broker is unreachable, the light appears bricked; check serial output.
- No last-will / retained-state — status topics are published only when the device is online. HA will show the entity as `unknown` until the ESP32 reconnects.

## Live smoke-test commands
Requires [`mqtt-cli`](https://www.npmjs.com/package/mqtt-cli) (used by `publish_discovery.sh` too).

```bash
mqtt sub -h 192.168.100.250 -t 'home/light/#' -v          # watch traffic
mqtt pub -h 192.168.100.250 -t home/light/bedroom_fairy/effect/set -m 'rainbow'
mqtt pub -h 192.168.100.250 -t home/light/bedroom_fairy/power/set   -m 'OFF'
```

Publishing to a `.../set` topic will change the strand's state — coordinate with anyone using the light. See the "Actions that touch hardware or shared systems" note in the [[build]] skill.

## Related
- [[MQTT Topics]] · [[Home Assistant Discovery]] · [[WiFi Config]] · [[Payload size gotcha]] · [[main.cpp]]
