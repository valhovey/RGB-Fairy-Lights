# main.cpp

The entire firmware. Lives at `src/main.cpp`. Arduino-framework C++ compiled via
[[PlatformIO]]. Substantially refactored 2026-07-03 — see
[[plans/2026-07-03-mqtt-discovery-refactor]].

## Structure, top to bottom
1. **Macros** — `LED_PIN A0`, `LED_COUNT 100`, `NET_ENABLED` (comment out for a
   WiFi/MQTT-less build), `WDT_TIMEOUT_S 30` ([[Watchdog]]).
2. **Network globals** (`#ifdef NET_ENABLED`) — `#include "secrets.h"` ([[Secrets]]),
   `WiFiClient` + `PubSubClient`, and the four slug-derived topic Strings
   (`mqtt_command_topic`, `mqtt_state_topic`, `mqtt_availability_topic`,
   `mqtt_discovery_topic`) plus the `EFFECTS[]` list. Everything derives from
   `DEVICE_SLUG`. See [[MQTT Topics]].
3. **`Adafruit_NeoPixel strip(…, NEO_RGB + NEO_KHZ800)`** — `NEO_RGB` byte order (the
   strand isn't the usual `NEO_GRB`). See [[Libraries]].
4. **`getHue(r,g,b)`** — RGB→hue helper; now called from `onCommand` on color changes
   so [[RGB to Hue gotcha|auroraSettable]] tracks color.
5. **Runtime state globals** — `r`, `g`, `b`, `hue`, `brightness`, `isOn` (bool,
   replaced the `power` string), `effect`.
6. **MQTT plumbing** (`#ifdef NET_ENABLED`) — `publishState()` and `publishDiscovery()`
   (both build JSON via [[Libraries|ArduinoJson]]), `onCommand()` (JSON parse),
   `onMqttConnected()` (re-subscribe + announce), `connectMqtt()` (auth + LWT), and the
   non-blocking `ensureConnected()`.
7. **Effect math** — `randomPhases[100]`, `beegHueFromSmol`, `fireflyActivation`,
   `fireflyHue`, `fireflyBrightness`, `auroraHue`. See [[Patterns]].
8. **`init_strip()`**, **`initWatchdog()`** (version-guarded esp_task_wdt setup).
9. **`setup()`** — `Serial.begin(115200)`, strip init, WiFi bring-up (bounded 10 s,
   then proceeds), MQTT client config (`setBufferSize(1024)`, `setKeepAlive(60)`,
   `setCallback`), watchdog init. Does **not** block on MQTT.
10. **`effectLoop()`** — per-effect `millis() % <period>` phase clock (no more
    `MAXVAL`), dispatch on `effect`, `!isOn` fills black.
11. **`loop()`** — `ensureConnected()` + `client.loop()` (inside `NET_ENABLED`),
    `effectLoop()`, `esp_task_wdt_reset()`.

## Config knobs
| Symbol | What it does | Change when |
|---|---|---|
| `LED_PIN` | Data pin. `A0` on both QT Py boards. | Rewiring |
| `LED_COUNT` | LED count; must match `randomPhases[]` length. | Different strand |
| `NET_ENABLED` | Compile WiFi + MQTT in/out. | Standalone build |
| `WDT_TIMEOUT_S` | [[Watchdog]] reboot timeout. | Rarely |
| `DEVICE_SLUG` / `DEVICE_NAME` etc. | In [[Secrets]], not here. | Per unit/network |

## Gotchas
- `strip.setBrightness(brightness)` runs every frame — `Adafruit_NeoPixel` warns
  repeated `setBrightness` is lossy; not a bug today but watch for colour drift.
- Credentials are **not** in this file anymore — see [[Secrets]] / [[WiFi Config]].
- The connection path is non-blocking now; the [[Watchdog]] reboots if the loop ever
  stalls anyway.

## Related
- [[RGB Fairy Lights]] · [[Patterns]] · [[MQTT]] · [[MQTT Topics]] · [[Secrets]] · [[Libraries]] · [[Watchdog]] · [[RGB to Hue gotcha]]
