#include "SPIFFS.h" // ESP32 only
#include "utils.hpp"

String getCurrentTime() {
  time_t nowSecs = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  String currentTime = String(asctime(&timeinfo));
  currentTime.trim();
  return currentTime;
}
