#include "abstract_state.h"

class Mowing : public AbstractState {
  Mowing(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
