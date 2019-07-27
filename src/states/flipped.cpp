#include "flipped.h"
#include "state_controller.h"

Flipped::Flipped(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {}

void Flipped::selected(Definitions::MOWER_STATES lastState) {
  // mower has flipped over/or are in a steep slope, stop cutter and wheels.
  previousState = lastState;
  resources.cutter.stop(true);
  resources.wheelController.stop(false);
  timer = millis();
}

void Flipped::process() {
  // if mower has been unflipped for at least 4 seconds, then resume last state (usually mowing).
  if (resources.accelerometer.isFlipped()) {
    timer = millis();
  } else {
    if (timer + 5000 < millis()) {
      stateController.setState(previousState);
    }
  }
}
