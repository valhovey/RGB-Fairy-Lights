# QT Py ESP32-S3

Adafruit product [5426](https://www.adafruit.com/product/5426), ESP32-S3 SoC in the QT Py form factor. The board recommended in `readme.md` and the target of the `adafruit_qtpy_esp32s3` [[PlatformIO]] env.

## Pinout for this project
- **`A0` → GPIO 18.** This is what `LED_PIN` in [[main.cpp]] resolves to. Digital output driving the [[Neopixel Fairy Lights]] strand's data line via [[Wiring]].
- **Onboard NeoPixel:** GPIO 39 (data), GPIO 38 (`NEOPIXEL_POWER`, active HIGH). Not used by this firmware — the strand is on the STEMMA-side of the board.
- **Native USB-C** (USB-Serial/JTAG built into the S3). No CP2102/CH340 bridge.

## Flash / PSRAM
- Product **5426 = 8 MB flash, NO PSRAM** (Adafruit product page + variant header). `readme.md`'s "8MB flash w/ PSRAM" phrasing is misleading — the 8 MB is *no-PSRAM*, and the 4 MB variant (product 5700) is the one with 2 MB PSRAM.
- `platformio.ini` sets `board_upload.flash_size = 8MB` correctly for 5426.
- If you ever swap to the PSRAM variant, PlatformIO board id is `adafruit_qtpy_esp32s3_n4r2`.

## Bootloader / reset
- Native USB, so the ROM DFU works: hold **BOOT** (GPIO 0) while tapping **RESET** to force download mode. Normally PlatformIO can trigger this automatically over USB.
- If a bad firmware bricks USB, the BOOT+RESET dance always works.

## Gotchas
- **`A0` and `A1` are on ADC2**, which is shared with the WiFi radio on the S3. Not a problem here because A0 is being used as digital output for Neopixel data, but if you ever add an analog sensor, use A2/A3 (ADC1) or you'll get garbage while WiFi is up.
- No DAC on the S3 (unlike the non-S3 [[QT Py ESP32]] — its A0 is also DAC2).
- Adafruit BSP asserts `NEOPIXEL_POWER` HIGH automatically; if you're driving the onboard pixel from a different framework and it stays dark, that pin is why.

## Related
- [[QT Py ESP32]] · [[PlatformIO]] · [[Wiring]] · [[Neopixel Fairy Lights]] · [[main.cpp]]
