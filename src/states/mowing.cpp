#include <ArduinoLog.h>
#include "mowing.h"
#include "state_controller.h"

Mowing::Mowing(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Mowing::selected(Definitions::MOWER_STATES lastState) {
  resources.cutter.start();
  delay(2000);
  resources.wheelController.forward(0, 100, true);
  lastShouldMowCheck = millis();
}

void Mowing::process() {

  if (resources.battery.needRecharge()) {
    stateController.setState(Definitions::MOWER_STATES::DOCKING);
    return;
  }

  // Only check time to mow every other second, for performance reasons.
  if (lastShouldMowCheck + 2000 < millis()) {
    if (!resources.mowingSchedule.isTimeToMow()) {
      stateController.setState(Definitions::MOWER_STATES::DOCKING);
      return;
    }

    lastShouldMowCheck = millis();
  }
}
