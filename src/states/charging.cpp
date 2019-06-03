#include "charging.h"
#include "state_controller.h"

Charging::Charging(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Charging::selected(Definitions::MOWER_STATES lastState) {
  resources.cutter.stop(true);
  resources.wheelController.stop();
}

void Charging::process() {

  if (resources.battery.isFullyCharged()) {
    stateController.setState(Definitions::MOWER_STATES::DOCKED);
    return;
  }
  
}
