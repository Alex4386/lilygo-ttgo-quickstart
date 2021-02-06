#include <FS.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>

#ifndef _WIFI_HPP

#define _WIFI_HPP
void loadAPsFromFile(WiFiMulti *wifiMulti, fs::FS *fs, String configFile, TFT_eSPI *tft);

void waitForWiFi(WiFiMulti *wifiMulti, int delayMs = 1000);

void syncClock();
#endif