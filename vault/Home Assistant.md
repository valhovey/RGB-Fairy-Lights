# Home Assistant

The user's smart-home controller — the entity that actually drives the fairy lights via UI. Not part of this repo; runs on a separate host on the same LAN as the [[MQTT]] broker.

This project integrates with HA purely via MQTT: the ESP32 is a "dumb" MQTT client, and HA is taught about it via a **retained** discovery payload — see [[Home Assistant Discovery]].

## Where HA-specific config lives
- **In HA itself:** the MQTT integration must be installed and pointed at the same broker as [[main.cpp]]'s `mqtt_server`. This project assumes it's already there.
- **In this repo:** `src/publish_discovery.sh` — the one-shot script that publishes the retained config payload.

## What HA does with it
- Shows a "Bedroom Fairy Lights" device with brightness slider, RGB picker, and effect dropdown.
- Publishes to `.../set` topics on user interaction.
- Subscribes to `.../status` topics to reflect state; the ESP32 echoes state on every command (`optimistic: false` in the discovery payload).

## Not covered here
Generic HA setup (installing HA, adding the MQTT broker add-on, MQTT usernames) is intentionally out of scope. See the upstream HA MQTT docs.

## Related
- [[Home Assistant Discovery]] · [[MQTT]] · [[MQTT Topics]] · [[Payload size gotcha]]
