# Libraries

Runtime dependencies declared in `platformio.ini` under `lib_deps` for both envs
(as of 2026-07-03):

| Library | Source | Purpose |
|---|---|---|
| `Adafruit_NeoPixel` | `git@github.com:adafruit/Adafruit_NeoPixel.git` (SSH) | Drives the [[Neopixel Fairy Lights]] strand. Used throughout `effectLoop`. Note `NEO_RGB` byte order. |
| `PubSubClient` | `https://github.com/knolleary/pubsubclient.git` | MQTT client — see [[MQTT]]. Buffer bumped to 1024 in-code (retired the [[Payload size gotcha]]). |
| `ArduinoJson` | `bblanchon/ArduinoJson@^7` (PlatformIO registry) | Builds/parses the HA JSON discovery + command/state payloads — see [[MQTT Topics]] and [[Home Assistant Discovery]]. Added in the 2026-07-03 refactor. |

## Dropped in the refactor
`Adafruit_PCT2075`, `Adafruit_BusIO`, `SPI`, and `Wire` were removed from `lib_deps`
— none were ever `#include`d ([[main.cpp]] has no I2C/temperature code). Smaller flash,
faster builds. Re-add them together only if a temperature-driven effect is ever added.

## ArduinoJson v7 notes
- `JsonDocument doc;` is elastic (no capacity template). Arrays via
  `doc["k"].to<JsonArray>()`, nested objects via `.to<JsonObject>()`.
- `deserializeJson(doc, payload, length)` parses the raw MQTT bytes directly.
- The `doc["k"] | fallback` operator is used to default missing color channels.

## Framework
- `framework = arduino`, `platform = espressif32` (both envs). Board packages: see
  [[QT Py ESP32]] / [[QT Py ESP32-S3]]. The `esp_task_wdt` [[Watchdog]] is part of the
  ESP32 core (no extra dep).

## Related
- [[main.cpp]] · [[PlatformIO]] · [[MQTT]] · [[Payload size gotcha]] · [[Home Assistant Discovery]]
