#include "docking.h"
#include "state_controller.h"

Docking::Docking(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Docking::selected(Definitions::MOWER_STATES lastState) {
  resources.cutter.stop(true);
  resources.wheelController.forward(0, 80, true);
}

void Docking::process() {

  if (resources.battery.isDocked()) {
    stateController.setState(Definitions::MOWER_STATES::DOCKED);
    return;
  }
  
}
