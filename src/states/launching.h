#ifndef _launching_h
#define _launching_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is heading out from the charging station to begin mowing.
*/
class Launching : public AbstractState {
  public:
    Launching(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    void selected(Definitions::MOWER_STATES lastState);
    void run();
};

#endif
