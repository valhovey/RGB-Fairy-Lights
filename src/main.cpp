#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <esp_task_wdt.h>

#define LED_PIN A0
#define LED_COUNT 100
#define NET_ENABLED

// Reboot the board if the main loop stalls for this long (defense-in-depth
// against any future freeze).
#define WDT_TIMEOUT_S 30

#if defined(NET_ENABLED)
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

WiFiClient espClient;
PubSubClient client(espClient);

// All topics derive from the single DEVICE_SLUG in secrets.h — rename a unit in
// exactly one place. HA discovery is published by the device itself (below), so
// there is no external discovery script.
const String mqtt_command_topic      = String("home/light/") + DEVICE_SLUG + "/set";
const String mqtt_state_topic        = String("home/light/") + DEVICE_SLUG + "/state";
const String mqtt_availability_topic = String("home/light/") + DEVICE_SLUG + "/status";
const String mqtt_discovery_topic    = String("homeassistant/light/") + DEVICE_SLUG + "/config";

const char* const EFFECTS[] = {"static", "rainbow", "fireflies", "aurora", "auroraSettable", "torchlight"};
const size_t EFFECT_COUNT = sizeof(EFFECTS) / sizeof(EFFECTS[0]);
#endif

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

// Adapted from https://stackoverflow.com/a/26233318
int getHue(int red, int green, int blue) {
    float minVal = float(min(min(red, green), blue));
    float maxVal = float(max(max(red, green), blue));
    float hueVal = 0.0;

    if (minVal == maxVal) {
        return 0;
    }

    if (maxVal == red) {
        hueVal = (float(green) - float(blue)) / (maxVal - minVal);
    } else if (maxVal == green) {
        hueVal = 2.0 + (float(blue) - float(red)) / (maxVal - minVal);
    } else {
        hueVal = 4.0 + (float(red) - float(green)) / (maxVal - minVal);
    }

    hueVal = hueVal * 60.0;
    if (hueVal < 0) hueVal = hueVal + 360.0;

    return round(hueVal);
}

int r = 100;
int g = 0;
int b = 100;
int hue = getHue(r, g, b);
int brightness = 255;
bool isOn = true;
String effect = "fireflies";

#if defined(NET_ENABLED)
// Publish the current device state as a single JSON object on the state topic
// (HA "json" light schema). Retained so HA restores it after a restart.
void publishState() {
  JsonDocument doc;
  doc["state"] = isOn ? "ON" : "OFF";
  doc["brightness"] = brightness;
  doc["color_mode"] = "rgb";
  JsonObject color = doc["color"].to<JsonObject>();
  color["r"] = r;
  color["g"] = g;
  color["b"] = b;
  doc["effect"] = effect;

  String out;
  serializeJson(doc, out);
  client.publish(mqtt_state_topic.c_str(), out.c_str(), true);
}

// Publish the retained Home Assistant MQTT discovery config so the light entity
// auto-appears (replaces the old publish_discovery.sh). Sent on every connect.
void publishDiscovery() {
  JsonDocument doc;
  doc["schema"] = "json";
  doc["name"] = DEVICE_NAME;
  doc["unique_id"] = DEVICE_SLUG;
  doc["command_topic"] = mqtt_command_topic;
  doc["state_topic"] = mqtt_state_topic;
  doc["availability_topic"] = mqtt_availability_topic;
  doc["payload_available"] = "online";
  doc["payload_not_available"] = "offline";
  doc["brightness"] = true;
  JsonArray modes = doc["supported_color_modes"].to<JsonArray>();
  modes.add("rgb");
  doc["effect"] = true;
  JsonArray effects = doc["effect_list"].to<JsonArray>();
  for (size_t i = 0; i < EFFECT_COUNT; ++i) effects.add(EFFECTS[i]);
  JsonObject device = doc["device"].to<JsonObject>();
  JsonArray ids = device["identifiers"].to<JsonArray>();
  ids.add(DEVICE_SLUG);
  device["name"] = DEVICE_NAME;
  device["manufacturer"] = "Adafruit";
  device["model"] = "QT Py ESP32 Fairy Lights";

  String out;
  serializeJson(doc, out);
  client.publish(mqtt_discovery_topic.c_str(), out.c_str(), true);
}

// Single JSON command handler (HA "json" light schema). Every field is optional;
// HA sends only what changed.
void onCommand(char* rawTopic, byte* payload, unsigned int length) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("Command JSON parse failed: ");
    Serial.println(err.c_str());
    return;
  }

  if (doc["state"].is<const char*>()) {
    isOn = String(doc["state"].as<const char*>()) == "ON";
  }
  if (doc["brightness"].is<int>()) {
    brightness = doc["brightness"].as<int>();
  }
  if (doc["color"].is<JsonObject>()) {
    JsonObject c = doc["color"];
    r = c["r"] | r;
    g = c["g"] | g;
    b = c["b"] | b;
    hue = getHue(r, g, b);   // keep hue in sync for the auroraSettable effect
  }
  if (doc["effect"].is<const char*>()) {
    effect = doc["effect"].as<const char*>();
  }

  publishState();
}

