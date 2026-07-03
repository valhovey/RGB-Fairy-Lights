# PlatformIO

Build system for this project. Two environments defined in `platformio.ini`:

| env | board | flash | notes |
|---|---|---|---|
| `adafruit_qtpy_esp32` | [[QT Py ESP32]] | default | Currently the **default env** (`default_envs = adafruit_qtpy_esp32`). |
| `adafruit_qtpy_esp32s3` | [[QT Py ESP32-S3]] | `board_upload.flash_size = 8MB` | The board recommended in `readme.md`. |

Both envs pull the same libraries from git — see [[Libraries]].

## Local install
- The CLI is installed under `~/.platformio/penv/bin/pio` but **not on `$PATH`**. Either activate the venv or call the full path.
- Confirmed version (as of 2026-07-03): `PlatformIO Core, version 6.1.19`.

## Common commands
```bash
~/.platformio/penv/bin/pio run                                  # build default env (esp32, non-S3)
~/.platformio/penv/bin/pio run -e adafruit_qtpy_esp32s3         # build S3 env
~/.platformio/penv/bin/pio run -t upload                        # flash default env
~/.platformio/penv/bin/pio run -e adafruit_qtpy_esp32s3 -t upload
~/.platformio/penv/bin/pio device monitor                       # serial monitor
~/.platformio/penv/bin/pio device list                          # what's connected
~/.platformio/penv/bin/pio pkg list                             # resolved lib versions
```

Serial baud in [[main.cpp]] is **`Serial.begin(112500)`** — set the monitor to match.

## Gotchas
- The default env is the non-S3 board; the `readme.md` sells the project on the S3. If you plug in an S3 and run `pio run -t upload` without `-e adafruit_qtpy_esp32s3`, PlatformIO will build for the wrong chip. Always pass `-e` unless you're sure the default matches your board.
- Two of the `lib_deps` entries use `git@github.com:` SSH URLs (`Adafruit_NeoPixel`). If your GitHub SSH isn't set up, that fetch fails; the fix is either add an SSH key or swap the URL for `https://github.com/…`.
- `upload_port` is commented out on the non-S3 env (references an old macOS device path `/dev/tty.wchusbserial…`). Autodetect works on Linux; force it with `upload_port = /dev/ttyACM0` if needed.
- `.pio/` is git-ignored; safe to `rm -rf .pio` for a clean rebuild.

## Related
- [[main.cpp]] · [[Libraries]] · [[QT Py ESP32]] · [[QT Py ESP32-S3]]
