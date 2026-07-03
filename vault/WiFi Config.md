# WiFi Config

WiFi is configured by hard-coded string globals at the top of [[main.cpp]]:

```cpp
const char* ssid     = "Tomahna";      // in the working tree; was "CHANGEME" on main
const char* password = "CHANGEME";     // never commit the real password
const char* mqtt_server = "192.168.100.250";
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
```

The `gateway` and `subnet` values are declared but never applied — `WiFi.config(...)` is not called. **Effective behavior is DHCP.** Commit `f73b9d4` removed the previous `WiFi.config` call ("Removed unused TCP/IP settings"). The `readme.md` still describes an older workflow with `local_IP` — that block no longer exists in-source.

## Boot sequence
1. `WiFi.mode(WIFI_STA)` — client mode.
2. `WiFi.begin(ssid, password)`.
3. `WiFi.waitForConnectResult()` returns; if not `WL_CONNECTED`, `setup()` returns early — **the strand will not animate**. Serial prints `"WiFi Failed!"`.
4. A polling loop `while (WiFi.status() != WL_CONNECTED)` with `delay(1000)` — mostly unreachable given the early return above but harmless if reached.
5. Prints local IP over serial at `Serial.begin(112500)`.

Find the assigned IP by connecting to serial after boot — no static IP by default. If you need a fixed address, set a DHCP reservation on the router.

## Gotchas
- **Plaintext credentials in-source.** Never commit real values. The working-tree diff currently changes `ssid` from `CHANGEME` to `Tomahna` — that edit is uncommitted for a reason (`password` is still `CHANGEME`, and should stay that way in commits).
- The early `return` on WiFi-join failure means a router outage bricks the light entirely; there's no offline fallback. If the `NET_ENABLED` macro in [[main.cpp]] is commented out, the light works with the default `fireflies` effect and nothing else.
- Broker IP change: main-branch source had `192.168.100.76`; working tree has `192.168.100.250`. `src/publish_discovery.sh` still hard-codes the old `.76` — see [[Home Assistant Discovery]].

## Related
- [[main.cpp]] · [[MQTT]] · [[Home Assistant Discovery]]
