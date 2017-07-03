#include "charging.h"
#include "state_controller.h"

Charging::Charging(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Charging::selected(Definitions::MOWER_STATES lastState) {

}

void Charging::run() {

}
