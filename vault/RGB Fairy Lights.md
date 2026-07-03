# RGB Fairy Lights

A single-strand Neopixel driver built on an Adafruit QT Py ESP32 / ESP32-S3, powering a strand of [[Neopixel Fairy Lights]] and taking commands from [[Home Assistant]] over [[MQTT]]. Firmware is a single-file Arduino sketch built with [[PlatformIO]]; the finished driver box is enclosed in a printed [[Case]] and powered from a [[Power Supply|5 V barrel jack]].

The whole thing is one `main.cpp` in `src/`, plus a `publish_discovery.sh` used once per install to teach Home Assistant about the device. See [[main.cpp]] for the sketch layout, [[Patterns]] for the LED animation modes, and [[MQTT Topics]] for the command/state topic tree.

## Current state
- Two build envs in `platformio.ini`: `adafruit_qtpy_esp32` (default) and `adafruit_qtpy_esp32s3`. See [[PlatformIO]].
- 100 LEDs on data pin `A0`, hard-coded via `LED_PIN`/`LED_COUNT`.
- Network stack is opt-in via `#define NET_ENABLED` at the top of [[main.cpp]] — commenting it out builds a purely local, MQTT-less firmware.
- WiFi + MQTT credentials and the topic prefix (`home/light/<name>/…`) are hard-coded in-source; each unit is customised by editing [[main.cpp]] before flashing. See [[WiFi Config]] and [[MQTT Topics]].
- HA integration is a manual, one-shot `publish_discovery.sh` — the payload lives outside the firmware because of a [[Payload size gotcha]] in PubSubClient.
- Five working effects (`static`, `rainbow`, `fireflies`, `aurora`, `torchlight`) plus one broken one (`auroraSettable`) — see [[Patterns]] and [[RGB to Hue gotcha]].

## Related
- Hardware: [[QT Py ESP32-S3]], [[QT Py ESP32]], [[Wiring]], [[Case]], [[Power Supply]]
- Firmware: [[main.cpp]], [[PlatformIO]], [[Libraries]], [[Patterns]]
- Network: [[WiFi Config]], [[MQTT]], [[MQTT Topics]], [[Home Assistant Discovery]]
- Gotchas: [[Payload size gotcha]], [[RGB to Hue gotcha]]
