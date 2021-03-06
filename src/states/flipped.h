#ifndef _state_flipped_h
#define _state_flipped_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is flipped upside down or tilted too much.
* This is a state that should not occur under normal conditions.
*/
class Flipped : public AbstractState {
  public:
    Flipped(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "FLIPPED";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();
  
  private:
    Definitions::MOWER_STATES previousState;
    uint32_t timer = 0;
};

#endif
