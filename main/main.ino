#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>

#include "SPIFFS.h" // ESP32 only
#include "screen.hpp"
#include "wifi.hpp"
#include "http.hpp"
#include "utils.hpp"

TFT_eSPI tft;
WiFiMulti wifiMulti;

// change this to /wifi/wifi.json
String wifiConfigFile = "/wifi/personal.json";

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();

  initScreenForCLI(&tft, 90);

  showSplash(SPIFFS, pJpgCallback);
  clearScreenForCLI(&tft);

  loadAPsFromFile(&wifiMulti, &SPIFFS, wifiConfigFile, &tft);

  getCurrentWiFiInfo();
  delay(1000);

  runCommsTest();
}

void getCurrentWiFiInfo() {
  clearScreenForCLI(&tft);

  printLog(&tft, info, "Connecting to WiFi");
  waitForWiFi(&wifiMulti);

  String currentSSID = WiFi.SSID();
  String currentIP = WiFi.localIP().toString();

  printLog(&tft, ok, "Connected to "+currentSSID);
  printLog(&tft, info, "IP: "+currentIP);
  printLog(&tft, info, "Time: "+getCurrentTime());
}

void runCommsTest() {
  clearScreenForCLI(&tft);

  printLog(&tft, info, "Starting Comms Test");

  HttpResponse *response;
  response = sendHttpRequest(GET, "http://icanhazip.com/", "", true);
  printHttpResponse(&tft, response);
  free(response);

  response = sendHttpRequest(GET, "https://meiling.stella-api.dev/", "");
  printHttpResponse(&tft, response);
  free(response);

  printLog(&tft, ok, "Comms Test Complete!");
}

void loop() {}

bool pJpgCallback(
  int16_t x,
  int16_t y,
  uint16_t w,
  uint16_t h,
  uint16_t* bitmap
) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

