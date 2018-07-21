#include "launching.h"
#include "state_controller.h"

Launching::Launching(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {}

void Launching::selected(Definitions::MOWER_STATES lastState) {
    launchSequence.clear();

    resources.wheelController.backward(0, 80, true);

    launchSequence.schedule([this]() {
        resources.wheelController.stop();
        resources.wheelController.turn(-180, [this](void) -> void { 
            stateController.setState(Definitions::MOWER_STATES::MOWING);
        });
    }, 6000);
}

void Launching::process() {
    launchSequence.process();
}
