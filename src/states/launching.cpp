#include "launching.h"
#include "state_controller.h"

Launching::Launching(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Launching::selected(Definitions::MOWER_STATES lastState) {

}

void Launching::process() {

}
