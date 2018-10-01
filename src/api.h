#ifndef _liam_api_h
#define _liam_api_h

#include <Arduino.h>  // This include is needed to solve this issue: https://github.com/me-no-dev/ESPAsyncWebServer/issues/46
#include <Ticker.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "state_controller.h"
#include "resources.h"

struct statusResponse {
  const char* state;
  float batteryVoltage;
  float batteryLevel;
  bool isCharging;
  uint32_t lastFullyChargeTime;
  uint32_t lastChargeDuration;
  uint32_t uptime;    // in seconds since start.
  int8_t wifiSignal;
  uint8_t cutterLoad;
  bool cutterRotating;
  int16_t leftWheelSpd;
  int16_t rightWheelSpd;
  uint16_t targetHeading;
  int16_t pitch;
  int16_t roll;
  uint16_t heading;
};

class Api {
  public:
    Api(StateController& stateController, Resources& resources);
    void setupApi();

  private:
    StateController& stateController;
    Resources& resources;
    Ticker pushNewInfoTicker;
    statusResponse currentStatus;
    uint32_t lastMQTT_push;
    void collectAndPushNewStatus();
    void statusToJson(statusResponse obj, JsonObject& json);
};

#endif
