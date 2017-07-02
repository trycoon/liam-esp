#include "abstract_state.h"

/**
* State the mower enters when it is mowing.
*/
class Mowing : public AbstractState {

  Mowing(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
