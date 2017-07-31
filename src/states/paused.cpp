#include "paused.h"
#include "state_controller.h"

Paused::Paused(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Paused::selected(Definitions::MOWER_STATES lastState) {

}

void Paused::process() {

}
