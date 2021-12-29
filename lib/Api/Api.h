#ifndef API_H
# define API_H

#include "Arduino.h"
#include <ESPAsyncWebServer.h>

typedef std::function<void()> ApiCallback;

enum AccessMode {
  R,
  W,
  RW,
};

class Api {
public:
  Api(AsyncWebServer&, char* channel);

  template<typename T>
  void add(char* key, T& value, AccessMode mode = RW);
  void on(char* event, ApiCallback callback);

private:
  AsyncWebServer& _server;
};

#endif
