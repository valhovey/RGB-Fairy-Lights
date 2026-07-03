# Patterns

The LED animation modes selectable over MQTT via the `effect` command topic (see [[MQTT Topics]]). Definitions live inline in `effectLoop()` in [[main.cpp]].

The full list is baked into the `effect_list` field of the [[Home Assistant Discovery]] payload:

```
static, rainbow, fireflies, aurora, auroraSettable, torchlight
```

All six effects work as of 2026-07-03 (`auroraSettable` was fixed — see [[RGB to Hue gotcha]]).

Each time-based effect derives its phase clock as `millis() % <period_ms>` where the
period is a close integer approximation of that effect's fundamental animation period.
This keeps the float phase small forever (no long-uptime precision loss — the old
`MAXVAL` rollover trick is gone) while wrapping seamlessly, since every sine/cosine
term's frequency is an integer multiple of the base.

## Working effects

### `static`
Fills all pixels with the current `(r, g, b)`. Colour is set via the `rgb/set` topic. Brightness respects the shared `brightness` global.

### `rainbow`
Delegates to the Neopixel library's own `strip.rainbow()`, driven by `millis() % 128000`. Cycles hue across the strand.

### `fireflies`
Default effect at boot (`String effect = "fireflies";`). Per-LED intensity from `fireflyActivation(t, phase)` — a hand-crafted sum of sines mixed with a wide sigmoid — mapped through:
- `fireflyHue(intensity)` — interpolates hue between `63` (warm) and `262` (cool), converted to the 16-bit hue space via `beegHueFromSmol`.
- `fireflyBrightness(intensity)` — sigmoid-shaped brightness envelope scaled by the shared `brightness`.
- Time base: `t = millis()/8000.0`. Each pixel is offset by `randomPhases[i]` so the strand isn't in sync.

### `aurora`
Hard-coded slow oscillation between hue `150` and `220` (green→blue). Per-pixel phase from `randomPhases[]`. Time base `t/4000.0`.

### `torchlight`
Same maths as `aurora`, hue range `18`–`39` (orange→amber). Time base `t/3000.0`.

### `auroraSettable`
Colour-tunable aurora: a calm ~80° band centered on the current color's hue (`hue` to
`hue + 70`). Now settable — the `color` command recomputes `hue`. The tight `hue + 70`
delta is a **deliberately preserved happy accident** (the "corrected" wrapped version
looked garish); see [[RGB to Hue gotcha]]. Do not "fix" the precedence.

## Shared globals used by every effect
- `isOn` (bool) — `false` skips the effect and fills black. (Replaced the old
  `power` string, killing the `OFF` vs `off` case foot-gun.)
- `brightness` (0–255) — global scale.
- `r`, `g`, `b` — used by `static` and (via `hue`) by `auroraSettable`.
- `randomPhases[100]` — baked at build time by the JS one-liner in the comment above the array.

## Related
- [[main.cpp]] · [[MQTT Topics]] · [[Home Assistant Discovery]] · [[RGB to Hue gotcha]]
