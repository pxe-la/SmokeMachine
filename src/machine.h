#ifndef MACHINE_H
#define MACHINE_H

#include "globals.h"

namespace Machine {
  extern bool recordStickyTime;
  extern bool autoRunner;

  void init();
  void handle();
  void run(uint16_t time = 10);

  int  getTemp();
  bool getHeaterState();
}

#endif
