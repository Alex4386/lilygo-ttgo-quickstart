#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <Button2.h>

#include "SPIFFS.h" // ESP32 only

#include "action.hpp"
#include "screen.hpp"
#include "wifi.hpp"
#include "http.hpp"
#include "utils.hpp"

TFT_eSPI tft;
WiFiMulti wifiMulti;

Button2 leftButton = Button2(0);
Button2 rightButton = Button2(35);

// change this to /wifi/wifi.json
String wifiConfigFile = "/wifi/personal.json";

enum IdleMode { NONE, CLOCK };
IdleMode idleMode = NONE;

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();

  initScreenForCLI(&tft, 90);

  showSplash(SPIFFS, pJpgCallback);
  clearScreenForCLI(&tft);

  loadAPsFromFile(&wifiMulti, &SPIFFS, wifiConfigFile, &tft);

  clearScreenForCLI(&tft);
  printLog(&tft, info, "Connecting to WiFi");
  waitForWiFi(&wifiMulti);

  getCurrentWiFiInfo(&tft, false);
  delay(1000);

  runCommsTest(&tft);

  leftButton.setClickHandler(shortBtnEvent);
  rightButton.setClickHandler(shortBtnEvent);
  leftButton.setLongClickDetectedHandler(longBtnEvent);
  rightButton.setLongClickDetectedHandler(longBtnEvent);
  leftButton.setDoubleClickHandler(doubleBtnEvent);
  rightButton.setDoubleClickHandler(doubleBtnEvent);
}

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

void loop() {
  leftButton.loop();
  rightButton.loop();

  if (idleMode == CLOCK) {
    getCurrentTime(&tft, false);
  }
}

// ===================================

void shortBtnEvent(Button2 &btn) {
  getCurrentTime(&tft);
  idleMode = CLOCK;
}

void longBtnEvent(Button2 &btn) {
  if (btn == leftButton) {
    getCurrentWiFiInfo(&tft);
  } else if (btn == rightButton) {
    runHttpRequest(&tft, true, GET, "https://meiling.stella-api.dev/v1");
  }
  idleMode = NONE;
}

void doubleBtnEvent(Button2 &btn) {
  if (btn == leftButton) {
    getCurrentWiFiInfo(&tft);
  } else if (btn == rightButton) {
    runCommsTest(&tft);
  }
  idleMode = NONE;
}
