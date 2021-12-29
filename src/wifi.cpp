#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <ArtnetWifi.h>
#include "wifi.h"

static const char* HOSTNAME = "smoke-machine";
static const char* OTA_PASS = "egziifxn";
static const uint16_t ARTNET_RUN_TIMEOUT = 2000;

static ArtnetWifi artnet;
static AsyncWebServer   server(80);

static void initWifi() {
  SPIFFS.begin();

  WiFi.hostname(HOSTNAME);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IPAddress( 4, 3, 2, 1 ), IPAddress( 4, 3, 2, 1 ), IPAddress( 255, 255, 255, 0 ));
  WiFi.softAP(config->ap_ssid, config->ap_pass);

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


  server.serveStatic("/", SPIFFS, "/");

  server.on("/api/state", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json",
      String("{\"temp\":") + Machine::getTemp() +
      ",\"stickyTime\":" + config->stickyTime +
      ",\"recStickyTime\":" + Machine::recordStickyTime +
      "}"
    );
  });

  server.on("/api/state", HTTP_POST, [](AsyncWebServerRequest * request) {
    bool saveFlag = false;
    auto param = F("stickyTime");
    if (request->hasParam(param, true)) {
      config->stickyTime = request->getParam(param, true)->value().toInt();
      saveFlag = true;
    }
    param = F("recStickyTime");
    if (request->hasParam(param, true)) {
      Machine::recordStickyTime = request->getParam(param, true)->value() == "true";
    }
    param = F("artNetUniverse");
    if (request->hasParam(param, true)) {
      saveFlag = true;
      config->artNetUniverse = request->getParam(param, true)->value().toInt();
    }
    param = F("artNetChannel");
    if (request->hasParam(param, true)) {
      saveFlag = true;
      config->artNetChannel = request->getParam(param, true)->value().toInt();
    }
    param = F("autoRunnerInterval");
    if (request->hasParam(param, true)) {
      saveFlag = true;
      config->autoRunnerInterval = request->getParam(param, true)->value().toInt();
    }
    param = F("autoRunnerDuration");
    if (request->hasParam(param, true)) {
      saveFlag = true;
      config->autoRunnerDuration = request->getParam(param, true)->value().toInt();
    }
    param = F("autoRunner");
    if (request->hasParam(param, true)) {
      Machine::autoRunner = request->getParam(param, true)->value() == "true";
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

  server.on("/api/ap", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String newApSsid = request->getParam("ssid", true)->value();
      String newApPass = request->getParam("pass", true)->value();
      strcpy(config->ap_ssid, newApSsid.c_str());
      strcpy(config->ap_pass, newApPass.c_str());
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
