# Power Supply

**5 V / 2 A regulated DC, 2.1 mm barrel jack, center-positive.** `readme.md` links [Adafruit product 276](https://www.adafruit.com/product/276).

Plugs into the panel-mount barrel jack in the [[Case]]; power flows through the reverse-polarity diode (see [[Wiring]]) to the QT Py 5 V rail, which also powers the strand.

## Why 2 A is enough
- WS2812 rule of thumb: ~60 mA/LED at full white → 100 LEDs at full white would draw ~6 A. The 2 A supply cannot deliver that.
- None of the current [[Patterns]] hit "all pixels, full white" — `fireflies`, `aurora`, and `torchlight` all keep large fractions of the strand dim; `rainbow` is bright but never white; `static` is user-controlled but rarely white in practice.
- If you ever add a full-white effect (e.g. a bright-white "task lighting" mode) or raise `brightness` to 255 on `static` with `r=g=b=255`, expect brownouts and possible USB brown-outs on the QT Py. Either cap brightness in firmware or move to a beefier PSU.

## Gotchas
- Center-negative wall warts will feed reverse polarity into the jack. The diode in [[Wiring]] is exactly for this — but check anyway; different brands print polarity differently.
- USB-C power (from a laptop or a USB PD wart) also works for bench testing — the QT Py bridges USB 5 V to the same rail. Do **not** connect both USB and barrel jack from ungrounded/isolated supplies simultaneously; ground offsets can flow through the strand's data line.

## Related
- [[Wiring]] · [[Case]] · [[Neopixel Fairy Lights]] · [[Patterns]]
