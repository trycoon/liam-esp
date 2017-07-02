#include "abstract_state.h"

/**
* State the mower enters when it is stuck somewhere and failes to operate properly.
* It may be stuck in a hole in the lawn, or under a obstacle, or there may be some other hardware issues, in either way it require some kind of human intervention.
* This is a state that should not occur under normal conditions.
*/
class Stuck : public AbstractState {

  Stuck(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
