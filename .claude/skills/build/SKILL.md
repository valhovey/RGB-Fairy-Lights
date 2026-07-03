---
name: build
description: Debug, plan changes for, and reason about the RGB Fairy Lights ESP32 firmware project — Adafruit QT Py ESP32/ESP32-S3 running Adafruit_NeoPixel + PubSubClient, driving Neopixel fairy lights, controlled over MQTT by Home Assistant. Reads and continuously enriches an Obsidian-style vault at ./vault/ that holds long-term knowledge, gotchas, wiring notes, and firmware/config details. Use whenever the user asks about this project's firmware, PlatformIO builds/uploads, board bring-up, MQTT/HA integration, Neopixel patterns, wiring/assembly, the 3D-printed case, or any project-level troubleshooting.
---

# build — RGB Fairy Lights vault + project reasoning skill

You are working on the user's **RGB Fairy Lights** project — an Adafruit QT Py ESP32 / ESP32-S3 driving Adafruit Neopixel fairy lights, controlled over MQTT by a Home Assistant instance on the user's LAN. The firmware is a PlatformIO / Arduino-framework C++ sketch (`src/main.cpp`). Long-term knowledge about the project lives in the Obsidian-style vault at:

```
./vault/
```
(relative to the repo root: `/home/speleo/Documents/Programs/ESP32/RGB-Fairy-Lights/vault/`)

This skill is the bridge between the live project (code, hardware, build output, MQTT traffic) and that vault. Every invocation has two halves: **read** (orient yourself in the vault, then answer) and **write** (enrich the vault with whatever you newly observed).

## How the vault is shaped

Digital-garden style. Concepts not folders.

- **Flat file layout.** All concept notes live directly in `vault/`. No subdirectories — **with one exception:** `vault/plans/` holds the timestamped plan records described under "Persisting plans into the vault" below. Filenames mirror Obsidian link text: `Neopixel Wiring.md` is the target of `[[Neopixel Wiring]]`.
- **Pages are concepts, not journals.** Title is the noun. Body explains what it is on *this* project, current state, and gotchas. Never a changelog.
- **Dense interlinking.** Every page should be reachable from several others. Liberally `[[link]]` to related concepts inline.
- **Stub links are encouraged.** If you mention a concept that deserves its own page but doesn't have one yet, link it anyway (`[[OTA Updates]]`). The dangling link is a TODO for the next time the topic comes up.
- **`README.md` is the MOC** (Map of Content) — the top-level index. Add new pages to its appropriate section.
- **No frontmatter required** on vault pages. Plain markdown.

## What to do on every invocation

### 1. Orient

Always begin by reading `vault/README.md`. It is small and tells you what exists. From the MOC, identify the 1–5 pages most relevant to the user's question and read those before forming an answer.

If the user asks about a specific concept (MQTT topics, Neopixel wiring, board selection, etc.) and a matching page exists, read it. If only a partial match exists, follow `[[wikilinks]]` to traverse outward.

If the vault does not yet exist or `README.md` is missing, create the minimum scaffolding (empty MOC + `plans/README.md`) before proceeding.

### 2. Verify before relying

Vault entries can go stale (library upgrades, pin swaps, board changes, MQTT broker moves, HA reconfigurations, code edits). Before quoting a fact that the user is about to act on, sanity-check it against reality with a quick command or file read (`pio device list`, `grep` in `src/main.cpp`, `cat platformio.ini`, a targeted `mqtt` publish). If reality disagrees with the vault, **trust reality** and queue the page for an update.

### 3. Answer the user

Use the vault context to give a sharper, more personalized answer than you could from generic ESP32/Arduino/HA knowledge. Mention the relevant pages by name so the user can open them in Obsidian if they want.

### 4. Enrich the vault — always

This is the most important step. After answering, update the vault to reflect what was learned in this turn. Specifically:

- **New concept came up?** Create a new page for it. Even a 3-line stub with a `[[link]]` or two is valuable — it gives the topic structure for next time.
- **Existing page was incomplete?** Add the new detail. Cross-link to related pages.
- **Existing page was wrong or stale?** Edit it. Do not preserve outdated information for history — this is a wiki, not a log. Replace, don't append "Update:" sections.
- **You discovered a gotcha** (something that bit the user, or would bite a future you)? Either give it its own page (`Foo gotcha.md`) or add a `## Gotchas` section to an existing concept page.
- **You linked to a stub that now has real content?** Promote it: write the page.
- **Add reciprocal links.** If `A.md` links to `[[B]]`, then `B.md` should generally link back to `[[A]]`. Density beats hierarchy.
- **Update `README.md`** when you add a new page — one line under the appropriate section.

