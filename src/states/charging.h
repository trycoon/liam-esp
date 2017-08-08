#ifndef _state_charging_h
#define _state_charging_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it has successfully docked with the charging station and have begun charging the batteries.
*/
class Charging : public AbstractState {
  public:
    Charging(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "CHARGING";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();
};

#endif
