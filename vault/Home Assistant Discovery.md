# Home Assistant Discovery

How the fairy lights show up as a light entity in [[Home Assistant]] without a hand-written YAML block. Done once per unit via a retained MQTT config payload published to `homeassistant/light/<object_id>/config`.

The device does **not** publish this itself. It's pushed by `src/publish_discovery.sh`, which uses the `mqtt` CLI from [`mqtt-cli`](https://www.npmjs.com/package/mqtt-cli). Reason: the payload is too big for the ESP32-side [[Libraries|PubSubClient]] with its stock buffer — see [[Payload size gotcha]].

## The script

`src/publish_discovery.sh` publishes (retained) one JSON blob to `homeassistant/light/bedroom_fairy/config`. Broker IP in the script is hard-coded (currently `192.168.100.76` — note that this is **the old broker IP**; the current firmware target is `192.168.100.250`. Update the script before running).

Fields in the payload:
- `name` — display name in HA (`Bedroom Fairy Lights`).
- `state_topic` / `command_topic` → `home/light/bedroom_fairy/power/…`
- `brightness_state_topic` / `brightness_command_topic` → `.../brightness/…`, with `brightness_scale: 100` so HA sends percentages.
- `rgb_state_topic` / `rgb_command_topic` → `.../rgb/…`
- `effect_command_topic` / `effect_state_topic` → `.../effect/…`
- `effect_list` — `["static","rainbow","fireflies","aurora","auroraSettable","torchlight"]`. Includes `auroraSettable`, which is currently broken — see [[Patterns]] and [[RGB to Hue gotcha]].
- `optimistic: false` — HA waits for the device to confirm state via the status topic before updating the UI.

All of these must match the topic strings in [[main.cpp]] — see [[MQTT Topics]] for how the `<name>` slug is threaded through both places.

## Running the script

```bash
# Before running: edit publish_discovery.sh to match your unit's slug and broker IP
bash src/publish_discovery.sh
```

The payload is **retained**, so it survives broker restarts and HA restarts, and it applies to any future discovery listener. To clear a stale entity, publish an empty retained message to the same topic:

```bash
mqtt pub -h <broker-ip> -t 'homeassistant/light/bedroom_fairy/config' -m '' -r
```

## Gotchas
- The script IP is not kept in sync with [[main.cpp]]. Double-check both before running.
- If you change the `<name>` slug in one place, you must update it in three: the topic strings in [[main.cpp]], every `home/light/<name>/…` occurrence in the script, and the `homeassistant/light/<object_id>/config` topic itself. A slug mismatch will silently create a duplicate HA entity that never receives state.
- Discovery is a one-shot manual step per install. If HA is factory-reset or the broker is wiped, re-run the script.
- Publishing the wrong retained payload sticks around forever until cleared — treat this script as a hardware-touching action per the [[build]] skill's guidance.

## Related
- [[MQTT Topics]] · [[MQTT]] · [[Payload size gotcha]] · [[Patterns]] · [[main.cpp]]
