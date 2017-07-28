#ifndef _mowing_h
#define _mowing_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is mowing.
*/
class Mowing : public AbstractState {
  public:
    Mowing(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "MOWING";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void run();
};

#endif
