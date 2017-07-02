#include "abstract_state.h"

/**
* State the mower enters when it is commanded to run the demo program.
* Usually this mean testing the wheel motors, the cutter motor, and stuff that shows the mowing is working properly.
*/
class Demo : public AbstractState {

  Demo(StateController& stateController) : AbstractState(stateController) {

  }

  void selected(Definitions::MOWER_STATES lastState) {

  }

  void run() {

  }
};
