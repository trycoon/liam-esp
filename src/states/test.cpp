#include "test.h"
#include "state_controller.h"

Test::Test(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources), testSequence(true) {

}

void Test::selected(Definitions::MOWER_STATES lastState) {
  // clear previous scheduled sequence.
  testSequence.clear();

  testSequence.schedule([this]() {
    Serial.println("cutter start");
    resources.cutter.start();
  }, 4000);

  testSequence.schedule([this]() {
    Serial.println("forward");
    resources.wheelController.forward(0, 100, true);
  }, 2000);

  testSequence.schedule([this]() {
    Serial.println("stop");
    resources.wheelController.stop(true);
  }, 20000);

  testSequence.schedule([this]() {
    Serial.println("left");
    resources.wheelController.turn(-90);
  }, 50);

  testSequence.schedule([this]() {
    Serial.println("right");
    resources.wheelController.turn(90);
  }, 2000);

  testSequence.schedule([this]() {
    Serial.println("stop");
    resources.wheelController.stop();
  }, 2000);

  testSequence.schedule([this]() {
    Serial.println("backward");
    resources.wheelController.backward(0, 100, true);
  }, 50);

  testSequence.schedule([this]() {
    Serial.println("stop");
    resources.wheelController.stop(true);
  }, 10000);

  testSequence.schedule([this, lastState]() {
    Serial.println("cutter stop");
    resources.cutter.stop(true);

    stateController.setState(lastState);
  }, 2000);
}

void Test::process() {
  testSequence.process();
}
