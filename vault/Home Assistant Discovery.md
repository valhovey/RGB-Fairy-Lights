# Home Assistant Discovery

How the fairy lights show up as a light entity in [[Home Assistant]] automatically.
As of 2026-07-03 the **device publishes its own discovery config** from firmware —
the old out-of-band `src/publish_discovery.sh` is gone.

## How it works now
- `publishDiscovery()` in [[main.cpp]] builds the config with [[Libraries|ArduinoJson]]
  and publishes it **retained** to `homeassistant/light/<slug>/config` on every MQTT
  (re)connect (inside `onMqttConnected()`).
- It fits because `client.setBufferSize(1024)` is set — this is what retired the
  [[Payload size gotcha]].
- Uses HA's **JSON schema** (`"schema":"json"`), so one command/state topic pair
  carries everything — see [[MQTT Topics]].

## Key fields
- `schema: "json"`, `name`: `DEVICE_NAME`, `unique_id`: `DEVICE_SLUG` (both from [[Secrets]]).
- `command_topic` / `state_topic` → `home/light/<slug>/set` · `/state`.
- `availability_topic` → `home/light/<slug>/status` with `payload_available: "online"`
  / `payload_not_available: "offline"` (retained LWT — see [[MQTT]]).
- `brightness: true`, `supported_color_modes: ["rgb"]`, `effect: true`,
  `effect_list`: the six [[Patterns]] (`auroraSettable` now works — see [[RGB to Hue gotcha]]).
- `device: {…}` block so HA groups it as a proper device.

## Why not ArduinoHA / a discovery library?
The closest Arduino analog to Python's `ha_mqtt_discoverable` is **ArduinoHA**
(`dawidchyrzynski/arduino-home-assistant`), but its `HALight` has **no `effect_list`
support** (open issue #142). Effects are core here, so we stayed on PubSubClient +
ArduinoJson + the JSON schema, which supports effects natively. See
[[plans/2026-07-03-mqtt-discovery-refactor]] for the full rationale.

## Gotchas
- Discovery is retained, so a wrong payload sticks until cleared. Clear a stale slug:
  `mqtt pub -h <broker-ip> -t 'homeassistant/light/<old_slug>/config' -m '' -r`.
- The device re-announces on every reconnect, so a broker/HA wipe self-heals on the
  next connect — no manual re-run needed anymore.

## Related
- [[MQTT Topics]] · [[MQTT]] · [[Payload size gotcha]] · [[Patterns]] · [[Secrets]] · [[main.cpp]]
