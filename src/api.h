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
  uint8_t cutterLoad;
  bool cutterRotating;
  uint16_t leftWheelSpd;
  uint16_t rightWheelSpd;
  uint16_t targetHeading;
  int16_t pitch;
  int16_t roll;
  int16_t heading;
};

class Api {
  public:
    Api(StateController& stateController, Resources& resources);
    void setupApi(AsyncWebServer& web_server, AsyncWebSocket& websocket_server);

  private:
    StateController& stateController;
    Resources& resources;
    AsyncWebSocket* websockeServer;
    Ticker pushNewInfoTicker;
    statusResponse currentStatus;
    void collectAndPushNewStatus();
    void statusToJson(statusResponse obj, JsonObject& json);
    void sendDataWebSocket(JsonObject& json, AsyncWebSocketClient* client = nullptr);
};

#endif
