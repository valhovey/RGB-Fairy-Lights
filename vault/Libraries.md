# Libraries

Runtime dependencies declared in `platformio.ini` under `lib_deps` for both envs:

| Library | Source | Purpose |
|---|---|---|
| `Adafruit_NeoPixel` | `git@github.com:adafruit/Adafruit_NeoPixel.git` (SSH) | Driving the [[Neopixel Fairy Lights]] strand. Used everywhere in `effectLoop`. |
| `Adafruit_PCT2075` | `https://github.com/adafruit/Adafruit_PCT2075.git` | Temperature sensor driver. **Not actually used in the current firmware** — no `#include` of it. Leftover from an earlier iteration or copy-paste from an Adafruit example. Safe to drop from `lib_deps` if trimming build time. |
| `Adafruit_BusIO` | `https://github.com/adafruit/Adafruit_BusIO.git` | Transitive dep of the PCT2075 driver. Same story — not needed once PCT2075 is dropped. |
| `PubSubClient` | `https://github.com/knolleary/pubsubclient.git` | MQTT client. Trips over the [[Payload size gotcha]] — max packet is small by default. |
| `SPI` | (Arduino core) | Same story as `Wire`: only needed by the unused BusIO chain. |
| `Wire` | (Arduino core) | I2C. Only needed by BusIO/PCT2075. |

## Framework
- `framework = arduino`
- `platform = espressif32` (both envs) — see [[QT Py ESP32]] / [[QT Py ESP32-S3]] for the specific board packages that get pulled in.

## Trim opportunity
`Adafruit_PCT2075`, `Adafruit_BusIO`, `SPI`, and `Wire` can all be removed together — none of them are referenced from [[main.cpp]]. Keep them if you plan to add temperature-based effects; drop them for smaller flash and faster builds otherwise.

## Related
- [[main.cpp]] · [[PlatformIO]] · [[Payload size gotcha]]
