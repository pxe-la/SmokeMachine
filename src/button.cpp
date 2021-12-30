#include "button.h"

static const int TRIGGER_PIN = D0;
static const int DEBOUNCE_TIME = 20;
static const int RELEASE_TIME = 300;

void Button::init() {
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
}

void Button::handle() {
  static uint32_t debounceTimer = 0;
  static bool prevState = false;
  static bool debouncedState = false;
  static uint8_t clicks = 0;

  uint32_t mil = millis();
  bool btnState = !digitalRead(TRIGGER_PIN);

  if (btnState != prevState) {
    debounceTimer = mil;
    prevState = btnState;
  }

  uint32_t debounce = mil - debounceTimer;

  if (debounceTimer && debounce > DEBOUNCE_TIME && btnState != debouncedState) {
    debouncedState = btnState;
    if (btnState) {
      Machine::run(100);
    } else {
      clicks++;
    }
  }

  if (debouncedState) {
    Machine::run(100);
  }

  if (debounceTimer && debounce > RELEASE_TIME && !btnState) {
    debounceTimer = 0;
    Serial.println(clicks);
    switch(clicks) {
      case 2:
        Machine::run(config->stickyTime);
        break;
      case 3:
        Machine::recordStickyTime = !Machine::recordStickyTime;
      // nobreak
      case 1:
        Machine::run(0);
        break;
      case 4:
        Machine::autoRunner = !Machine::autoRunner;
        break;
      case 5:
        Radio::learnCode = Radio::learnCode ? 0 : 1;
        break;
      case 6:
        Radio::learnCode = Radio::learnCode ? 0 : 2;
        break;
      case 10:
        if (millis() < 10000) resetToDefaults();
    }
    clicks = 0;
  }
}
