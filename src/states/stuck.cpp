#include "abstract_state.h"

class Stuck : public AbstractState {
  Stuck(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
