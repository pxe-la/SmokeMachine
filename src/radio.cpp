#include "RCSwitch.h"
#include "radio.h"

static const int RADIO_PIN = D7;
static RCSwitch radio;

void Radio::init() {
  radio.enableReceive(RADIO_PIN);
}

void Radio::handle() {
  if (radio.available()) {
    uint32_t code = radio.getReceivedValue();
    radio.resetAvailable();

    if (learnCode == 1) {
      config->radioCodeShort = code;
    } else if (learnCode == 2) {
      config->radioCodeSticky = code;
    }
    learnCode = 0;

    if (config->radioCodeShort == code) {
      Machine::run(200);
    }
    if (config->radioCodeSticky == code) {
      Machine::run(config->stickyTime);
    }
  }
}

uint8_t Radio::learnCode = 0;
