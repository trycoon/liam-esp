#ifndef _controller_h
#define _controller_h

#include "mqtt/mqtt.h"
#include "wheel.h"
#include "cutter.h"
#include "bwf.h"
#include "battery.h"
#include "gps.h"
#include "definitions.h"
#include "state_controller.h"

/**
* The main controller (or "brain" if you like) of the mower.
*/
class Controller {
  public:
    Controller(MQTT_Client& mqttClient, Wheel& leftWheel, Wheel& rightWheel, Cutter& cutter, BWF& bwf, Battery& battery, GPS& gps);
    void run();

  private:
    MQTT_Client& mqttClient;
    Wheel& leftWheel;
    Wheel& rightWheel;
    Cutter& cutter;
    BWF& bwf;
    Battery& battery;
    GPS& gps;
    StateController stateController;
};

#endif