If you change anything in the vault during a turn, tell the user briefly at the end ("Updated `PlatformIO.md`, created `MQTT Topics.md`"). Don't recite diffs.

## Style rules for vault pages

- **Title line:** `# Page Name` matching the filename.
- **Lead sentence:** what this concept *is on this project* (not the generic definition).
- **Use sections** (`## Current state`, `## Gotchas`, `## Related`, `## Commands`) when content warrants them, but don't force them on tiny pages.
- **Prefer bullet lists** with concrete values (pin numbers, GPIO IDs, MQTT topics, library versions, board FQBNs) over prose.
- **Inline `[[wikilinks]]`** densely. A page with only a "Related" section at the bottom is too sparse — link as you write.
- **Be honest about uncertainty.** If a fact wasn't verified live this session, mark it `(as of YYYY-MM-DD)`. If something is a guess, say so.
- **No emoji**, no decorative headings, no marketing tone.
- **Short is fine.** A 5-line page with good links beats a 50-line page that nobody reads.

## What NOT to write in the vault

- Generic ESP32 / Arduino / Home Assistant / MQTT documentation that any web search would give. The vault is about *this project*.
- Transcripts of conversations or task progress. That belongs in the chat, not the vault.
- **Never garden credentials.** WiFi passwords, MQTT broker usernames/passwords, HA long-lived access tokens, API keys, TLS certs/keys, or any other secret must never be written into the vault — not even redacted, partial, or "example" values. If a secret must be referenced, refer to it by name/location only (`WIFI_PASS in main.cpp`), never by value.
- **Never garden internal network details.** Private/LAN IP addresses (the user's, the broker's, HA's, the ESP32's DHCP lease), MAC addresses, internal hostnames, mDNS names, subnet/gateway/DNS layout, port-forwarding rules, SSIDs, or any other detail that describes the user's private network. Refer to these by role, not value (`the MQTT broker`, `the HA host`, `<broker-ip>`), and keep concrete values in `src/main.cpp` / local config where the user manages them. This holds even when you learned the value from a live check (`pio device list`, `mqtt sub`, `ifconfig`) — observing it is fine, writing it into the vault is not.
- Time-bound TODOs ("fix this by Friday"). Use the page itself to describe the *current state*; if a fix is pending, note it as a gotcha or open question.

## Filename conventions

- Spaces in filenames are fine and expected (matches Obsidian link syntax).
- Use Title Case for concept names: `Neopixel Wiring.md`, `MQTT Topics.md`, `Home Assistant Discovery.md`.
- For gotcha pages, suffix `gotcha`: `Payload size gotcha.md`.
- For things tied to a specific board/library/version, only put the version in the title if disambiguation requires it (`QT Py ESP32-S3.md` vs `QT Py ESP32.md` is fine — they are different boards; `PubSubClient.md` is fine without a version).

## Quick reference — common live-check commands

