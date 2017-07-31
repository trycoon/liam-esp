#include "docking.h"
#include "state_controller.h"

Docking::Docking(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Docking::selected(Definitions::MOWER_STATES lastState) {

}

void Docking::process() {

}
