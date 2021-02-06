#include <Button2.h>
#include "http.hpp"

#ifndef _ACTION_HPP

#define _ACTION_HPP
void getCurrentWiFiInfo(TFT_eSPI *tft, bool clear = true);
void getCurrentTime(TFT_eSPI *tft, bool clear = true);
void runCommsTest(TFT_eSPI *tft, bool clear = true);

void runHttpRequest(TFT_eSPI *tft, bool clear, HttpMethod method, String url, String payload = "", bool trimResult = false);

#endif