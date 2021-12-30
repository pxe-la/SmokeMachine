#include "globals.h"

void setup() {
  ESP.wdtEnable(1000);
  initGlobals();
  Serial.println("Starting");

/*
str32_t   ssid = {0};
str32_t   pass = {0};
str32_t   apSsid = {0};
str32_t   apPass = {0};

uint16_t  stickyTime = 0;

uint32_t  autoRunnerInterval;
uint16_t  autoRunnerDuration;

uint32_t  radioCodeShort;
uint32_t  radioCodeSticky;

uint8_t   artNetUniverse;
uint16_t  artNetChannel;

bool      resetToDefaults;
*/
  Serial.print("ssid: "); Serial.println(config->ssid);
  Serial.print("pass: "); Serial.println(config->pass);
  Serial.print("apSsid: "); Serial.println(config->apSsid);
  Serial.print("apPass: "); Serial.println(config->apPass);
  Serial.print("stickyTime: "); Serial.println(config->stickyTime);
  Serial.print("autoRunnerInterval: "); Serial.println(config->autoRunnerInterval);
  Serial.print("autoRunnerDuration: "); Serial.println(config->autoRunnerDuration);
  Serial.print("autoRunnerDuration: "); Serial.println(config->autoRunnerDuration);
  Serial.print("radioCodeShort: "); Serial.println(config->radioCodeShort);
  Serial.print("radioCodeSticky: "); Serial.println(config->radioCodeSticky);
  Serial.print("artNetUniverse: "); Serial.println(config->artNetUniverse);
  Serial.print("artNetChannel: "); Serial.println(config->artNetChannel);
  Serial.print("resetToDefaults: "); Serial.println(config->resetToDefaults);

  Button::init();
  Wifi::init();
  Machine::init();
  Oled::init();
  Radio::init();
}

void loop() {
  ESP.wdtFeed();
  Button::handle();
  Wifi::handle();
  Machine::handle();
  Oled::handle();
  Radio::handle();
}
