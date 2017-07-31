#ifndef _docking_h
#define _docking_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it is heading home to dock with the charging station.
* It could be low on batteries, or it's seeking shelter from the rain, or maybe it's just done mowing.
*/
class Docking : public AbstractState {
  public:
    Docking(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "DOCKING";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();
};

#endif
