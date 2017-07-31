#ifndef _ota_h
#define _ota_h

#include "../wifi/wifi.h"

class OTA {
  public:
    OTA(WiFi_Client& wifiClient);
    void start();
    void handle();

  private:
    WiFi_Client& wifi;
};

#endif
