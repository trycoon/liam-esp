#ifndef _state_stuck_h
#define _state_stuck_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is stuck somewhere and failes to operate properly.
* It may be stuck in a hole in the lawn, or under a obstacle, or there may be some other hardware issues, in either way it require some kind of human intervention.
* This is a state that should not occur under normal conditions.
*/
class Stuck : public AbstractState {
  public:
    Stuck(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "STUCK";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();

  private:
    Definitions::MOWER_STATES previousState;
    unsigned long retryTimer = 0; // when did we last try if we are still stuck?
};

#endif
