#include "abstract_state.h"

class Launching : public AbstractState {
  Launching(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(MOWER_STATES lastState) {

  }

  void run() {

  }
};
