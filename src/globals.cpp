#include "globals.h"

Config<config_s> config;

void initGlobals() {
  Serial.begin(115200);

  if (config->resetToDefaults) {
    config->resetToDefaults = false;
    strcpy(config->ssid, "AlexNET");
    strcpy(config->pass, "");
    strcpy(config->apSsid, "SmokeMachine");
    strcpy(config->apPass, "12345678");
    config->stickyTime = 1000;

    config->autoRunnerInterval = 30000;
    config->autoRunnerDuration = 500;

    config->radioCodeShort = 0;
    config->radioCodeSticky = 0;

    config->artNetUniverse = 0;
    config->artNetChannel = 0;

    config.save();
  }
}

void resetToDefaults() {
  config->resetToDefaults = true;
  config.save();
  ESP.restart();
}
