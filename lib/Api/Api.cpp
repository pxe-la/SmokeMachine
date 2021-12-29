#include "Api.h"

Api::Api(AsyncWebServer& server) : _server(server) {

}

void Api::add(char* name, KeyValue* props, ApiCallback callback) {
  _server.on(name, HTTP_POST, [props, callback](AsyncWebServerRequest* request) {
    for (KeyValue* _props = props; _props != 0; _props++) {
      if (request->hasParam(props->key, true)) {
        String param = request->getParam(props->key, true)->value();
        switch (props->type) {
          case TYPE_STR_32:
            strlcpy((char*)props->value, param.c_str(), 32);
            break;
          case TYPE_INT:
            *(int*)props->value = param.toInt();
            break;
        }
      }
    }
    if (callback)
      callback();
    request->send(200);
  });

  _server.on(name, HTTP_GET, [props, callback](AsyncWebServerRequest* request) {
    for (KeyValue* _props = props; _props != 0; _props++) {
      if (request->hasParam(props->key)) {

      }
    }
  });
}

api.add("wifiAp", {
  {"ssid", config->ApSsid},
  {"pass", config->ApPass}
});
api.add("wifi", {
  {"ssid", config->}
})
