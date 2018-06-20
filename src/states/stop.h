#ifndef _state_stop_h
#define _state_stop_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is stopped by a user (by some clicking the stopbutton on mower or UI).
*/
class Stop : public AbstractState {
  public:
    Stop(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "STOP";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();
};

#endif
