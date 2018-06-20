#include "stop.h"
#include "state_controller.h"

Stop::Stop(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Stop::selected(Definitions::MOWER_STATES lastState) {

}

void Stop::process() {

}
