#ifndef _UTILS_HPP

#define _UTILS_HPP

enum HttpMethod { GET, POST, PATCH, PUT };

struct HttpResponse {
  int code;
  String response;
};

void syncClock();
char *getCurrentTime();

HttpResponse *sendHttpRequest(HttpMethod method, String url, String payload);
HttpResponse *sendHttpRequest(HttpMethod method, String url);

#endif