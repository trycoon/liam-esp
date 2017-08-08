#include "test.h"
#include "state_controller.h"

Test::Test(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources), testSequence(true) {

}

void Test::selected(Definitions::MOWER_STATES lastState) {
  Serial.println("state selected");
  // clear previous scheduled sequence.
  testSequence.clear();

  testSequence.schedule([this]() {
    Serial.println("cutter start");
    resources.cutter.start();
  }, 4000);

  testSequence.schedule([this]() {
    Serial.println("forward");
    resources.wheelController.forward();
  }, 2000);

  testSequence.schedule([this]() {
    Serial.println("stop");
    resources.wheelController.stop();
  }, 20000);

  testSequence.schedule([this]() {
    Serial.println("left");
    resources.wheelController.turnLeft(true, 1500);
  }, 50);

  testSequence.schedule([this]() {
    Serial.println("right");
    resources.wheelController.turnRight(true, 1500);
  }, 2000);

  testSequence.schedule([this]() {
    Serial.println("stop");
    resources.wheelController.stop();
  }, 2000);

  testSequence.schedule([this]() {
    Serial.println("backward");
    resources.wheelController.backward();
  }, 50);

  testSequence.schedule([this]() {
    Serial.println("stop");
    resources.wheelController.stop();
  }, 10000);

  testSequence.schedule([this]() {
    Serial.println("cutter stop");
    resources.cutter.stop(true);
  }, 2000);
}

void Test::process() {
  testSequence.process();
}
