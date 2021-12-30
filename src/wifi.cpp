#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <ArtnetWifi.h>
#include "wifi.h"

static const char*    HOSTNAME = "smoke-machine";
static const char*    OTA_PASS = "egziifxn";
static const uint16_t ARTNET_RUN_TIMEOUT = 2000;

static const char* applicationJson = "application/json";

static ArtnetWifi artnet;
static AsyncWebServer server(80);

static void initWifi() {
  SPIFFS.begin();

  WiFi.hostname(HOSTNAME);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress( 4, 3, 2, 1 ), IPAddress( 4, 3, 2, 1 ), IPAddress( 255, 255, 255, 0 ));
  WiFi.softAP(config->apSsid, config->apPass);

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(config->ssid, config->pass);

  MDNS.begin(HOSTNAME);
  MDNS.addService("http", "tcp", 80);

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA.begin();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  static bool prevRun = false;

  if (universe != config->artNetUniverse || config->artNetChannel > length) return;

  if (data[config->artNetChannel]) Machine::run(ARTNET_RUN_TIMEOUT);
  else if (prevRun) Machine::run(0);

  prevRun = data[config->artNetChannel];
}

void Wifi::init() {
  initWifi();
  artnet.begin();
  artnet.setArtDmxCallback(onDmxFrame);


  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");;

  server.on("/api/state", HTTP_GET, [](AsyncWebServerRequest * request) {
    static String stringBuf;
    stringBuf.reserve(90);
    stringBuf = F("{\"temp\":");
    stringBuf += Machine::getTemp();
    stringBuf += F(",\"autoRunner\":");
    stringBuf += (Machine::autoRunner ? F("true") : F("false"));
    stringBuf += F(",\"stickyTime\":");
    stringBuf += config->stickyTime;
    stringBuf += '}';
    request->send(200, applicationJson, stringBuf);
  });

  server.on("/api/stickyTime", HTTP_GET, [](AsyncWebServerRequest * request) {
    static String stringBuf;
    stringBuf.reserve(70);
    stringBuf = F("{\"stickyTime\":");
    stringBuf += config->stickyTime;
    stringBuf += F(",\"recStickyTime\":");
    stringBuf += Machine::recordStickyTime ? F("true") : F("false");
    stringBuf += '}';
    request->send(200, applicationJson, stringBuf);
  });
  server.on("/api/stickyTime", HTTP_POST, [](AsyncWebServerRequest * request) {
    auto param = F("stickyTime");
    if (request->hasParam(param, true)) {
      config->stickyTime = request->getParam(param, true)->value().toInt();
      config.save();
    }
    param = F("recStickyTime");
    if (request->hasParam(param, true)) {
      Machine::recordStickyTime = request->getParam(param, true)->value() == "true";
    }
    request->send(200);
  });

  server.on("/api/autoRunner", HTTP_GET, [](AsyncWebServerRequest * request) {
    static String stringBuf;
    stringBuf.reserve(70);
    stringBuf = F("{\"enabled\":");
    stringBuf += (Machine::autoRunner ? F("true") : F("false"));
    stringBuf += F(",\"interval\":");
    stringBuf += config->autoRunnerInterval;
    stringBuf += F(",\"duration\":");
    stringBuf +=config->autoRunnerDuration;
    stringBuf += '}';

    request->send(200, applicationJson, stringBuf);
  });
  server.on("/api/autoRunner", HTTP_POST, [](AsyncWebServerRequest * request) {
    bool saveFlag = false;
    auto param = F("enabled");
    if (request->hasParam(param, true)) {
      Machine::autoRunner = request->getParam(param, true)->value() == "true";
    }
    param = F("interval");
    if (request->hasParam(param, true)) {
      saveFlag = true;
      config->autoRunnerInterval = request->getParam(param, true)->value().toInt();
    }
    param = F("duration");
    if (request->hasParam(param, true)) {
      saveFlag = true;
      config->autoRunnerDuration = request->getParam(param, true)->value().toInt();
    }
    if (saveFlag) config.save();
    request->send(200);
  });

  server.on("/api/run", HTTP_POST, [](AsyncWebServerRequest * request) {
    int duration = 0;
    if (request->hasParam("dur", true)) {
      duration = request->getParam("dur", true)->value().toInt();
    }
    Machine::run(duration);
    request->send(200);
  });

  server.on("/api/artnet", HTTP_GET, [](AsyncWebServerRequest * request) {
    static String stringBuf;
    stringBuf.reserve(50);
    stringBuf = F("{\"universe\":");
    stringBuf += config->artNetUniverse;
    stringBuf += F(",\"channel\":");
    stringBuf += config->artNetChannel;
    stringBuf += '}';
    request->send(200, applicationJson, stringBuf);
  });
  server.on("/api/artnet", HTTP_POST, [](AsyncWebServerRequest * request) {
    auto param = F("artNetUniverse");
    if (request->hasParam(param, true)) {
      config->artNetUniverse = request->getParam(param, true)->value().toInt();
    }
    param = F("artNetChannel");
    if (request->hasParam(param, true)) {
      config->artNetChannel = request->getParam(param, true)->value().toInt();
    }
    config.save();
  });

  server.on("/api/wifi", HTTP_GET, [](AsyncWebServerRequest * request) {
    static String stringBuf;
    stringBuf.reserve(50);
    stringBuf = F("{\"ssid\":\"");
    stringBuf += config->ssid;
    stringBuf += "\"}";
    request->send(200, applicationJson, stringBuf);
  });
  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String newSsid = request->getParam("ssid", true)->value();
      String newPass = request->getParam("pass", true)->value();

      strcpy(config->ssid, newSsid.c_str());
      strcpy(config->pass, newPass.c_str());
      config.save();
      ESP.restart();
    }
    request->send(200);
  });

  server.on("/api/ap", HTTP_GET, [](AsyncWebServerRequest * request) {
    static String stringBuf;
    stringBuf.reserve(50);
    stringBuf = F("{\"ssid\":\"");
    stringBuf += config->apSsid;
    stringBuf += "\"}";
    request->send(200, applicationJson, stringBuf);
  });
  server.on("/api/ap", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String newApSsid = request->getParam("ssid", true)->value();
      String newApPass = request->getParam("pass", true)->value();
      strcpy(config->apSsid, newApSsid.c_str());
      strcpy(config->apPass, newApPass.c_str());
      config.save();
      ESP.restart();
    }
    request->send(200);
  });

  server.begin();
}

void Wifi::handle() {
  MDNS.update();
  ArduinoOTA.handle();
  artnet.read();
}

IPAddress Wifi::getIp() {
  return WiFi.localIP();
}
