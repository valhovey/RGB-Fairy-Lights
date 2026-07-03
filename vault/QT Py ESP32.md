# QT Py ESP32

Adafruit product [5395](https://www.adafruit.com/product/5395), the non-S3 QT Py Pico module. Currently the **default** [[PlatformIO]] env (`default_envs = adafruit_qtpy_esp32`) despite the `readme.md` recommending the [[QT Py ESP32-S3]].

## Pinout for this project
- **`A0` → GPIO 26.** Different GPIO number from the S3 board. Same code path in [[main.cpp]] resolves to the right pin because Arduino's variant header remaps `A0` for each board. Also happens to be `DAC2` — irrelevant for our digital-out use.
- **Onboard NeoPixel:** GPIO 5 (data), GPIO 8 (`NEOPIXEL_POWER`, active HIGH). Not used here.
- **USB-C via CP2102N bridge** — not native USB. If Linux doesn't pick up the port, install the CP210x driver. Serial device is usually `/dev/ttyUSB0` (contrast with the S3's `/dev/ttyACM0`).

## Chip
- ESP32-PICO-MINI-02 SiP, 8 MB flash + 2 MB PSRAM. Standard ESP32 bootloader (hold GPIO 0 low at reset).

## Gotchas
- GPIO 6–11 are tied to internal flash and are not usable as GPIOs. Not a problem here since we're on `A0`/GPIO 26.
- A0/A1 double as DACs; `dacWrite()` on the same pin clobbers `analogRead()`. Irrelevant for our digital output use.
- The default [[PlatformIO]] env targets this board, so `pio run -t upload` without `-e` will build for the ESP32 even if you have an S3 plugged in. Always pass `-e adafruit_qtpy_esp32s3` for the S3.
- `platformio.ini` has an old macOS `upload_port = /dev/tty.wchusbserial…` commented out — a hint this board was used on a Mac at some point.

## Related
- [[QT Py ESP32-S3]] · [[PlatformIO]] · [[Wiring]] · [[main.cpp]]
