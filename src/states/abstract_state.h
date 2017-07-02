#ifndef _abstract_state_h
#define _abstract_state_h

#include "definitions.h"
class StateController;

/**
* Abstract base class for all states to inherit from.
*/
class AbstractState {
  public:
    /**
    * Constructor with a reference to be able to communicate back to the state controller.
    */
    AbstractState(StateController& stateController) : stateController(stateController) {}

    /**
    * Should be called upon when this state has been selected as the current state.
    * @param lastState the previous running state.
    */
    virtual void selected(Definitions::MOWER_STATES lastState) = 0;

    /**
    * This method will be executed on each turn in the event loop when this state is currently selected.
    */
    virtual void run() = 0;

  protected:
    StateController& stateController;
};

#endif
