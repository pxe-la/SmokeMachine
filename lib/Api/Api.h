#ifndef API_H
# define API_H

#include "Arduino.h"
#include <ESPAsyncWebServer.h>

typedef std::function<void()> ApiCallback;

class Api {
public:
  Api(AsyncWebServer&, char* channel);

  template<typename T>
  void add(char* key, T& value, AccessMode mode = RW);
  void on(char* event, ApiCallback callback);

private:
  AsyncWebServer& _server;
};


enum Type {
  TYPE_FINISH,
  TYPE_STR,
  TYPE_INT,
  TYPE_BOOL,
  TYPE_FN_STR,
  TYPE_FN_INT,
  TYPE_FN_BOOL,
};

enum AccessMode {
  R,
  W,
  RW,
};

typedef std::function<void()> ApiCallback;

struct Prop {
  const char* key;
  void* value;
  Type type;
  AccessMode accessMode;
};

void addApiMethod(const char* url, Prop* props) {
  server.on(url, HTTP_GET, [props](AsyncWebServerRequest * request) {
    String responceJson = "{";
    for (uint8_t i = 0; props[i].type != FINISH; i++) {
      if (props[i].accessMode == W) continue;
      responceJson += '"';
      responceJson += props[i].key;
      responceJson += "\":";
      switch (props[i].type) {
        case TYPE_STR:
          responceJson += '"';
          responceJson += (char*)(props[i].value);
          responceJson += '"';
          break;
        case TYPE_INT:
          responceJson += (int)(props[i].value);
          break;
        case TYPE_BOOL:
          responceJson += ((bool)(props[i].value) ? "true" : "false");
          break;
        case TYPE_FN_STR:
          responceJson += '"';
          responceJson += ((char*(*)())(props[i].value))();
          responceJson += '"';
          break;
        case TYPE_FN_INT:
          responceJson += ((int(*)())(props[i].value))();
          break;
        case TYPE_FN_BOOL:
          responceJson += ((bool(*)())(props[i].value))() ? "true" : "false";
          break;
      }
      responceJson += ',';
    }
    responceJson[responceJson.length() - 1] = '}';
    request->send(200, applicationJson, responceJson);
  });
  server.on(url, HTTP_POST, [props](AsyncWebServerRequest * request) {
    for (uint8_t i = 0; props[i].type != FINISH; i++) {
      if (props[i].accessMode == R) continue;
      if (request->hasParam(props[i].key, true)) {
        String param = request->getParam(props[i].key, true)->value();
        switch (props[i].type) {
          case TYPE_STR:
            strlcpy((char*)props[i].value, param.c_str(), 32);
            break;
          case TYPE_INT:
            *((int*)(props[i].value)) = param.toInt();
            break;
          case TYPE_BOOL:
            *((bool*)(props[i].value)) = param == "true";
            break;
        }
      }
    }
  });
}

#endif
