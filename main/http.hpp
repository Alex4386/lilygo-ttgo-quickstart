#include <TFT_eSPI.h>

#ifndef _HTTP_HPP

#define _HTTP_HPP

enum HttpMethod { GET, POST, PATCH, PUT };

struct HttpResponse {
  int code;
  String response;
};

HttpResponse *sendHttpRequest(HttpMethod method, String url, String payload, String contentType = "application/json", bool trimResult = false);
HttpResponse *sendHttpRequest(HttpMethod method, String url);
void printHttpResponse(TFT_eSPI *tft, HttpResponse *response);

String getMethodString(HttpMethod method);

#endif
