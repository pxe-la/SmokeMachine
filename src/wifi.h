#ifndef WIFI_H
# define WIFI_H

#include "IPAddress.h"
#include "globals.h"

namespace Wifi {
  void init();
  void handle();

  IPAddress getIp();
}

#endif
