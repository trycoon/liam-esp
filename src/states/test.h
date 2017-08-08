#ifndef _state_test_h
#define _state_test_h

#include "abstract_state.h"
#include "resources.h"
#include "../scheduler/scheduler.h"

/**
* State the mower enters when it is commanded to run the test program.
* Usually this mean testing the wheel motors, the cutter motor, and stuff that shows the mowing is working properly.
*/
class Test : public AbstractState {
  public:
    Test(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "TEST";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();

  private:
    Scheduler testSequence;
};

#endif
