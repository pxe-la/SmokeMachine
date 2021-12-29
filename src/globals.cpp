#include "globals.h"

Config<config_s> config;

void initGlobals() {
  Serial.begin(115200);

  if (config->resetToDefaults) {
    config->resetToDefaults = false;
    strcpy(config->ssid, DEFAULT_SSID);
    strcpy(config->pass, DEFAULT_PASS);
    strcpy(config->ap_ssid, DEFAULT_AP_SSID);
    strcpy(config->ap_pass, DEFAULT_AP_PASS);

    config.save();
  }
}

void resetToDefaults() {
  config->resetToDefaults = true;
  config.save();
  ESP.restart();
}