// Runs after every successful MQTT (re)connect: re-subscribe, re-announce, and
// re-publish state. The old code never re-subscribed, so commands were silently
// dropped after the first reconnect.
void onMqttConnected() {
  client.subscribe(mqtt_command_topic.c_str());
  client.publish(mqtt_availability_topic.c_str(), "online", true);
  publishDiscovery();
  publishState();
  Serial.println("MQTT connected: subscribed, announced online");
}

bool connectMqtt() {
  const char* user = strlen(MQTT_USER) ? MQTT_USER : nullptr;
  const char* pass = strlen(MQTT_PASS) ? MQTT_PASS : nullptr;

  // Last Will: broker publishes retained "offline" on the availability topic if
  // we drop unexpectedly, so HA shows the device unavailable (not stale).
  bool ok = client.connect(DEVICE_SLUG, user, pass,
                           mqtt_availability_topic.c_str(), 0, true, "offline");
  if (ok) {
    onMqttConnected();
  }
  return ok;
}

// Non-blocking network keep-alive. Never blocks the animation: WiFi + MQTT
// recovery is attempted at most once per NET_RETRY_MS and always returns quickly.
// This replaces the old blocking reconnect() that spun forever (freezing the
// strand) when WiFi was down.
const unsigned long NET_RETRY_MS = 5000;
unsigned long lastNetAttempt = 0;

void ensureConnected() {
  if (WiFi.status() == WL_CONNECTED && client.connected()) {
    return;
  }

  unsigned long now = millis();
  if (now - lastNetAttempt < NET_RETRY_MS) {
    return;   // back off — let effectLoop() keep animating
  }
  lastNetAttempt = now;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi down — reconnecting");
    WiFi.reconnect();
    return;   // pick up MQTT on a later pass once WiFi is back
  }

  Serial.println("MQTT down — reconnecting");
  if (!connectMqtt()) {
    Serial.print("MQTT connect failed, state=");
    Serial.println(client.state());
  }
}
#endif

// Generated with:
// Array(100).fill().map(() => Math.random()).map(x => x * Math.PI * 2).map(x => x.toFixed(2)).join(',')
float randomPhases[] = {5.04,5.81,4.95,6.19,2.88,3.42,5.60,2.07,5.27,5.89,0.41,3.66,1.61,4.67,6.05,2.91,3.42,0.36,4.71,5.04,4.26,6.15,1.68,1.53,2.43,6.14,4.38,1.80,0.17,5.15,5.11,1.67,5.03,0.31,5.95,1.95,4.91,1.62,3.57,5.43,4.28,2.52,2.22,2.52,5.18,1.20,0.16,0.99,4.23,5.59,1.39,3.46,1.67,5.61,1.12,5.16,1.79,5.49,1.48,0.84,4.61,0.04,2.81,3.15,3.70,2.97,5.27,3.13,1.98,5.28,2.20,0.65,5.42,5.00,1.45,0.91,2.87,4.02,5.46,4.29,5.61,5.87,0.57,0.57,4.80,1.80,6.05,3.96,1.53,0.81,1.73,6.21,2.25,0.95,0.89,3.15,4.28,4.22,4.76,4.43};

float beegHueFromSmol(float hueSmall) {
  return hueSmall * 65535.0 / 360.0;
}

float fireflyActivation(float t, float phase = 0) {
  float E = (sin(t + phase) + cos(5.0*t + phase) - cos(10.0*t + phase) + sin(25.0*t + phase)) / 35.0;
  float x = 20.0 * sin(0.5 * t + phase) / 0.2;
  float fire = 2.0 / (1.0 + exp(x * x));
  float intensity = fire + (1 - fire) * E;

  return intensity;
}

uint32_t fireflyHue(float intensity) {
  float hueSmall = 63.0 * intensity + 262.0 * (1.0 - intensity);
  float beegHue = beegHueFromSmol(hueSmall);

  return uint32_t(beegHue);
}

float fireflyBrightness(float intensity) {
  return float(brightness) / (2.0 * (1.0 + exp(-8.0 * (intensity - 0.5)))) + 0.5;
}

uint32_t auroraHue(float t, float phase, float beginningHue, float endingHue) {
  float intensity = (sin(t + phase) + cos(5.0*t + phase) - cos(10.0*t + phase) + sin(25.0*t + phase)) / 7.05;
  float hueSmall = endingHue * intensity + beginningHue * (1.0 - intensity);
  float beegHue = beegHueFromSmol(hueSmall);

  return uint32_t(beegHue);
}

