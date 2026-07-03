# RGB to Hue gotcha

The `auroraSettable` [[Patterns|effect]] is exposed to Home Assistant (it's in the `effect_list` of the discovery payload — see [[Home Assistant Discovery]]) but does not actually respond to color changes. `readme.md`'s "Known Problems" section calls this out: *"The settable aurora pattern doesn't work yet... I need to find a good formula to convert RGB to hue."*

## What's actually broken

Two issues in [[main.cpp]]'s `effectLoop` `auroraSettable` branch:

### 1. Stale `hue` global
```cpp
int hue = getHue(r, g, b);   // computed ONCE at file scope, at boot
```
The `rgb/set` MQTT callback updates `r`/`g`/`b`, but never re-runs `getHue`. So `hue` stays whatever it was at startup — for the default `(r,g,b) = (100,0,100)`, that's ~300° (magenta) forever, regardless of what HA sends.

### 2. Operator precedence
```cpp
int endingHue = hue + deltaHue % 360;
```
`%` binds tighter than `+`, so this is `hue + (deltaHue % 360)` = `hue + 70`. The author almost certainly meant `(hue + deltaHue) % 360` — wrap the sum. As written, `endingHue` can exceed 360 and gets fed to `auroraHue` → `beegHueFromSmol`, which just linearly scales, so hues past 360 wrap oddly through the 16-bit `ColorHSV` space.

### 3. Shadow variable inside the loop
```cpp
float hue = auroraHue(pTime, randomPhases[i], startingHue, endingHue);
strip.setPixelColor(i, strip.ColorHSV(hue, 255, brightness));
```
Inside the for-loop, a new local `float hue` shadows the outer `int hue`. That's fine on its own — but it means the outer `int hue` was only ever needed to seed `startingHue`, which reinforces issue #1.

## The canonical fix

Recompute hue in the `rgb/set` callback, and wrap the sum:

```cpp
} else if (topic == mqtt_rgb_command_topic) {
  // ...existing parse...
  hue = getHue(r, g, b);           // <-- add this
  publishRgb();
}
```

```cpp
} else if (effect == "auroraSettable") {
  float pTime = t / 4000.0;
  int deltaHue = 70;
  int startingHue = hue;
  int endingHue = (hue + deltaHue) % 360;   // <-- parenthesize
  for (int i = 0; i < LED_COUNT; ++i) {
    float h = auroraHue(pTime, randomPhases[i], startingHue, endingHue);
    strip.setPixelColor(i, strip.ColorHSV(h, 255, brightness));
  }
}
```

The `getHue()` function in [[main.cpp]] already implements the [canonical RGB→hue formula](https://en.wikipedia.org/wiki/HSL_and_HSV#Formal_derivation) — the readme's "need a good formula" is misdirection; the function is right, it just never gets called after boot.

## Related
- [[Patterns]] · [[main.cpp]] · [[Home Assistant Discovery]] · [[MQTT Topics]]
