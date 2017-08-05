#include "mowing.h"
#include "state_controller.h"

Mowing::Mowing(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Mowing::selected(Definitions::MOWER_STATES lastState) {
  //TODO: test code, remove this later.
  Serial.println("state selected");
  delay(4000);
  Serial.println("cutter start");
  resources.cutter.start();
  delay(2000);
  Serial.println("forward");
  resources.wheelController.forward();
  delay(20000);
  Serial.println("left");
  resources.wheelController.turnLeft(true, 1500);
  delay(2000);
  Serial.println("right");
  resources.wheelController.turnRight(true, 1500);
  delay(2000);
  Serial.println("backward");
  resources.wheelController.backward();
  delay(10000);
  Serial.println("stop");
  resources.wheelController.stop();
}

void Mowing::process() {
}
