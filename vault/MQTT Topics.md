# MQTT Topics

The command/state topic tree used by this device. All topics share the prefix `home/light/<name>/` where `<name>` is the per-unit slug (currently the literal placeholder `CHANGEME` in [[main.cpp]], and `bedroom_fairy` in `publish_discovery.sh`).

## Layout

| Direction | Topic | Payload | Notes |
|---|---|---|---|
| ← in | `home/light/<name>/power/set` | `on` / `OFF` | Case matters — `effectLoop` compares `power == "OFF"` exactly. Anything non-`OFF` is treated as on. |
| ← in | `home/light/<name>/brightness/set` | `0`–`100` (percent, as string) | Converted to 0–255 via `round(255 * pct/100)`. |
| ← in | `home/light/<name>/rgb/set` | `"r,g,b"` (0–255 each, comma-separated) | Parsed by `indexOf`/`lastIndexOf` — no whitespace tolerance. |
| ← in | `home/light/<name>/effect/set` | one of the [[Patterns]] names | `static`, `rainbow`, `fireflies`, `aurora`, `auroraSettable`, `torchlight`. |
| → out | `home/light/<name>/power/status` | echo of current `power` | Published on set and on MQTT reconnect (`init_mqtt`). |
| → out | `home/light/<name>/brightness/status` | percent (0–100) | Not raw 0–255. |
| → out | `home/light/<name>/rgb/status` | `"r,g,b"` |
| → out | `home/light/<name>/effect/status` | current effect name |

The HA discovery config (see [[Home Assistant Discovery]]) tells HA to use these same topics, plus `brightness_scale: 100` so it publishes percentages.

## Renaming a unit
Every occurrence of `CHANGEME` in [[main.cpp]] must be replaced (8 topic strings) **and** the corresponding topics in `src/publish_discovery.sh` must match, **and** HA discovery `object_id` in `homeassistant/light/<object_id>/config` should be updated too, or you'll strand a duplicate device in HA. The retained discovery payload can be cleared with:

```bash
mqtt pub -h <broker-ip> -t 'homeassistant/light/<old_object_id>/config' -m '' -r
```

## Gotchas
- Payload parsing in `onCommand` uses `String((char*) payload).substring(0, length)`. If `payload` isn't NUL-terminated (PubSubClient does not guarantee it), the `String(char*)` constructor may read past the end before the substring trims. Works in practice for the payloads we send; be wary if you ever accept binary.
- Case sensitivity in `power` (`OFF` vs `off`) is a foot-gun — HA usually sends uppercase by default. Verify with `mqtt sub` before troubleshooting.

## Related
- [[MQTT]] · [[main.cpp]] · [[Home Assistant Discovery]] · [[Patterns]]
