#ifndef _liam_api_h
#define _liam_api_h

#include <Arduino.h>  // This include is needed to solve this issue: https://github.com/me-no-dev/ESPAsyncWebServer/issues/46
#include <Ticker.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include "state_controller.h"
#include "resources.h"

struct statusResponse {
  const char* state;
  float batteryVoltage = 0;
  float batteryChargeCurrent = 0;
  float batteryLevel = 0;
  bool isCharging = false;
  uint32_t lastFullyChargeTime = 0;
  uint32_t lastChargeDuration = 0;
  uint32_t uptime = 0;    // in seconds since start.
  int8_t wifiSignal = 0;
  uint8_t cutterLoad = 0;
  bool cutterRotating = false;
  int16_t leftWheelSpd = 0;
  int16_t rightWheelSpd = 0;
  int16_t pitch = 0;
  int16_t roll = 0;
  uint16_t heading = 0;
  uint16_t obstacleDistance1 = 0;
  uint16_t obstacleDistance2 = 0;
  uint16_t obstacleDistance3 = 0;
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
    uint32_t lastMQTT_push = 0;
    void collectAndPushNewStatus();
    void statusToJson(statusResponse& obj, JsonObject& json);
    void onMqttMessage(char* topic, char* payload, size_t length);
    void actOnWsRequest(String type, JsonObject& payload);
};

#endif
