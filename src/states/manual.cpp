#include "manual.h"
#include "state_controller.h"

Manual::Manual(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Manual::selected(Definitions::MOWER_STATES lastState) {

}

void Manual::process() {
  // if we drive manually and get disconnected from mower or mower loose WiFi connectivity, then stop mower from driving further.
  if (resources.wifi.getConnectedWebsocketClientsCount() == 0) {
    
    resources.cutter.stop();

    auto wheelStatus = resources.wheelController.getStatus();

    if (wheelStatus.leftWheelSpeed != 0 || wheelStatus.rightWheelSpeed != 0) {
      resources.wheelController.stop(true);
    }
  }
}
