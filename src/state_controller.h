#ifndef _statecontroller_h
#define _statecontroller_h

#include <unordered_map>
#include "definitions.h"
#include "states/abstract_state.h"

class StateController {
  public:
    StateController(Definitions::MOWER_STATES initialState);

    /**
    * Set new running state.
    */
    void setState(Definitions::MOWER_STATES newState);

    /**
    * Get running state.
    */
    AbstractState* getState();

  private:
    Definitions::MOWER_STATES currentState;
    AbstractState* currentStateInstance;
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
