#include "flipped.h"
#include "state_controller.h"

Flipped::Flipped(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Flipped::selected(Definitions::MOWER_STATES lastState) {
  resources.cutter.stop(true);
  resources.wheelController.stop(false);
}

void Flipped::process() {
/*
  // pause 4 seconds
  if (!resources.io_accelerometer.isFlipped()) {
    stateController.setState(lastState);
  }*/
}
