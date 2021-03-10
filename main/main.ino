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
#include "meiling.hpp"

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
  clearScreenForCLI(&tft);

  MeilingDeviceCodeResult *res = loadMeilingTokens(&SPIFFS);

  if (res == nullptr) {
    Serial.println("Reauth required. Triggering abort()");

    MeilingDeviceCodeRequestResponse *codeReq = runMeilingDevice(&SPIFFS, &tft, false);
    clearScreenForCLI(&tft);
  
    while (res == nullptr) {
      clearScreenForCLI(&tft);
      
      tft.println();
      tft.setTextFont(2);
      tft.setTextSize(2);
    
      tft.print(*codeReq->user_code);
      tft.println();
    
      tft.setTextSize(1);
      tft.setTextFont(1);
      tft.println(*codeReq->verification_url);
      delay((codeReq->interval) * 1000);

      tft.println("Polling");
      res = pollServer(&SPIFFS, &tft, codeReq, false);
    }
  } else {
    Serial.println("Previous session was found.");

    if (isAccessTokenExpired(res)) {
      bool updateResult = updateMeilingTokens(&SPIFFS, &tft, res, false);
      if (!updateResult) {
        Serial.println("Reauth required. Triggering abort()");
        deleteMeilingTokens(&SPIFFS);
        throw "Reauth Required";
      }
    }
  }

  saveMeilingTokens(&SPIFFS, res);
  clearScreenForCLI(&tft);

  printLog(&tft, info, "Access Token: "+(*res->access_token));
  printLog(&tft, info, "Refresh Token: "+(*res->refresh_token));

  delay(1000);

  HttpResponse *consoleRes = sendMeilingAPIRequest(
    res,
    GET,
    "https://meiling.stella-api.dev/v1/oauth2/userinfo",
    "", "", false
  );

  printHttpResponse(&tft, consoleRes);

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

}
