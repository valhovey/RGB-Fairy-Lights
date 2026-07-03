# RGB to Hue gotcha

The `auroraSettable` [[Patterns|effect]] has a history here. As of 2026-07-03 it is
**color-settable AND keeps its original look on purpose** — one of its three original
"bugs" turned out to be a happy accident and is retained deliberately.

## The three original issues
1. **Stale `hue` global** — `int hue = getHue(r,g,b)` was computed once at boot and
   never recomputed, so the effect ignored HA color. **Fixed:** `onCommand` now calls
   `hue = getHue(r, g, b)` whenever a `color` arrives (see [[MQTT Topics]]).
2. **Operator precedence** — `endingHue = hue + deltaHue % 360` parses as `hue + 70`
   (a *tight* 70° delta), not `(hue + deltaHue) % 360`. **Kept on purpose** — see below.
3. **Shadow variable** — a loop-local `float hue` shadowed the global. Cleaned up
   (renamed to `pixelHue`); behaviorally a no-op.

## The happy accident (do not "fix")
The tight `hue + 70` delta is what makes the effect pretty. With `auroraHue`'s mild
intensity swing (~±0.57), the rendered band is `hue ± 40` — a calm ~80° arc centered
on the base hue (e.g. magenta 300 → 260–340, blues/purple/pink). The "corrected"
`(hue + deltaHue) % 360` instead makes the interpolation span ~290° and wrap through
the whole color wheel — garish. So the code keeps `endingHue = hue + deltaHue`
(unwrapped) intentionally. For high base hues the top of the band overflows and wraps
through red in `ColorHSV`'s 16-bit space, giving a pleasant sunset edge.

Net effect: because #1 is fixed, the accident now follows whatever color you set in HA
— the effect finally lives up to its "settable" name while preserving the look. The
`main.cpp` branch carries a long comment marking it as intentional.

`getHue()` implements the standard RGB→hue formula and was always correct.

## Related
- [[Patterns]] · [[main.cpp]] · [[Home Assistant Discovery]] · [[MQTT Topics]]
