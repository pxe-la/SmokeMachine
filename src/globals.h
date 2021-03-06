#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"

#include "Config.h"
#include "button.h"
#include "oled.h"
#include "wifi.h"
#include "machine.h"
#include "radio.h"

typedef char str32_t[32];

struct config_s {
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
};

extern Config<config_s> config;

void initGlobals();
void resetToDefaults();

#endif
