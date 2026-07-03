# Payload size gotcha

**Resolved (2026-07-03).** This used to be why [[Home Assistant Discovery]] was a
shell script instead of a `client.publish()` in [[main.cpp]].

## The old constraint
- [[Libraries|PubSubClient]]'s default `MQTT_MAX_PACKET_SIZE` is **256 bytes**
  (header + topic + payload). Anything larger silently fails — `publish()` returns
  `false` and the broker never sees it.
- The HA discovery payload is ~700 bytes, far over 256, so it couldn't be sent from
  the device.

## The fix that was applied
`client.setBufferSize(1024)` in `setup()`. The device now publishes its own retained
discovery from `publishDiscovery()` (and the JSON state payloads) directly — the
out-of-band `publish_discovery.sh` was deleted. Combined with the move to HA's JSON
light schema (one command/state topic, see [[MQTT Topics]]), payload sizes stay well
under the buffer.

## If you add more discovery fields
Keep an eye on the total: the buffer must exceed `fixed header + topic + JSON`. Bump
`setBufferSize()` further if you add a large `device` block, more effects, etc. It
costs ~800 bytes of RAM per the current setting — trivial on ESP32.

## Related
- [[Home Assistant Discovery]] · [[Libraries]] · [[MQTT]] · [[MQTT Topics]]
