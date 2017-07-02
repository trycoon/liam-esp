#include "state_controller.h"
#include "states/docked.h"
//#include "states/launching.cpp"

StateController::StateController(Definitions::MOWER_STATES initialState) {
  stateLookup[Definitions::MOWER_STATES::DOCKED] = new Docked(*this);
  //stateLookup[Definitions::MOWER_STATES::LAUNCHING] = new Launching(*this);
  setState(initialState);
}

void StateController::setState(Definitions::MOWER_STATES newState) {
  currentState = newState;
  currentStateInstance = stateLookup[currentState];
}

AbstractState* StateController::getState() {
  return currentStateInstance;
}
