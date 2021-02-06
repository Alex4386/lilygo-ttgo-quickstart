#include <FS.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <TFT_eSPI.h>

#include "screen.hpp"
#include "wifi.hpp"

void loadAPsFromFile(WiFiMulti *wifiMulti, fs::FS *fs, String configFile, TFT_eSPI *tft = nullptr) {
  fs::File wifiFile = fs->open(configFile);
  if (wifiFile == NULL) {
    throw "Invalid wifiFile";
  }

  String wifiConfig = wifiFile.readString();
  printLog(tft, info, "loaded "+String(configFile));
  
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, wifiConfig);

  JsonArray aps = doc["accessPoints"].as<JsonArray>();
  printLog(tft, info, "Detected "+String(aps.size())+" APs");

  for (int i = 0; i < aps.size(); i++) {
    const char *ssid = aps[i]["ssid"].as<char *>();
    const char *password = aps[i]["password"].as<char *>();

    wifiMulti->addAP(
      ssid,
      password
    );
  }
}

void waitForWiFi(WiFiMulti *wifiMulti, int delayMs) {
  while (wifiMulti->run() != WL_CONNECTED) {
    delay(delayMs);
  }

  syncClock();
}

void syncClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    yield();
    nowSecs = time(nullptr);
  }
}
