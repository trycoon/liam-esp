#include "manual.h"
#include "state_controller.h"

Manual::Manual(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Manual::selected(Definitions::MOWER_STATES lastState) {
  dockedDetectedTime = 0;
}

void Manual::process() {
  // if we drive manually and get disconnected from mower or mower loose WiFi connectivity, then stop mower from driving further.
  if (resources.wlan.getConnectedWebsocketClientsCount() == 0) {
    
    resources.cutter.stop();

    auto wheelStatus = resources.wheelController.getStatus();

    if (wheelStatus.leftWheelSpeed != 0 || wheelStatus.rightWheelSpeed != 0) {
      resources.wheelController.stop(true);
    }
  }

  // if we have parked in dockingstation manually and mower detects it's docked, then enter docked-state after a short timeout.
  if (resources.battery.isDocked() && dockedDetectedTime == 0) {
    dockedDetectedTime = millis();
  }

  if (millis() - dockedDetectedTime > 2000) {
    
    dockedDetectedTime = 0;

    if (resources.battery.isDocked()) {
      stateController.setState(Definitions::MOWER_STATES::DOCKED);
    }
  }  
}
