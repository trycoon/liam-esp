#include "docked.h"
#include "state_controller.h"

Docked::Docked(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Docked::selected(Definitions::MOWER_STATES lastState) {

}

void Docked::process() {
  // TODO: if battery level too low, enter "charging"-state.
  // TODO: if battery level OK, enter "mowing"-state.
  stateController.setState(Definitions::MOWER_STATES::MOWING);
}
