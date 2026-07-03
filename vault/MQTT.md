# MQTT

Command/state bus for this project. Broker lives on the user's LAN; the ESP32
connects as a client using [[Libraries|PubSubClient]] over plain TCP. As of the
2026-07-03 refactor the light speaks Home Assistant's **JSON light schema** — one
command topic and one state topic carrying JSON, instead of the old eight separate
topics. See [[MQTT Topics]].

## Configuration (from [[Secrets]], via `main.cpp`)
- Broker: `MQTT_SERVER` / `MQTT_PORT` (defaults to `1883`).
- **Auth**: `client.connect(DEVICE_SLUG, MQTT_USER, MQTT_PASS, willTopic, 0, true, "offline")`.
  Empty `MQTT_USER` → `nullptr` → anonymous. Credentials live in [[Secrets]].
- Client id: `DEVICE_SLUG` (per-unit, no longer the shared `"Hannah Fairy Lights"`).
- Buffer: `client.setBufferSize(1024)` so the ~700-byte discovery JSON fits — this
  retired the old [[Payload size gotcha]] workaround.
- Keepalive: `client.setKeepAlive(60)`.

## Connection lifecycle (non-blocking)
1. WiFi comes up in `setup()` (bounded 10 s wait, then proceeds) — see [[WiFi Config]].
2. `client.setServer/​setBufferSize/​setKeepAlive/​setCallback(onCommand)` in `setup()`.
   MQTT is **not** connected in `setup()` — the strand animates immediately.
3. In `loop()`, `ensureConnected()` runs every iteration: if WiFi or MQTT is down it
   retries **at most once per 5 s** and returns fast, so `effectLoop()` never blocks.
   Then `client.loop()` drives keepalives + inbound dispatch.
4. On every successful (re)connect, `onMqttConnected()` **re-subscribes** to the
   command topic, publishes `"online"` availability, re-publishes discovery, and
   publishes current state.

## The freeze fix (why this was rewritten)
The old `reconnect()` was a blocking `while (!connected) { delay(1000); }` with **no
WiFi recovery**. When WiFi dropped after ~half a day, MQTT could never reconnect, so
the loop spun forever — animation frozen, commands ignored. It also never
re-subscribed after a reconnect. The rewrite makes connection management non-blocking,
adds WiFi auto-reconnect, re-subscribes on every connect, and adds a [[Watchdog]]
reboot backstop. See [[plans/2026-07-03-mqtt-discovery-refactor]].

## Availability / Last Will
- Retained LWT publishes `"offline"` to `home/light/<slug>/status` if the device
  drops; `"online"` is published on connect. HA shows the entity unavailable instead
  of stale. Wired into discovery as `availability_topic`.

## Live smoke-test commands
Requires [`mqtt-cli`](https://www.npmjs.com/package/mqtt-cli).

```bash
mqtt sub -h <broker-ip> -t 'home/light/#' -v
mqtt pub -h <broker-ip> -t home/light/<slug>/set -m '{"state":"ON","effect":"rainbow"}'
mqtt pub -h <broker-ip> -t home/light/<slug>/set -m '{"state":"OFF"}'
```

Publishing to `.../set` changes the strand's state — coordinate with anyone using the
light (see the [[build]] skill's hardware-actions note).

## Related
- [[MQTT Topics]] · [[Home Assistant Discovery]] · [[Secrets]] · [[WiFi Config]] · [[Watchdog]] · [[Libraries]] · [[main.cpp]]
