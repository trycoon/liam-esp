#ifndef _state_mowing_h
#define _state_mowing_h

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
    void process();
  
  private:
    long lastShouldMowCheck = 0;
};

#endif
