#ifndef _demo_h
#define _demo_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is commanded to run the demo program.
* Usually this mean testing the wheel motors, the cutter motor, and stuff that shows the mowing is working properly.
*/
class Demo : public AbstractState {
  public:
    Demo(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "DEMO";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void run();
};

#endif