Use these to verify vault facts before acting on them. Prefer `pio` (PlatformIO's CLI) over the VS Code UI for scriptable checks.

| What | Command |
|---|---|
| PlatformIO version | `pio --version` |
| Available/connected boards | `pio device list` |
| Build (default env) | `pio run` |
| Build (specific env) | `pio run -e adafruit_qtpy_esp32s3` |
| Upload firmware | `pio run -t upload` |
| Serial monitor | `pio device monitor` |
| Clean build | `pio run -t clean` |
| Installed libs (this project) | `pio pkg list` |
| Update libs | `pio pkg update` |
| Board / framework info | `pio boards adafruit_qtpy_esp32s3` |
| Firmware source | `grep -n <symbol> src/main.cpp` |
| Wifi/MQTT config in code | `grep -nE 'ssid\|password\|mqtt_server\|local_IP' src/main.cpp` |
| MQTT smoke test (subscribe) | `mqtt sub -h <broker-ip> -t '#' -v` (via `mqtt-cli`) |
| MQTT smoke test (publish) | `mqtt pub -h <broker-ip> -t <topic> -m <payload>` |
| HA discovery payload | `cat src/publish_discovery.sh` |
| Git state | `git status`, `git log --oneline -10` |

Prefer read-only checks. Do not flash the board or publish MQTT messages that would change device state without confirming with the user first — see "Actions that touch hardware or shared systems" below.

## Persisting plans into the vault

When you write a plan to `~/.claude/plans/<slug>.md` (the file Claude Code's plan mode tracks), you MUST also persist that plan into the vault so future agents can see what was proposed and what actually got done. The source file in `~/.claude/plans/` is ephemeral; `vault/plans/` is the durable record.

### Where it lives

- Path: `vault/plans/<YYYY-MM-DD>-<short-slug>.md`. The date is the date the plan was first written. The slug is short, hyphenated, descriptive (`aurora-hue-conversion`, not `can-you-help-me-with-the-lights`).
- Index: `vault/plans/README.md` — chronological list, one line per plan: `- YYYY-MM-DD — [Title](filename.md) — status`.

### Frontmatter (required)

```
---
created: YYYY-MM-DDTHH:MM±TZ
updated: YYYY-MM-DDTHH:MM±TZ
status: proposed | in-progress | done | abandoned
source: ~/.claude/plans/<original-slug>.md
---
```

- `created` is set once when the vault copy is first written, never changed afterward.
- `updated` is bumped every time the file is edited (including work-log appends).
- `status` transitions: `proposed` → `in-progress` (when the first work-log entry lands) → `done` (when verification passes) or `abandoned` (if the user redirects or cancels). Keep the index in sync.

### Work log (required)

Every vault plan ends with a `## Work log` section. As work executes, append one bullet per non-trivial action:

```
- YYYY-MM-DD HH:MM — what was done — outcome / next
```

Mirror entries into the source `~/.claude/plans/<slug>.md` so the in-conversation plan also reflects reality. Record both successes and failures — the log is for future-you and future agents, not a marketing summary.

### Backfill rule

If you find unsaved plan files in `~/.claude/plans/` that have no vault counterpart, create stubs in `vault/plans/` with `status: unknown` and a single work-log line noting that prior progress was not captured.

### When NOT to use this

- One-off `/loop` / `/schedule` instructions that aren't real plans.
- Trivial single-edit tasks where no plan file was created.
- Anything that doesn't go through plan mode and doesn't warrant a durable record.

## Actions that touch hardware or shared systems

Some operations in this project have real-world side effects and cannot be silently retried:

- **Flashing the board** (`pio run -t upload`) — bricking risk if the wrong env is chosen or a bad build lands; also interrupts whatever the ESP32 is currently doing.
- **Publishing MQTT messages** to the live broker — can change the state of the fairy lights (or, worse, of a same-topic entity in Home Assistant if the topic is misnamed).
- **Publishing HA discovery payloads** (`src/publish_discovery.sh`) — retained, so a mistake sticks around until explicitly cleared.
- **Editing WiFi / MQTT credentials in `src/main.cpp`** — easy to leak via a commit if not careful.

For any of the above, **confirm with the user before running**, print the exact command you intend to execute with a one-line reason, and — for uploads — confirm the board is plugged in and which env is targeted. After the action, verify with a read-only check (serial monitor output, MQTT subscribe, `git status`) before continuing.

## Failure modes to avoid

- **Don't answer before reading the vault.** Even a quick `README.md` skim reframes generic advice into something specific.
- **Don't trust the vault blindly.** If a fact is load-bearing for an action, verify it live.
- **Don't leave the vault unchanged after a non-trivial project conversation.** Either you confirmed existing info (no edit needed), or you learned something new (edit needed). Pure no-op turns should be rare.
- **Don't create duplicate pages.** Check `README.md` and `ls vault/` for an existing page before creating a new one. If two pages cover the same concept, merge them.
- **Don't reorganize aggressively.** The vault is the user's; small additive edits each turn, not big restructures.
- **Don't garden secrets or internal network details** when enriching the vault — WiFi passwords, MQTT credentials, HA tokens, private LAN IPs, MAC addresses, and internal hostnames belong in `src/main.cpp` / local config (which the user manages), never copied into vault pages. See "What NOT to write in the vault" for the full rule. If you catch an earlier turn having written one of these into a vault page, scrub it (replace the value with a by-name reference) as part of the current turn.
