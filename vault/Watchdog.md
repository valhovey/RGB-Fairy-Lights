# Watchdog

An ESP32 task watchdog (`esp_task_wdt`) that reboots the board if the main `loop()`
stalls for `WDT_TIMEOUT_S` (30 s). Defense-in-depth backstop added alongside the
[[MQTT]] freeze fix — if any future bug ever wedges the loop, the board recovers on
its own instead of hanging until a manual power-cycle.

## How it's wired in [[main.cpp]]
- `initWatchdog()` (called at the end of `setup()`) configures the timeout and adds
  the loop task via `esp_task_wdt_add(NULL)`.
- `esp_task_wdt_reset()` is called once per `loop()` iteration — as long as the loop
  keeps turning, the WDT never fires.
- The init call is version-guarded: Arduino-ESP32 3.x (IDF 5.x) uses the
  `esp_task_wdt_config_t` struct + `esp_task_wdt_reconfigure()`; 2.x uses the older
  `esp_task_wdt_init(timeout_s, panic)`. Guarded on `ESP_ARDUINO_VERSION`.

## Gotchas
- The blocking bits that remain (`client.connect()` can block up to PubSubClient's
  socket timeout, ~15 s) are well under the 30 s window. If you ever lower the WDT
  timeout below ~15 s, also lower `client.setSocketTimeout()`.

## Related
- [[MQTT]] · [[main.cpp]]
