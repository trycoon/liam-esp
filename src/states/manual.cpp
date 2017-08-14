#include "manual.h"
#include "state_controller.h"

Manual::Manual(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Manual::selected(Definitions::MOWER_STATES lastState) {

}

void Manual::process() {

}
