#include "abstract_state.h"

/**
* State the mower enters when it is heading home to dock with the charging station.
* It could be low on batteries, or it's seeking shelter from the rain, or maybe it's just done mowing.
*/
class Docking : public AbstractState {

  Docking(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
