#ifndef RADIO_H
#define RADIO_H

#include "globals.h"

namespace Radio {
  extern uint8_t learnCode;

  void init();
  void handle();
}

#endif
