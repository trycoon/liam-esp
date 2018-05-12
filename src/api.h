#ifndef _liam_api_h
#define _liam_api_h

#include <Arduino.h>  // This include is needed to solve this issue: https://github.com/me-no-dev/ESPAsyncWebServer/issues/46
//#include <Hash.h>     // This include is needed to solve this issue: https://github.com/me-no-dev/ESPAsyncWebServer/issues/46
#include <ESPAsyncWebServer.h>
#include "state_controller.h"
#include "resources.h"

class Api {
  public:
    Api(StateController& stateController, Resources& resources);
    void setupApi(AsyncWebServer& web_server);

  private:
    StateController& stateController;
    Resources& resources;
};

#endif
