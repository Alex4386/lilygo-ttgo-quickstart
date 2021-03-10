#include <FS.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include "screen.hpp"
#include "http.hpp"
#include "utils.hpp"
#include "urlencode.hpp"
#include "meiling.hpp"


DynamicJsonDocument getMeilingJson(fs::FS *filesystem) {
  fs::File meilingFile = filesystem->open("/meiling/config.json");
  if (meilingFile == NULL) {
    throw "Invalid meilingFile";
  }

  String meilingFileRaw = meilingFile.readString();
  DynamicJsonDocument doc(1024);

  deserializeJson(doc, meilingFileRaw);
  return doc;
}

String getClientId(fs::FS *filesystem) {
  DynamicJsonDocument doc = getMeilingJson(filesystem);
  String clientId = doc["clientId"].as<String>();

  return clientId;
}

String getScope(fs::FS *filesystem) {
  DynamicJsonDocument doc = getMeilingJson(filesystem);
  String clientId = doc["scope"].as<String>();

  return clientId;
}

MeilingDeviceCodeRequestResponse *runMeilingDevice(fs::FS *filesystem, TFT_eSPI *tft, bool clear) {
  String deviceCodeQuery = "client_id="+urlencode(getClientId(filesystem))+"&scope="+urlencode(getScope(filesystem));
  
  HttpResponse *response;
  response = sendHttpRequest(
    POST, 
    "https://meiling.stella-api.dev/v1/oauth2/device/code", 
    deviceCodeQuery, 
    "application/x-www-form-urlencoded",
    false
  );

  printHttpResponse(tft, response);

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, response->response);

  String device_code = doc["device_code"].as<String>();
  int interval = doc["interval"].as<int>();
  String user_code = doc["user_code"].as<String>();
  String verification_url = doc["verification_url"].as<String>();

  MeilingDeviceCodeRequestResponse *resp = (MeilingDeviceCodeRequestResponse *)malloc(sizeof(MeilingDeviceCodeRequestResponse));

  resp->device_code = new String(device_code);
  resp->interval = interval;
  resp->user_code = new String(user_code);
  resp->verification_url = new String(verification_url);

  return resp;
}

MeilingDeviceCodeResult *pollServer(fs::FS *filesystem, TFT_eSPI *tft, MeilingDeviceCodeRequestResponse *resp, bool clear) {
  HttpResponse *response;
  time_t now = time(nullptr);

  response = sendHttpRequest(
    POST, 
    "https://meiling.stella-api.dev/v1/oauth2/token", 
    "client_id="+urlencode(getClientId(filesystem))+"&device_code="+urlencode(*(resp->device_code))+"&grant_type="+urlencode("urn:ietf:params:oauth:grant-type:device_code"), 
    "application/x-www-form-urlencoded",
    false
  );

  if (response->code == 428) {
    return nullptr;
  } else if (response->code == 400) {
    throw "Invalid Device Code";
  }

  MeilingDeviceCodeResult *res = (MeilingDeviceCodeResult *)malloc(sizeof(MeilingDeviceCodeResult));

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, response->response);

  res->access_token = new String(doc["access_token"].as<String>());
  res->refresh_token = new String(doc["refresh_token"].as<String>());
  res->expires_at = now + doc["expires_in"].as<int>();

  return res;
}

bool updateMeilingTokens(fs::FS *filesystem, TFT_eSPI *tft, MeilingDeviceCodeResult *res, bool clear) {
  HttpResponse *response;
  time_t now = time(nullptr);

  response = sendHttpRequest(
    POST, 
    "https://meiling.stella-api.dev/v1/oauth2/token", 
    "client_id="+urlencode(getClientId(filesystem))+"&refresh_token="+urlencode(*(res->refresh_token))+"&grant_type=refresh_token", 
    "application/x-www-form-urlencoded",
    false
  );

  if (response->code == 403) {
    return false;
  }

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, response->response);

  free(res->access_token);

  res->access_token = new String(doc["access_token"].as<String>());
  res->expires_at = now + doc["expires_in"].as<int>();
}

bool isAccessTokenExpired(MeilingDeviceCodeResult *res) {
  time_t now = time(nullptr);

  return now > res->expires_at;
}

String meilingTokenLocation = "/meiling/token.json";


MeilingDeviceCodeResult *loadMeilingTokens(fs::FS *filesystem) {
  fs::File file = filesystem->open(meilingTokenLocation, "r");
  if (file == NULL) {
    return nullptr;
  }

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, file.readString());

  if (doc.isNull()) {
    return nullptr;
  }

  MeilingDeviceCodeResult *res = (MeilingDeviceCodeResult *)malloc(sizeof(MeilingDeviceCodeResult));

  res->access_token = new String(doc["access_token"].as<String>());
  res->refresh_token = new String(doc["refresh_token"].as<String>());
  res->expires_at = (time_t) doc["expires_at"].as<int>();
  return res;
}

void saveMeilingTokens(fs::FS *filesystem, MeilingDeviceCodeResult *res) {
  fs::File file = filesystem->open(meilingTokenLocation, "w+");

  DynamicJsonDocument doc(2048);
  doc["access_token"] = *(res->access_token);
  doc["refresh_token"] = *(res->refresh_token);
  doc["expires_at"] = res->expires_at;

  String json = "";
  serializeJson(doc, file);
  file.close();
  return;
}

void deleteMeilingTokens(fs::FS *filesystem) {
  filesystem->remove(meilingTokenLocation);
}

HttpResponse *sendMeilingAPIRequest(MeilingDeviceCodeResult *res, HttpMethod method, String url, String payload, String contentType, bool trimResult) {
  HTTPClient http;

  bool success = false;
  String methodString = getMethodString(method);
  
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

  http.addHeader("Authorization", "Bearer "+(*res->access_token));
  http.addHeader("Content-Type", contentType);

  if (success) {
    int httpCode;

    switch (method) {
      case GET:
        httpCode = http.GET();
        break;
      case POST:
        httpCode = http.POST(payload);
        break;
      case PATCH:
        httpCode = http.PATCH(payload);
        break;
      case PUT:
        httpCode = http.PUT(payload);
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

