#include "globals.h"

void setup() {
  ESP.wdtEnable(1000);
  initGlobals();

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
