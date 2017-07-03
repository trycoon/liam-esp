#include "demo.h"
#include "state_controller.h"

Demo::Demo(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Demo::selected(Definitions::MOWER_STATES lastState) {

}

void Demo::run() {

}
