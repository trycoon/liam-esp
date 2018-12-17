#include "launching.h"
#include "state_controller.h"

Launching::Launching(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {}

void Launching::selected(Definitions::MOWER_STATES lastState) {
    resources.wheelController.backward(0, 70, true, Definitions::LAUNCH_DISTANCE, 
    [this](void) -> void { 
        resources.wheelController.turn(-180, [this](void) -> void { 
            stateController.setState(Definitions::MOWER_STATES::MOWING);
        });
    });
}

void Launching::process() {
}
