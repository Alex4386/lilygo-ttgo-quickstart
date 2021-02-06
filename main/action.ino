#include <Button2.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#include "action.hpp"
#include "screen.hpp"
#include "http.hpp"
#include "utils.hpp"


time_t prevTime = time(nullptr);

void getCurrentWiFiInfo(TFT_eSPI *tft, bool clear) {
  if (clear) clearScreenForCLI(tft);

  String currentSSID = WiFi.SSID();
  String currentIP = WiFi.localIP().toString();
  String gatewayIP = WiFi.gatewayIP().toString();
  String subnetMask = WiFi.subnetMask().toString();

  printLog(tft, info, "Connected to:");
  printLog(tft, info, "SSID : "+currentSSID);
  printLog(tft, info, "My IP: "+currentIP);
  printLog(tft, info, "GW IP: "+gatewayIP);
  printLog(tft, info, "SubNT: "+subnetMask);
  printLog(tft, info, "Time : "+getCurrentTime());
  printLog(tft, info, "RSSI : "+String(WiFi.RSSI())+"dBm");
}

void getCurrentTime(TFT_eSPI *tft, bool clear) {
  if (clear || time(nullptr) != prevTime) clearScreenForCLI(tft);
  prevTime = time(nullptr);

  tft->setTextSize(2);
  tft->println("Current Time:");
  tft->println(getCurrentTime());
  tft->setCursor(0, 0, 1);
}

void runHttpRequest(TFT_eSPI *tft, bool clear, HttpMethod method, String url, String payload, bool trimResult) {
  if (clear) clearScreenForCLI(tft);

  printLog(tft, info, getMethodString(method)+" "+url);

  HttpResponse *response;
  response = sendHttpRequest(method, url, payload, trimResult);
  printHttpResponse(tft, response);
  free(response);
}

void runCommsTest(TFT_eSPI *tft, bool clear) {
  if (clear) clearScreenForCLI(tft);

  printLog(tft, info, "Starting Comms Test");

  runHttpRequest(tft, false, GET, "http://icanhazip.com/");
  runHttpRequest(tft, false, GET, "https://meiling.stella-api.dev/");
  
  printLog(tft, ok, "Comms Test Complete!");
}