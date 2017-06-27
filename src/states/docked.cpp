#include "abstract_state.h"

class Docked : public AbstractState {
  Docked(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(MOWER_STATES lastState) {

  }

  void run() {

  }
};
