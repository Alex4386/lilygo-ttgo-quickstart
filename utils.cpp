#include <FS.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "SPIFFS.h" // ESP32 only
#include "utils.hpp"

void syncClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    yield();
    nowSecs = time(nullptr);
  }

  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
}

char *getCurrentTime() {
  time_t nowSecs = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  return asctime(&timeinfo);
}

String readFile(const char * path){
  String data;

  fs::File file = SPIFFS.open(path);
  if(!file || file.isDirectory()){
      return "";
  }

  while(file.available()){
    data.concat(file.readString());
  }

  file.close();
}

HttpResponse *sendHttpRequest(HttpMethod method, String url, String payload) {
  HTTPClient http;

  bool success = false;
  
  if (url.startsWith("https")) {
    WiFiClientSecure *client = new WiFiClientSecure;

    // TODO: Implement certstore.
    client->setInsecure();
    success = http.begin(*client, url);
  } else {
    success = http.begin(url);
  }

  if (success) {
    int httpCode;

    switch (method) {
      case GET:
        httpCode = http.GET();
      case POST:
        httpCode = http.POST(payload);
      case PATCH:
        httpCode = http.PATCH(payload);
      case PUT:
        httpCode = http.PUT(payload);
    }


    Serial.println(url);
    Serial.println("Request retrieve Success.");

    HttpResponse *response = new HttpResponse;
    response->code = httpCode;
    response->response = http.getString();

    Serial.println(response->code);
    Serial.println(response->response);

    http.end();

    return response;
  }

  Serial.println(url);
  Serial.println("Request begin failed.");

  return nullptr;
}

HttpResponse *sendHttpRequest(HttpMethod method, String url) {
  return sendHttpRequest(method, url.c_str(), "");
}
