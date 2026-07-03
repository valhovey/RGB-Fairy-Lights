# main.cpp

The entire firmware. Lives at `src/main.cpp`. Arduino-framework C++ compiled via [[PlatformIO]].

## Structure, top to bottom

1. **Pin/count/build-mode macros** — `LED_PIN A0`, `LED_COUNT 100`, and `NET_ENABLED` (comment out to build a WiFi/MQTT-less firmware — added in commit `175ea73`).
2. **Network globals** (inside `#ifdef NET_ENABLED`) — `ssid`, `password`, `mqtt_server`, plus the four command and four status [[MQTT Topics|topic strings]] with a shared `home/light/<name>/…` prefix. `WiFiClient` and `PubSubClient` instances. Also declares a `gateway`/`subnet` pair even though DHCP is used; see [[WiFi Config]].
3. **`Adafruit_NeoPixel strip(…, NEO_RGB + NEO_KHZ800)`** — note the `NEO_RGB` byte order, not the more common `NEO_GRB`. This is because the [[Neopixel Fairy Lights]] strand uses RGB order. See [[Libraries]].
4. **`getHue(r,g,b)`** — RGB→hue helper adapted from Stack Overflow. Currently declared but effectively unused by the working effects; hue is computed live per-LED. Related: [[RGB to Hue gotcha]].
5. **Runtime state globals** — `r`, `g`, `b`, `hue`, `brightness`, `power`, `effect`. These are mutated by MQTT callbacks and read by `effectLoop`.
6. **MQTT plumbing** (`#ifdef NET_ENABLED`) — `reconnect()`, four `publishX()` state-echoers, `onCommand()` router keyed on the incoming topic, and `init_mqtt()` which subscribes + pushes initial state + registers the callback.
7. **Firefly math** — `randomPhases[100]` (baked at build time, JS one-liner in the comment), `beegHueFromSmol` (0–360 → 0–65535 for `ColorHSV`), `fireflyActivation`, `fireflyHue`, `fireflyBrightness`, `auroraHue`. See [[Patterns]] for what each does.
8. **`init_strip()`** — `strip.begin()` + a first `strip.show()` + a default `setBrightness(50)`.
9. **`setup()`** — `Serial.begin(112500)` (unusual baud — not a typo, that's what's in the source), WiFi join with a `waitForConnectResult` bail, blocking MQTT connect, then `init_strip()` + `init_mqtt()`.
10. **`MAXVAL` + `effectLoop()`** — the animation dispatch. Uses `millis() % MAXVAL` where `MAXVAL = numeric_limits<unsigned long>::max() / 2` to sidestep an `unsigned long`→`float` rollover that used to break animations at long uptimes (commit `1fc9452`, "Hopefully fixed overflow error", and `175ea73`).
11. **`loop()`** — MQTT reconnect + `client.loop()` (inside `NET_ENABLED`), then `effectLoop()`.

## Config knobs at the top of the file
| Symbol | What it does | Change when |
|---|---|---|
| `LED_PIN` | Data pin driving the strand. `A0` on both QT Py boards. | Rewiring |
| `LED_COUNT` | Number of LEDs. Must match `randomPhases[]` length for per-pixel effects. | Different strand length |
| `NET_ENABLED` | Compile WiFi + MQTT in/out. | Bench testing or standalone build |
| `ssid` / `password` | WiFi credentials, plaintext. | Per-network. See [[WiFi Config]]. |
| `mqtt_server` | Broker IP. | Per-install. See [[MQTT]]. |
| `mqtt_*_topic` | Command/state topics — replace `CHANGEME` with the unit name (e.g. `bedroom_fairy`). See [[MQTT Topics]]. |
| `clientId` in `reconnect()` | MQTT client id string. Currently hard-coded to `"Hannah Fairy Lights"`. Change per unit or brokers will see collisions. |

## Gotchas
- `Serial.begin(112500)` — the odd baud is intentional in-source. Set your serial monitor to match, or comment it out.
- `strip.setBrightness(brightness)` is called every frame inside `effectLoop`. The `Adafruit_NeoPixel` docs warn that repeated `setBrightness` calls are lossy; not a bug today but worth remembering if colours drift.
- The `auroraSettable` effect has a shadow-variable bug and the RGB-input plumbing isn't finished — see [[RGB to Hue gotcha]].
- `reconnect()` is a blocking `while` — if the broker is down at boot, the strip won't animate until MQTT comes up. Symptom: strand stays whatever it was pre-reset.
- The MQTT `clientId` collides across units unless changed per install; watch for kicked connections if you ever run two boards on one broker.

## Related
- [[RGB Fairy Lights]] · [[Patterns]] · [[MQTT Topics]] · [[Libraries]] · [[Payload size gotcha]] · [[RGB to Hue gotcha]]
