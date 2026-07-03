# MQTT Topics

The command/state topic tree. As of 2026-07-03 this uses Home Assistant's **JSON
light schema**: a single command topic and a single state topic carrying JSON,
plus an availability topic. All derive from one `DEVICE_SLUG` in [[Secrets]] —
`home/light/<slug>/…`. (Was: four `_command` + four `_status` topics with hand-rolled
string parsing.)

## Layout

| Direction | Topic | Payload |
|---|---|---|
| ← in | `home/light/<slug>/set` | JSON, any subset of the fields below |
| → out | `home/light/<slug>/state` | JSON, full current state (retained) |
| → out | `home/light/<slug>/status` | `online` / `offline` (retained LWT) |
| discovery | `homeassistant/light/<slug>/config` | retained discovery JSON — see [[Home Assistant Discovery]] |

## JSON payload shape

```json
{"state":"ON","brightness":200,"color_mode":"rgb",
 "color":{"r":100,"g":0,"b":100},"effect":"fireflies"}
```

- `state`: `"ON"` / `"OFF"` (drives the `isOn` bool; case from HA is uppercase).
- `brightness`: **0–255** (JSON-schema default scale — the old 0–100 percent
  conversion is gone).
- `color.{r,g,b}`: 0–255 each. Parsed with [[Libraries|ArduinoJson]]; no more manual
  `indexOf`/`substring`. Setting color also recomputes the `hue` global (fixes the
  old [[RGB to Hue gotcha]]).
- `effect`: one of the [[Patterns]] names.

Command messages are partial (HA sends only what changed); `onCommand` applies each
present key then echoes full state on `.../state`.

## Renaming a unit
Change `DEVICE_SLUG` (and `DEVICE_NAME`) in [[Secrets]] — **one place**. Every topic
and the discovery `object_id`/`unique_id` follow automatically. The device re-publishes
its own discovery on connect, so there's no separate script to keep in sync anymore.
To clear a stale retained entity from an old slug:

```bash
mqtt pub -h <broker-ip> -t 'homeassistant/light/<old_slug>/config' -m '' -r
```

## Related
- [[MQTT]] · [[main.cpp]] · [[Home Assistant Discovery]] · [[Patterns]] · [[Secrets]] · [[RGB to Hue gotcha]]
