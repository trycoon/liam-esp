#ifndef _paused_h
#define _paused_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is paused by a user (by some clicked button or central command). When unpaused it resume last running state.
*/
class Paused : public AbstractState {
  public:
    Paused(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "PAUSED";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void run();
};

#endif
