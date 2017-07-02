#include "abstract_state.h"

/**
* State the mower enters when it has successfully docked with the charging station and have begun charging the batteries.
*/
class Charging : public AbstractState {

  Charging(StateController& stateController) : AbstractState(stateController) {}

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
