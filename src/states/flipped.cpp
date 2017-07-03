#include "flipped.h"
#include "state_controller.h"

Flipped::Flipped(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Flipped::selected(Definitions::MOWER_STATES lastState) {

}

void Flipped::run() {

}
