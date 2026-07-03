// Template for secrets.h. Copy this file to secrets.h and fill in real values:
//
//     cp include/secrets.example.h include/secrets.h
//
// secrets.h is gitignored; this template is committed. Do not put real
// credentials here.
#pragma once

// WiFi
#define WIFI_SSID   "your-wifi-ssid"
#define WIFI_PASS   "your-wifi-password"

// MQTT broker
#define MQTT_SERVER "192.168.1.10"
#define MQTT_PORT   1883
// Leave user/pass empty ("") to connect anonymously.
#define MQTT_USER   "mqtt-username"
#define MQTT_PASS   "mqtt-password"

// Home Assistant entity identity (slug drives every topic; must be unique per unit)
#define DEVICE_SLUG "bedroom_fairy"
#define DEVICE_NAME "Bedroom Fairy Lights"
