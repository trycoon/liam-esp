#include "abstract_state.h"

/**
* State the mower enters when it is paused by a user (by some clicked button or central command). When unpaused it resume last running state.
*/
class Paused : public AbstractState {

  Paused(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
