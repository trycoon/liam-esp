#ifndef _docked_h
#define _docked_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it has successfully docked with the charging station.
*/
class Docked : public AbstractState {
  public:
    Docked(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "DOCKED";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();
};

#endif
