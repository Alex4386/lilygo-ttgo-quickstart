#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "http.hpp"
#include "screen.hpp"

HttpResponse *sendHttpRequest(HttpMethod method, String url, String payload, bool trimResult) {
  HTTPClient http;

  bool success = false;
  String methodString;
  
  if (url.startsWith("https")) {
    WiFiClientSecure *client = new WiFiClientSecure;

    // TODO: Implement certstore.
    // Supported from 1.0.5-rc. 
    // Comment out if you are in older versions.
    client->setInsecure();

    success = http.begin(*client, url);
  } else {
    success = http.begin(url);
  }

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  if (!doc.isNull()) {
    http.addHeader("Content-Type", "application/json");
  }

  if (success) {
    int httpCode;

    switch (method) {
      case GET:
        httpCode = http.GET();
        methodString = "GET";
        break;
      case POST:
        httpCode = http.POST(payload);
        methodString = "POST";
        break;
      case PATCH:
        httpCode = http.PATCH(payload);
        methodString = "PATCH";
        break;
      case PUT:
        httpCode = http.PUT(payload);
        methodString = "PUT";
        break;
    }

    HttpResponse *response = new HttpResponse;
    response->code = httpCode;
    response->response = http.getString();

    http.end();

    if (trimResult) {
      response->response.trim();
    }

    return response;
  }
  return nullptr;
}

HttpResponse *sendHttpRequest(HttpMethod method, String url) {
  return sendHttpRequest(method, url.c_str(), "");
}

void printHttpResponse(TFT_eSPI *tft, HttpResponse *response) {
  if (!response) {
    printLog(tft, error, "Failed Request");
    return;
  }

  LogLevel level = (100 <= response->code && response->code < 300) ? ok : error;
  printLog(tft, level, response->response);
}
