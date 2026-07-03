# WiFi Config

WiFi credentials live in the gitignored [[Secrets]] header (`include/secrets.h`),
**not** in [[main.cpp]] anymore. `main.cpp` reads them via `#define`s:

```cpp
WiFi.begin(WIFI_SSID, WIFI_PASS);   // both from secrets.h
```

`WIFI_SSID`, `WIFI_PASS`, and `MQTT_SERVER` are all defined in `include/secrets.h`
(copied from the committed `include/secrets.example.h`). The old in-source `ssid` /
`password` / `mqtt_server` globals and the unused `gateway`/`subnet` `IPAddress`
pair are gone. **Effective behavior is DHCP** (no `WiFi.config()` call).

## Boot / recovery sequence
1. `WiFi.mode(WIFI_STA)`, `WiFi.setAutoReconnect(true)`, `WiFi.persistent(false)`.
2. `WiFi.begin(WIFI_SSID, WIFI_PASS)`.
3. `setup()` waits **up to 10 s** for a join, then proceeds **regardless** — the
   strand starts animating even if WiFi never comes up (no more early-return brick).
4. In `loop()`, `ensureConnected()` re-checks `WiFi.status()` every ~5 s and calls
   `WiFi.reconnect()` if it dropped — non-blocking, so the animation never stalls.
   This is the core of the [[MQTT]] freeze fix.

Find the assigned IP over serial at `Serial.begin(115200)` (baud updated from the
old nonstandard 112500; `monitor_speed = 115200` is now set in [[PlatformIO]]). Use a
DHCP reservation on the router if you want a fixed address.

## Gotchas
- **Credentials are no longer in source.** They live in the gitignored [[Secrets]]
  file. Do not reintroduce plaintext creds into `main.cpp`.
- WiFi loss is now self-healing (auto-reconnect + the `ensureConnected()` backstop).
  A [[Watchdog]] reboots the board if the loop ever stalls anyway.
- If `NET_ENABLED` in [[main.cpp]] is commented out, the light runs standalone with
  the default `fireflies` effect and no network code compiled in.

## Related
- [[Secrets]] · [[main.cpp]] · [[MQTT]] · [[Home Assistant Discovery]] · [[Watchdog]]
