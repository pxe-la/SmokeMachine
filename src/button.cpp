#include "button.h"

static const int TRIGGER_PIN = D0;
static const int DEBOUNCE_TIME = 30;
static const int RELEASE_TIME = 400;

void Button::init() {
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
}

void Button::handle() {
  static uint32_t debounceTimer = 0;
  static bool prevState = false;
  static bool prevDebouncedState = false;
  static uint8_t clicks = 0;

  uint32_t mil = millis();
  bool btnState = !digitalRead(TRIGGER_PIN);

  if (btnState != prevState) {
    debounceTimer = mil;
  }

  uint32_t debounce = mil - debounceTimer;

  if (debounce > DEBOUNCE_TIME && btnState != prevDebouncedState) {
    prevDebouncedState = btnState;
    if (!btnState) {
      clicks++;
    } else {
      Machine::run(100);
    }
  }

  if (debounce > RELEASE_TIME && !btnState) {
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
        Radio::learnCode = Radio::learnCode ? 0 : 1;
        break;
      case 5:
        Radio::learnCode = Radio::learnCode ? 0 : 2;
        break;
      case 10:
        if (millis() < 10000) resetToDefaults();
    }
    clicks = 0;
  }
}
