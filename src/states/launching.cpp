#include "abstract_state.h"

/**
* State the mower enters when it is heading out from the charging station to begin mowing.
*/
class Launching : public AbstractState {

  Launching(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
