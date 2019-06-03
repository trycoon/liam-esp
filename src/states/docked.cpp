#include "docked.h"
#include "state_controller.h"

Docked::Docked(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : AbstractState(myState, stateController, resources) {

}

void Docked::selected(Definitions::MOWER_STATES lastState) {
  resources.cutter.stop(true);
  resources.wheelController.stop();
  lastShouldMowCheck = millis();
}

void Docked::process() {

  // if we receive current from the docking station, enter "charging"-state.
  if (resources.battery.isCharging()) {
    stateController.setState(Definitions::MOWER_STATES::CHARGING);
    return;
  }

  // Only check time to mow every other second, for performance reasons.
  if (lastShouldMowCheck + 2000 < millis()) {

    if (resources.mowingSchedule.isTimeToMow()) {
      stateController.setState(Definitions::MOWER_STATES::LAUNCHING);
    }
  
    lastShouldMowCheck = millis();
  } 
   
}
