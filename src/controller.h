#ifndef _controller_h
#define _controller_h

#include <Arduino.h>
#include "mqtt/mqtt.h"
#include "wheel.h"
#include "cutter.h"
#include "bwf.h"
#include "battery.h"
#include "gps.h"
#include "state_controller.h"

class Controller : public StateController {
  public:
    Controller(MQTT_Client& mqttClient, Wheel& leftWheel, Wheel& rightWheel, Cutter& cutter, BWF& bwf, Battery& battery, GPS& gps);

  private:
    MQTT_Client& mqttClient;
    Wheel& leftWheel;
    Wheel& rightWheel;
    Cutter& cutter;
    BWF& bwf;
    Battery& battery;
    GPS& gps;
};

#endif
