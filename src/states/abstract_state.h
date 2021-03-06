#ifndef _abstract_state_h
#define _abstract_state_h

#include "definitions.h"
#include "resources.h"
class StateController;

/**
* Abstract base class for all states to inherit from.
*/
class AbstractState {
  public:
    /**
    * Constructor with a reference to be able to communicate back to the state controller.
    */
    AbstractState(Definitions::MOWER_STATES myState, StateController& stateController, Resources& resources) : myState(myState), stateController(stateController), resources(resources) {}

    /**
    * Get the state the class repressent.
    */
    Definitions::MOWER_STATES getState() {
      return myState;
    }

    /**
    * Textual representation of the state, e.g. "MOWING".
    */
    virtual const char* const getStateName();

    /**
    * Should be called upon when this state has been selected as the current state.
    * @param lastState the previous running state.
    */
    virtual void selected(Definitions::MOWER_STATES lastState) = 0;

    /**
    * This method will be executed on each turn in the event loop when this state is currently selected.
    */
    virtual void process() = 0;

  protected:
    Definitions::MOWER_STATES myState;
    StateController& stateController;
    Resources& resources;
};

#endif
