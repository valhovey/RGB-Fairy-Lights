# Neopixel Fairy Lights

Adafruit product [4917](https://www.adafruit.com/product/4917) — "Fixed Address" NeoPixel Fairy Lights. What this project drives.

## Strand specs
- **100 LEDs**, pre-addressed 0…99. `LED_COUNT 100` in [[main.cpp]] matches this, and the `randomPhases[]` array is length 100 for the per-pixel phase offsets used by the [[Patterns|firefly/aurora effects]].
- **"Fixed address" = each LED responds only to its own slot in the WS2812 stream.** You cannot chain a second strand for pixels 100–199; they'd both listen to the same addresses.
- **Nominal 5 V, do not exceed 6 V.** See [[Power Supply]].
- WS2812-compatible protocol per Adafruit's product page.

## Wire colors (as used in [[Wiring]])
- **Red** — +5 V
- **Green** — data in (goes to `LED_PIN`, i.e. `A0`)
- **Black** — GND

## Color order — an unresolved curiosity
- WS2812B spec is **GRB** color order (per the [WS2812B datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)).
- The code declares `Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800)` — i.e. **RGB order**.
- The effects visibly work as intended (see the aurora GIF in `readme.md`), so either this specific strand really is RGB-ordered (some Adafruit strands are), or the firefly/aurora palettes happen to hide the swap. Worth verifying with `static` + `r=255,g=0,b=0`: if it looks red, RGB is right; if green, switch to `NEO_GRB`.

## Current draw
- WS2812 rule of thumb: ~20 mA per channel at full white → ~60 mA/LED → **~6 A peak for 100 LEDs at full white**.
- Never happens in the [[Patterns|current effects]] (nothing hits full white all at once), so the [[Power Supply|5 V / 2 A]] adapter is fine for the palettes we use. Would be undersized for a full-white pattern.

## 3.3 V data-line quirk
- WS2812B `V_IH ≥ 0.7 × VDD` → at 5 V that's 3.5 V, above the ESP32's 3.3 V output high. Adafruit's official guidance is a **~470 Ω series resistor** on data and, if needed, a **74AHCT125** level shifter ([Uberguide](https://learn.adafruit.com/adafruit-neopixel-uberguide/basic-connections)).
- In practice at short cable lengths (this project's runs are inches inside the [[Case]]), the strand works fine driven directly from an ESP32 3.3 V line. If flicker ever appears, add the resistor first.

## Related
- [[Wiring]] · [[Power Supply]] · [[Patterns]] · [[main.cpp]] · [[Libraries]]
