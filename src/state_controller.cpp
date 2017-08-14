#include "state_controller.h"
#include "states/docked.h"
#include "states/launching.h"
#include "states/mowing.h"
#include "states/docking.h"
#include "states/charging.h"
#include "states/stuck.h"
#include "states/flipped.h"
#include "states/manual.h"
#include "states/paused.h"
#include "states/test.h"

StateController::StateController(Definitions::MOWER_STATES initialState, Resources& resources) : resources(resources) {
  stateLookup[Definitions::MOWER_STATES::DOCKED] = new Docked(Definitions::MOWER_STATES::DOCKED, *this, resources);
  stateLookup[Definitions::MOWER_STATES::LAUNCHING] = new Launching(Definitions::MOWER_STATES::LAUNCHING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::MOWING] = new Mowing(Definitions::MOWER_STATES::MOWING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::DOCKING] = new Docking(Definitions::MOWER_STATES::DOCKING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::CHARGING] = new Charging(Definitions::MOWER_STATES::CHARGING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::STUCK] = new Stuck(Definitions::MOWER_STATES::STUCK, *this, resources);
  stateLookup[Definitions::MOWER_STATES::FLIPPED] = new Flipped(Definitions::MOWER_STATES::FLIPPED, *this, resources);
  stateLookup[Definitions::MOWER_STATES::MANUAL] = new Manual(Definitions::MOWER_STATES::MANUAL, *this, resources);
  stateLookup[Definitions::MOWER_STATES::PAUSED] = new Paused(Definitions::MOWER_STATES::PAUSED, *this, resources);
  stateLookup[Definitions::MOWER_STATES::TEST] = new Test(Definitions::MOWER_STATES::TEST, *this, resources);

  setState(initialState);
}

void StateController::setState(Definitions::MOWER_STATES newState) {
  // save reference to previous state before we switching to a new one. We check for nullptr because the first time there will be no previous state.
  Definitions::MOWER_STATES previousState = currentStateInstance == nullptr ? newState : currentStateInstance->getState();

  currentStateInstance = stateLookup[newState];
  currentStateInstance->selected(previousState);

  Serial.print("New state: "); Serial.println(currentStateInstance->getStateName());
  resources.mqtt.publish_message(currentStateInstance->getStateName(), "/state");
}

AbstractState* StateController::getStateInstance() {
  return currentStateInstance;
}
