#include "abstract_state.h"

class Docking : public AbstractState {
  Docking(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
