#ifndef _statecontroller_h
#define _statecontroller_h

#include "definitions.h"

class StateController {
  public:
    /**
    * Set new running state.
    */
    virtual void setState(MOWER_STATES newState);
};

#endif
