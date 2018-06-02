#ifndef _statecontroller_h
#define _statecontroller_h

#include <unordered_map>
#include <vector> // NOTE: needed to fix the following error: "Arduino.h:253:18: error: expected unqualified-id before '(' token".
#include "definitions.h"
#include "resources.h"
#include "states/abstract_state.h"

class StateController {
  public:
    StateController(Definitions::MOWER_STATES initialState, Resources& resources);

    /**
    * Set new running state.
    */
    void setState(Definitions::MOWER_STATES newState);

    /**
    * Get running state instance.
    */
    AbstractState* getStateInstance();

  private:
    AbstractState* currentStateInstance;
    Resources& resources;
    // https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
    struct EnumClassHash {
        template <typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);
        }
    };
    std::unordered_map<Definitions::MOWER_STATES, AbstractState*, EnumClassHash> stateLookup;
};



#endif