void init_strip() {
  strip.begin();
  strip.show();
  strip.setBrightness(50);
}

void initWatchdog() {
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  // Arduino-ESP32 3.x (IDF 5.x): the task WDT is already started by the core, so
  // reconfigure it rather than init it again.
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = WDT_TIMEOUT_S * 1000,
    .idle_core_mask = 0,
    .trigger_panic = true,
  };
  esp_task_wdt_reconfigure(&wdt_config);
#else
  esp_task_wdt_init(WDT_TIMEOUT_S, true);   // panic + reboot on timeout
#endif
  esp_task_wdt_add(NULL);                   // watch the loop task
}

void setup() {
  Serial.begin(115200);

  strip.begin();
  init_strip();

#if defined(NET_ENABLED)
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi up: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not up yet; will keep retrying in loop()");
  }

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setBufferSize(1024);      // room for the ~700-byte discovery payload
  client.setKeepAlive(60);
  client.setCallback(onCommand);
  // Do not block on MQTT here — ensureConnected() handles it while the strand animates.
#endif

  initWatchdog();
}

void effectLoop() {
  if (!isOn) {
    strip.fill(strip.Color(0, 0, 0));
    strip.show();
    return;
  }

  strip.setBrightness(brightness);

  // Each time-based effect wraps its phase clock at an integer number of ms equal
  // to (a close integer approximation of) its fundamental period. This keeps the
  // float phase small forever — no long-run precision loss — while wrapping
  // seamlessly, since every term's frequency is an integer multiple of the base.
  if (effect == "static") {
    strip.fill(strip.Color(r, g, b));
  } else if (effect == "rainbow") {
    const auto iterations = millis() % 128000;
    strip.rainbow(round(256.0 * iterations / 100.0));
  } else if (effect == "fireflies") {
    float pTime = (millis() % 100531UL) / 8000.0f;   // ~= 4*PI * 8000 ms
    for (int i = 0; i < LED_COUNT; ++i) {
      float intensity = fireflyActivation(pTime, randomPhases[i]);
      uint32_t pixelHue = fireflyHue(intensity);
      float fBrightness = fireflyBrightness(intensity);
      strip.setPixelColor(i, strip.ColorHSV(pixelHue, 255, round(fBrightness)));
    }
  } else if (effect == "aurora") {
    float pTime = (millis() % 25133UL) / 4000.0f;     // ~= 2*PI * 4000 ms
    for (int i = 0; i < LED_COUNT; ++i) {
      uint32_t pixelHue = auroraHue(pTime, randomPhases[i], 150.0, 220.0);
      strip.setPixelColor(i, strip.ColorHSV(pixelHue, 255, brightness));
    }
  } else if (effect == "auroraSettable") {
    // "Happy accident" aurora: a deliberately TIGHT ~70-degree hue band centered on
    // the current color's hue (kept live by the color-set path in onCommand). The
    // original code wrote `hue + deltaHue % 360` which, by operator precedence, is
    // `hue + 70` — a small delta that yields a calm band. The "corrected"
    // `(hue + deltaHue) % 360` instead spans ~290 degrees and looks garish, so the
    // accident is preserved on purpose. Intentionally NOT wrapped: for high base hues
    // the top of the band overflows 65535 and wraps through red in ColorHSV's 16-bit
    // space, giving a pleasant sunset edge. Do not "fix". See vault RGB to Hue gotcha.
    float pTime = (millis() % 25133UL) / 4000.0f;     // ~= 2*PI * 4000 ms
    const int deltaHue = 70;
    int startingHue = hue;
    int endingHue = hue + deltaHue;                   // intentional happy accident — do not wrap
    for (int i = 0; i < LED_COUNT; ++i) {
      uint32_t pixelHue = auroraHue(pTime, randomPhases[i], startingHue, endingHue);
      strip.setPixelColor(i, strip.ColorHSV(pixelHue, 255, brightness));
    }
  } else if (effect == "torchlight") {
    float pTime = (millis() % 18850UL) / 3000.0f;     // ~= 2*PI * 3000 ms
    for (int i = 0; i < LED_COUNT; ++i) {
      uint32_t pixelHue = auroraHue(pTime, randomPhases[i], 18.0, 39.0);
      strip.setPixelColor(i, strip.ColorHSV(pixelHue, 255, brightness));
    }
  }

  strip.show();
}

void loop() {
#if defined(NET_ENABLED)
  ensureConnected();
  client.loop();
#endif

  effectLoop();
  esp_task_wdt_reset();
}
