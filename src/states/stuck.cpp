#include "stuck.h"
#include "state_controller.h"

Stuck::Stuck(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Stuck::selected(Definitions::MOWER_STATES lastState) {
    previousState = lastState;
    resources.cutter.stop(true);
    resources.wheelController.stop(false);
    retryTimer = millis();
}

void Stuck::process() {

    if (Definitions::STUCK_RETRY_DELAY > 0 && (millis() - retryTimer > Definitions::STUCK_RETRY_DELAY * 1000)) {
        stateController.setState(previousState);
    }
    
}
