#ifndef _state_launching_h
#define _state_launching_h

#include "abstract_state.h"
#include "resources.h"
#include "../scheduler/scheduler.h"

/**
* State the mower enters when it is heading out from the charging station to begin mowing.
*/
class Launching : public AbstractState {
  public:
    Launching(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "LAUNCHING";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();
  
  private:
    Scheduler launchSequence;
};

#endif
