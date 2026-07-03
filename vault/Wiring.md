# Wiring

Solder plan for one driver box. Sits between a [[Power Supply|barrel-jack 5 V input]] and the [[Neopixel Fairy Lights]] strand, with the QT Py ([[QT Py ESP32-S3]] or [[QT Py ESP32]]) as the controller.

## Connections

| From | To | Notes |
|---|---|---|
| Barrel jack + | QT Py 5 V pin, through a diode | Diode drops one Vf (~0.7 V for a generic silicon; less if Schottky). Protects against reverse polarity at the jack. |
| Barrel jack − | QT Py GND | Shared ground with the strand. |
| JST connector (red / +5 V) | QT Py 5 V (same node as jack +) | Power the strand from board 5 V. |
| JST connector (black / GND) | QT Py GND | |
| JST connector (green / data) | QT Py `A0` — **GPIO 18 on S3, GPIO 26 on non-S3** | This is `LED_PIN` in [[main.cpp]]. |

USB-C is only used for flashing and serial. Once mounted in the [[Case]], the barrel jack powers everything.

## The diode
- On the +5 V line from the barrel jack, before the QT Py 5 V pin. Cathode (band) toward the QT Py.
- Purpose: reverse-polarity protection at the jack. If a wall-wart with reversed polarity is plugged in, the diode blocks — nothing gets fried.
- Trim the leads short before soldering; the whole board has to fit in the [[Case]].
- Silicon diode is fine; a Schottky (e.g. 1N5817) drops less voltage, giving the strand a bit more headroom.

## Data-line notes
- WS2812B officially wants `V_IH ≥ 3.5 V` at 5 V VDD, but the ESP32's 3.3 V logic works in practice at the short cable runs inside the [[Case]]. See [[Neopixel Fairy Lights]] for the level-shifter/resistor upgrade path if flicker ever appears.
- No series resistor is currently installed. Adafruit's Uberguide recommends 470 Ω — cheap insurance for a future revision.

## JST connector orientation
- The male connector is the "outside" of the case, going to the strand. The bare wires from its inside pigtail land on the QT Py.
- Match strand colors when plugging in: red=+5 V, black=GND, green=data. Wire color is documented on the [[Neopixel Fairy Lights]] page too.

## Gotchas
- Getting the diode polarity backwards makes the strand appear completely dead — no power to the QT Py at all. Symptom looks identical to "wrong barrel jack".
- If A0 is connected but you're seeing no light, verify `LED_PIN` in [[main.cpp]] is `A0` and that the correct board is being built (`pio run -e <env>`; see [[PlatformIO]]).

## Related
- [[QT Py ESP32-S3]] · [[QT Py ESP32]] · [[Neopixel Fairy Lights]] · [[Power Supply]] · [[Case]] · [[main.cpp]]
