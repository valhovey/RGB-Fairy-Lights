# Secrets

All per-install credentials and identity live in `include/secrets.h`, a **gitignored**
header. A committed template `include/secrets.example.h` documents the keys. This
replaced the old pattern of hard-coding WiFi/broker values in [[main.cpp]].

## Keys

| Macro | Purpose |
|---|---|
| `WIFI_SSID` / `WIFI_PASS` | WiFi join — see [[WiFi Config]]. |
| `MQTT_SERVER` / `MQTT_PORT` | Broker address — see [[MQTT]]. |
| `MQTT_USER` / `MQTT_PASS` | MQTT auth. Leave `""` for anonymous; `main.cpp` passes `nullptr` when empty. |
| `DEVICE_SLUG` | The per-unit slug (e.g. `bedroom_fairy`). **Every** MQTT topic and the HA discovery `object_id` derive from this one value — see [[MQTT Topics]]. |
| `DEVICE_NAME` | Display name in Home Assistant (e.g. `Bedroom Fairy Lights`). |

## Setup for a new checkout / new unit

```bash
cp include/secrets.example.h include/secrets.h
# then edit include/secrets.h with real values
```

`include/` is on PlatformIO's include path automatically, so `#include "secrets.h"`
just works. `include/secrets.h` is listed in `.gitignore` and must never be committed.

## Gotchas
- Renaming a unit is now a **one-line** change (`DEVICE_SLUG`) — the old pain of
  editing 8 topic strings + a shell script + a discovery id is gone (see [[MQTT Topics]]).
- Never copy these values into the vault (see the [[build]] skill's rules). The real
  SSID was previously leaked into [[WiFi Config]] and has since been scrubbed.

## Related
- [[WiFi Config]] · [[MQTT]] · [[MQTT Topics]] · [[main.cpp]] · [[Home Assistant Discovery]]
