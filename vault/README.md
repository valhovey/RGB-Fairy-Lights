# Vault — RGB Fairy Lights

Obsidian-style knowledge base for this project. Pages are concepts, links are dense, stubs are encouraged. Edits live here, not in commit history. See the [[build]] skill (`.claude/skills/build/SKILL.md`) for read/write conventions.

Start with [[RGB Fairy Lights]] — the top-level project page. For the chronological record of planned work, see [[plans/README|plans/]].

## Project overview
- [[RGB Fairy Lights]] — what this project is, at a glance

## Hardware
- [[QT Py ESP32-S3]] — primary target board (Adafruit product 5426, 8 MB flash, no PSRAM)
- [[QT Py ESP32]] — alternate/older target board (Adafruit product 5395, currently the default PlatformIO env)
- [[Neopixel Fairy Lights]] — the Adafruit 100-LED strand (product 4917)
- [[Wiring]] — power, ground, data, diode, JST connector
- [[Case]] — 3D-printed enclosure (`models/case.stl`, `models/lid.stl`)
- [[Power Supply]] — 5V/2A via barrel jack

## Firmware
- [[main.cpp]] — the sketch, entry points, structure
- [[PlatformIO]] — build/upload/monitor workflow
- [[Libraries]] — Adafruit_NeoPixel, PubSubClient, and the (unused) PCT2075/BusIO
- [[Patterns]] — the LED animation modes

## Network & integration
- [[WiFi Config]] — SSID, effective DHCP, hard-coded credentials
- [[MQTT]] — broker, connection, retry behavior
- [[MQTT Topics]] — command/state topic layout
- [[Home Assistant]] — how HA fits in
- [[Home Assistant Discovery]] — `publish_discovery.sh`, retained config payload

## Known issues / gotchas
- [[Payload size gotcha]] — PubSubClient's 256-byte default buffer forces `publish_discovery.sh`
- [[RGB to Hue gotcha]] — why `auroraSettable` doesn't respond to color changes

## Stubs (linked but not yet written)
- (none — expand as new concepts get mentioned)
