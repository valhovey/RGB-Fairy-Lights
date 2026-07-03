# Payload size gotcha

Why [[Home Assistant Discovery]] is a shell script instead of a `client.publish()` call inside [[main.cpp]].

## The constraint
- [[Libraries|PubSubClient]]'s default `MQTT_MAX_PACKET_SIZE` is **256 bytes** — inclusive of the fixed header, topic string, and payload. Anything larger silently fails: `client.publish()` returns `false`, the broker never sees the message.
- The HA MQTT-light discovery payload for this device (see the JSON in `src/publish_discovery.sh`) is **~900 bytes** of JSON, plus the `homeassistant/light/bedroom_fairy/config` topic string. Nowhere close to fitting in 256.

## Two fixes if we ever want to publish from-device
1. **Bump the buffer at runtime**: `client.setBufferSize(1024)` (available since PubSubClient v2.7 — we're on the git tip, so it's there). Costs ~800 extra bytes of RAM; on ESP32 that's fine. Publish discovery once from `setup()`, then reset the buffer if desired.
2. **Switch to HA's [JSON schema light](https://www.home-assistant.io/integrations/light.mqtt/#json-schema)** — one `command_topic` receiving a single JSON object instead of the four separate `_command_topic`s used today. Shrinks the discovery payload roughly in half **and** halves the number of topic subscriptions in [[main.cpp]]. Bigger refactor but a cleaner endpoint.

Either fix would obsolete `src/publish_discovery.sh` and simplify install-per-unit setup (see [[MQTT Topics]] renaming pain).

## Status
- Neither fix applied yet — the current install flow still relies on the shell script.
- No urgency: the script works and only runs once per unit.

## Related
- [[Home Assistant Discovery]] · [[Libraries]] · [[MQTT]] · [[MQTT Topics]]
