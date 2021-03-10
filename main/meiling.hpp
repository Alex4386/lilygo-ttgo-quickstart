#include <FS.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#ifndef _MEILING_HPP

#define _MEILING_HPP

typedef struct MeilingDeviceCodeRequestResponse {
  String *device_code;
  int interval;
  String *user_code;
  String *verification_url;
} MeilingDeviceCodeRequestResponse;

typedef struct MeilingDeviceCodeResult {
  String *access_token;
  String *refresh_token;
  time_t expires_at;
} MeilingDeviceCodeResult;

DynamicJsonDocument getMeilingJson(fs::FS *filesystem);
String getClientId(fs::FS *filesystem);
String getScope(fs::FS *filesystem);
MeilingDeviceCodeRequestResponse *runMeilingDevice(fs::FS *filesystem, TFT_eSPI *tft, bool clear);
MeilingDeviceCodeResult *pollServer(fs::FS *filesystem, TFT_eSPI *tft, MeilingDeviceCodeRequestResponse *resp, bool clear);
bool updateMeilingTokens(fs::FS *filesystem, TFT_eSPI *tft, MeilingDeviceCodeResult *res, bool clear);
bool isAccessTokenExpired(MeilingDeviceCodeResult *res);
MeilingDeviceCodeResult *loadMeilingTokens(fs::FS *filesystem);
void saveMeilingTokens(fs::FS *filesystem, MeilingDeviceCodeResult *res);
void deleteMeilingTokens(fs::FS *filesystem);

HttpResponse *sendMeilingAPIRequest(MeilingDeviceCodeResult *res, HttpMethod method, String url, String payload, String contentType, bool trimResult);

#endif
