---
created: 2026-07-03T12:20-06:00
updated: 2026-07-03T13:05-06:00
status: in-progress
source: ~/.claude/plans/survey-the-codebase-and-zazzy-blanket.md
---

# MQTT/HA discovery refactor, MQTT auth, half-day freeze fix

Consolidates three asks plus a reinvented-wheel cleanup pass.

## Why

- [[MQTT Topics]] hand-rolls 8 topic strings and subscribes to each; [[Home Assistant Discovery]] is pushed out-of-band by `publish_discovery.sh` because of the [[Payload size gotcha]]. Wanted a more mature, `ha_mqtt_discoverable`-style approach.
- No MQTT auth (see [[MQTT]]); broker is adding a username/password.
- Firmware freezes after ~half a day — unresponsive to commands *and* animation stops.
- Secrets are hard-coded in [[main.cpp]]; unused libs in [[Libraries]].

## Key decision

The closest Arduino analog to Python `ha_mqtt_discoverable` is **ArduinoHA**
(`dawidchyrzynski/arduino-home-assistant`), but its `HALight` has **no `effect_list`
support** (open issue #142). Effects are core here (6 [[Patterns]]), so ArduinoHA was
rejected — it would force effects into a separate `select` entity.

**Chosen:** HA **JSON light schema** + **ArduinoJson**, discovery published from the
firmware. Keeps native `effect_list`, collapses 8 topics → 2 (`/set` + `/state` plus
`/status` availability), deletes the shell script, replaces manual string parsing.
MQTT transport stays on [[Libraries|PubSubClient]] with buffer bumped to 1024.

## Freeze root cause

Blocking `reconnect()` (`while(!connected){delay(1000)}`) + no WiFi recovery. When
WiFi drops after some hours, MQTT can't reconnect, so `reconnect()` spins forever and
the whole loop is stuck — animation frozen, commands ignored. Secondary: reconnect
never re-`subscribe`s, so even a clean reconnect drops command handling.

Fix: non-blocking `ensureConnected()` (millis-paced retry, no delay), WiFi
auto-reconnect + status check, re-subscribe/re-announce on every connect, watchdog
backstop.

## Scope

1. `include/secrets.h` (gitignored) + `secrets.example.h`; `.gitignore` entry.
2. Slug-derived topics from one `DEVICE_SLUG`.
3. ArduinoJson: `publishDiscovery()`, JSON `onCommand()`, JSON `publishState()`.
4. Non-blocking connection mgmt + WiFi recovery + re-subscribe.
5. MQTT auth (`connect(id,user,pass,willTopic,...)`) + availability/LWT.
6. Cleanups: drop unused libs, ESP32 task watchdog, fix `auroraSettable`
   ([[RGB to Hue gotcha]]), shrink effect phase time to curb float drift.
7. Delete `publish_discovery.sh`.

## Work log

- 2026-07-03 12:20 — Plan approved (JSON schema + ArduinoJson; gitignored secrets.h; all four cleanups). Record created. — starting implementation.
- 2026-07-03 12:50 — Added `include/secrets.h` (gitignored) + `secrets.example.h`; `.gitignore` entry verified via `git check-ignore`. Updated `platformio.ini`: added `ArduinoJson@^7`, dropped PCT2075/BusIO/SPI/Wire, added `[env] monitor_speed = 115200`. — done.
- 2026-07-03 12:55 — Rewrote `src/main.cpp`: JSON schema `onCommand`/`publishState`/`publishDiscovery`, non-blocking `ensureConnected()` + WiFi auto-reconnect + re-subscribe, MQTT auth + LWT/availability, esp_task_wdt watchdog, `auroraSettable` fix (hue recompute + precedence + un-shadow), per-effect `millis()%period` phase clock (removed `MAXVAL`), `isOn` bool, baud 112500→115200. Deleted `publish_discovery.sh`. — done.
- 2026-07-03 13:00 — Build: `adafruit_qtpy_esp32` SUCCESS. `adafruit_qtpy_esp32s3` failed — board id `adafruit_qtpy_esp32s3` no longer exists in espressif32; corrected to `adafruit_qtpy_esp32s3_nopsram` (8MB no PSRAM, matches Adafruit 5426). Both envs now build green. This was a pre-existing broken env, not caused by the refactor.
- 2026-07-03 13:05 — Secret-hygiene: real SSID was never committed in source, but WAS committed inside `vault/WiFi Config.md` — scrubbed it (by-name reference now) per the build skill. Enriched vault: MQTT, MQTT Topics, Home Assistant Discovery, Payload size gotcha (resolved), Libraries, RGB to Hue gotcha (resolved), Patterns, main.cpp, WiFi Config, README; new pages Secrets + Watchdog.
- 2026-07-03 13:05 — **Status: implementation code-complete, both PlatformIO envs build clean.** Remaining before `done`: on-hardware verification (flash, confirm HA entity auto-discovers + all 6 effects + auth, then drop WiFi to confirm the animation keeps running and recovers, ideally a >12h soak). Needs the user's board — not flashed from here.
- 2026-07-03 13:40 — Serial upload blocked by Linux port perms (user not in `dialout`; ModemManager active; udev rules outdated). Gave the user copy-paste sudo commands (they run privileged commands themselves — see memory). Not yet flashed.
- 2026-07-03 13:55 — Per user request, restored the `auroraSettable` "happy accident": reverted `endingHue` to the unwrapped `hue + deltaHue` (tight ~70° band) but KEPT hue tracking (issue #1 fix), so it's now settable AND preserves the calm look. Chosen over exact-original (which ignored color). Rebuilt esp32s3: SUCCESS. Updated RGB to Hue gotcha + Patterns to mark the precedence as intentional.
