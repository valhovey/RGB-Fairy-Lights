# Patterns

The LED animation modes selectable over MQTT via the `effect` command topic (see [[MQTT Topics]]). Definitions live inline in `effectLoop()` in [[main.cpp]].

The full list is baked into the `effect_list` field of the [[Home Assistant Discovery]] payload:

```
static, rainbow, fireflies, aurora, auroraSettable, torchlight
```

`auroraSettable` is exposed to HA but does not currently work — see [[RGB to Hue gotcha]].

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

## Broken effect

### `auroraSettable`
Meant to be a colour-tunable aurora. Two known problems in the current code (see [[RGB to Hue gotcha]] for details):
1. A shadow variable: the loop body declares `float hue = auroraHue(...)` **and** reads `int hue = ...` from the outer scope. C++ scoping means the outer `hue` global is what feeds `startingHue`, but that global is only ever computed once at file scope from `getHue(r,g,b)` — the MQTT `rgb/set` callback updates `r`/`g`/`b` but never recomputes `hue`.
2. `endingHue = hue + deltaHue % 360` doesn't wrap what the author probably intended (`%` binds tighter than `+`), so it's really `hue + (deltaHue % 360) = hue + 70`, which can exceed 360.

Both are fixable in a few lines — treat as an open task.

## Shared globals used by every effect
- `power` (`"on"`/`"OFF"`) — `OFF` skips the effect and fills black.
- `brightness` (0–255) — global scale.
- `r`, `g`, `b` — used by `static` and (indirectly, via `hue`) by `auroraSettable`.
- `randomPhases[100]` — baked at build time by the JS one-liner in the comment above the array.

## Related
- [[main.cpp]] · [[MQTT Topics]] · [[Home Assistant Discovery]] · [[RGB to Hue gotcha]]
