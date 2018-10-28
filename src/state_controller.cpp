#include <ArduinoLog.h>
#include "state_controller.h"
#include "states/docked.h"
#include "states/launching.h"
#include "states/mowing.h"
#include "states/docking.h"
#include "states/charging.h"
#include "states/stuck.h"
#include "states/flipped.h"
#include "states/manual.h"
#include "states/stop.h"
#include "states/test.h"
#include "configuration.h"

StateController::StateController(Resources& resources) : resources(resources) {
  stateLookup[Definitions::MOWER_STATES::DOCKED] = new Docked(Definitions::MOWER_STATES::DOCKED, *this, resources);
  stateLookup[Definitions::MOWER_STATES::LAUNCHING] = new Launching(Definitions::MOWER_STATES::LAUNCHING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::MOWING] = new Mowing(Definitions::MOWER_STATES::MOWING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::DOCKING] = new Docking(Definitions::MOWER_STATES::DOCKING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::CHARGING] = new Charging(Definitions::MOWER_STATES::CHARGING, *this, resources);
  stateLookup[Definitions::MOWER_STATES::STUCK] = new Stuck(Definitions::MOWER_STATES::STUCK, *this, resources);
  stateLookup[Definitions::MOWER_STATES::FLIPPED] = new Flipped(Definitions::MOWER_STATES::FLIPPED, *this, resources);
  stateLookup[Definitions::MOWER_STATES::MANUAL] = new Manual(Definitions::MOWER_STATES::MANUAL, *this, resources);
  stateLookup[Definitions::MOWER_STATES::STOP] = new Stop(Definitions::MOWER_STATES::STOP, *this, resources);
  stateLookup[Definitions::MOWER_STATES::TEST] = new Test(Definitions::MOWER_STATES::TEST, *this, resources);
}

void StateController::setState(Definitions::MOWER_STATES newState) {
  // only set state if not same
  if (currentStateInstance == nullptr || currentStateInstance->getState() != newState) {
    // save reference to previous state before we switching to a new one. We check for nullptr because the first time there will be no previous state.
    Definitions::MOWER_STATES previousState = currentStateInstance == nullptr ? newState : currentStateInstance->getState();

    currentStateInstance = stateLookup[newState];
    currentStateInstance->selected(previousState);

    Log.notice("New state: %s" CR, currentStateInstance->getStateName());
    // save state in case we reboot
    Configuration::config.lastState = currentStateInstance->getStateName();
    Configuration::save();

    resources.wifi.publish_mqtt(currentStateInstance->getStateName(), "/state");
  }
}

void StateController::setState(String newState) {
  // switch-cases don't support String and std::unordered_map does not support String, so we are stuck with if-else.  
  if (newState == "DOCKED") {
    setState(Definitions::MOWER_STATES::DOCKED);
  } else if (newState == "LAUNCHING") {
    setState(Definitions::MOWER_STATES::LAUNCHING);
  } else if (newState == "MOWING") {
    setState(Definitions::MOWER_STATES::MOWING);
  } else if (newState == "DOCKING") {
    setState(Definitions::MOWER_STATES::DOCKING);
  } else if (newState == "CHARGING") {
    setState(Definitions::MOWER_STATES::CHARGING);
  } else if (newState == "STUCK") {
    setState(Definitions::MOWER_STATES::STUCK);
  } else if (newState == "FLIPPED") {
    setState(Definitions::MOWER_STATES::FLIPPED);
  } else if (newState == "MANUAL") {
    setState(Definitions::MOWER_STATES::MANUAL);
  } else if (newState == "STOP") {
    setState(Definitions::MOWER_STATES::STOP);
  } else if (newState == "TEST") {
    setState(Definitions::MOWER_STATES::TEST);
  } else {
    Log.warning("state \"%s\" unknown, ignoring in setState." CR, newState);
  }
}

bool StateController::setUserChangableState(String newState) {
  if (newState == "LAUNCHING") {
    setState(Definitions::MOWER_STATES::LAUNCHING);
  } else if (newState == "MOWING") {
    setState(Definitions::MOWER_STATES::MOWING);
  } else if (newState == "DOCKING") {
    setState(Definitions::MOWER_STATES::DOCKING);
  } else if (newState == "STOP") {
    setState(Definitions::MOWER_STATES::STOP);
  } else if (newState == "TEST") {
    setState(Definitions::MOWER_STATES::TEST);
  } else {
    Log.notice(F("State \"%s\" not available for user to change." CR), newState);
    return false;
  }

  return true;
}

AbstractState* StateController::getStateInstance() {
  return currentStateInstance;
}
