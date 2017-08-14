#ifndef _liam_api_h
#define _liam_api_h

//#include <ArduinoJson.h>
#include <Arduino.h>  // This include is needed to solve this issue: https://github.com/me-no-dev/ESPAsyncWebServer/issues/46
#include <Hash.h>     // This include is needed to solve this issue: https://github.com/me-no-dev/ESPAsyncWebServer/issues/46
#include <ESPAsyncWebServer.h>
#include "state_controller.h"
#include "wheel_controller.h"
#include "battery.h"
#include "cutter.h"
#include "gps.h"
#include "io_accelerometer.h"
#include "metrics.h"


//https://bblanchon.github.io/ArduinoJson/faq/
class Api {
  public:
    Api(StateController& stateController, WheelController& wheelController,Battery& battery, Cutter& cutter, GPS& gps, IO_Accelerometer& tilt, Metrics& metrics);
    void setupApi(AsyncWebServer& web_server);

  private:
    StateController& stateController;
    WheelController& wheelController;
    Battery& battery;
    Cutter& cutter;
    GPS& gps;
    IO_Accelerometer& tilt;
    Metrics& metrics;
};

#endif
