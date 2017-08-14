#ifndef _state_manual_h
#define _state_manual_h

#include "abstract_state.h"
#include "resources.h"

/**
* State the mower enters when it has been set to manual driving mode from REST-API. User then has to change state again to resume automation.
*/
class Manual : public AbstractState {
  public:
    Manual(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources);
    const char* const getStateName() {
      return "MANUAL";
    }
    void selected(Definitions::MOWER_STATES lastState);
    void process();
};

#endif
