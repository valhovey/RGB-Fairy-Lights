# Case

3D-printed enclosure for the driver box. Two STL files in `models/`:

- `models/case.stl` — main body with slots for the USB-C port (QT Py side), the barrel jack (through-wall mount), and the JST connector (through-wall, upper slot above the USB-C).
- `models/lid.stl` — snap-on top.

`readme.md` recommends PLA and Cura for slicing; no exotic material or support settings required.

## Assembly order (summary of `readme.md`)
1. Solder the barrel-jack power leads and heat-shrink them; mount the jack in its case slot and tighten the nut.
2. Solder power leads to QT Py (5 V through the diode; GND direct). See [[Wiring]].
3. Slide the JST pigtail bare wires through the upper slot and solder to the QT Py (data → `A0`; power/GND share the jack node).
4. Plug the strand into the JST connector to verify wire colors — see [[Neopixel Fairy Lights]] and [[Wiring]] for the color map.
5. Stick the QT Py down with double-sided tape, USB-C slotted into the case wall, fold the board into its walled area.
6. Snap the lid on.
7. If leaded solder was used: hand-wash and wipe the case with a paper towel.

## Gotchas
- The USB-C slot is only accessible before the lid goes on. Flash + confirm the firmware works before final assembly, or re-flashing means popping the lid.
- No dedicated ventilation. The ESP32 doesn't get hot for this workload, but stacking cases (multiple units next to each other) can trap warmth.
- Slots are sized for the exact Adafruit parts listed in `readme.md`. Swapping to a different-brand JST or barrel jack may require rescaling the slots.

## Related
- [[Wiring]] · [[Power Supply]] · [[QT Py ESP32-S3]] · [[QT Py ESP32]]
