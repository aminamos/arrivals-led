#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// ---- Config ----
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASSWORD"
#define STOP_URL "https://svc.metrotransit.org/nextripv2/17941"
#define POLL_MS 30000

// 2x 64x32 P2.5 chained horizontally = 128x32
#define PANEL_W 128
#define PANEL_H 32

MatrixPanel_I2S_DMA *dma = nullptr;
String row1 = "E NB  --";
String row2 = "E NB  --";
unsigned long lastPoll = 0;

// "Due" -> "DUE", "5 Min" -> "5m", "6:15" stays
String shortTime(const String &t) {
  if (t.equalsIgnoreCase("Due")) return "DUE";
  int sp = t.indexOf(' ');
  if (sp > 0) return t.substring(0, sp) + "m";
  return t;
}

void draw() {
  dma->fillScreen(0);
  dma->setTextSize(1);
  dma->setTextWrap(false);
  dma->setTextColor(dma->color565(255, 180, 0));
  dma->setCursor(2, 4);
  dma->print("E NB  " + shortTime(row1));
  dma->setTextColor(dma->color565(0, 220, 255));
  dma->setCursor(2, 20);
  dma->print("E NB  " + shortTime(row2));
}

void poll() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.setTimeout(8000);
  http.begin(STOP_URL);
  int code = http.GET();
  if (code != 200) { http.end(); return; }
  String body = http.getString();
  http.end();

  JsonDocument doc;
  if (deserializeJson(doc, body)) return;
  JsonArray deps = doc["departures"].as<JsonArray>();
  String e[2]; int n = 0;
  for (JsonObject d : deps) {
    if (n >= 2) break;
    if (String((const char*)d["route_id"]) == "925")
      e[n++] = String((const char*)d["departure_text"]);
  }
  if (n > 0) row1 = e[0];
  if (n > 1) row2 = e[1];
  draw();
}

void setup() {
  HUB75_I2S_CFG mx(PANEL_W, PANEL_H, 1);
  dma = new MatrixPanel_I2S_DMA(mx);
  dma->begin();
  dma->setBrightness8(160);
  draw();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) delay(250);
  poll();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { WiFi.reconnect(); delay(2000); return; }
  if (millis() - lastPoll > POLL_MS) { lastPoll = millis(); poll(); }
  delay(500);
}
