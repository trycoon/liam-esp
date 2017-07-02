#include "abstract_state.h"

/**
* State the mower enters when it is flipped upside down or tilted too much.
* This is a state that should not occur under normal conditions.
*/
class Flipped : public AbstractState {

  Flipped(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
