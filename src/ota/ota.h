#ifndef _ota_h
#define _ota_h

#include "../mqtt/mqtt.h"

class OTA {
  public:
    OTA(MQTT_Client& mqttClient);
    void start();
    void handle();

  private:
    MQTT_Client& mqtt;
};

#endif
