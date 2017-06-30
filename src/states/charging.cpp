#include "abstract_state.h"

class Charging : public AbstractState {
  Charging(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
