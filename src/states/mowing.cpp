#include "mowing.h"
#include "state_controller.h"

Mowing::Mowing(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Mowing::selected(Definitions::MOWER_STATES lastState) {
  //TODO: test code, remove this later.
  delay(4000);
  resources.cutter.start();
  delay(2000);
  resources.wheelController.forward();
  delay(20000);
  resources.wheelController.turnLeft(true, 1500);
  delay(2000);
  resources.wheelController.turnRight(true, 1500);
  delay(2000);
  resources.wheelController.backward();
  delay(10000);
  resources.wheelController.stop();
}

void Mowing::process() {

}
