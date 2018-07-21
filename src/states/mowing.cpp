#include "mowing.h"
#include "state_controller.h"

Mowing::Mowing(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Mowing::selected(Definitions::MOWER_STATES lastState) {
    resources.cutter.start();
    resources.wheelController.forward(0, 100, true);
}

void Mowing::process() {
}
