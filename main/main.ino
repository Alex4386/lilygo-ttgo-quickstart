#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include "SPIFFS.h" // ESP32 only
#include "screen.hpp"
#include "utils.hpp"

TFT_eSPI tft;
WiFiMulti wifiMulti;

String ssid = "ssid";
String pass = "password";

void setup() {
  Serial.begin(115200);
  TJpgDec.setCallback(tJpgTFTHandler);

  initScreenForCLI(&tft, 90);

  SPIFFS.begin();
  fs::File root = SPIFFS.open("/");

	showSplash();
  delay(500);

  tft.fillScreen(TFT_BLACK);

  wifiMulti.addAP(
    ssid.c_str(),
    pass.c_str()
  );

  printLog(&tft, info, "Trying to connect: "+ssid);
  while (wifiMulti.run() != WL_CONNECTED) {}

  printLog(&tft, info, "Synchronizing Clock");
  syncClock();

  String currentTime = String(getCurrentTime());
  printLog(&tft, info, "Time: "+currentTime);

  String currentSSID = WiFi.SSID();
  String currentIP = WiFi.localIP().toString();

  printLog(&tft, info, "Connected to "+currentSSID);
  printLog(&tft, info, "IP: "+currentIP);

  HttpResponse *response;
  response = sendHttpRequest(GET, "http://icanhazip.com/", "");
  printLog(&tft, info, response->response);
  free(response);

  response = sendHttpRequest(GET, "https://meiling.stella-api.dev/", "");
  printLog(&tft, info, response->response);
  free(response);
}

bool tJpgTFTHandler(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void loop() {}
