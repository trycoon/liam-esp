#ifndef _liam_dockingstation_h
#define _liam_dockingstation_h

#include <Arduino.h>
#include <Ticker.h>
#include <LoRaLib.h>
#include "state_controller.h"
#include "resources.h"

class Dockingstation {
  public:
    Dockingstation(StateController& stateController, Resources& resources);
    void start();

  private:
    StateController& stateController;
    Resources& resources;
    Ticker pushNewInfoTicker;
    SX1278 lora;
    void collectAndPushNewStatus();
};

#endif
