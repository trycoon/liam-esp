#include "stuck.h"
#include "state_controller.h"

Stuck::Stuck(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Stuck::selected(Definitions::MOWER_STATES lastState) {
    resources.cutter.stop(true);
    resources.wheelController.stop(false);
}

void Stuck::process() {

}
