#ifndef _definitions_h
#define _definitions_h

#include <Arduino.h>

/*
  Constants and other global stuff that you should probably never need to touch.
*/

namespace Definitions {
    extern const char* const APP_NAME;
    extern const char* const APP_VERSION;

    enum class MOWER_STATES {
                              DOCKED,     // mower is docked in its charging station and is fully charged
                              LAUNCHING,  // mower is leaving its charging station to head out for mowing
                              MOWING,     // mower is currently mowing
                              DOCKING,    // mower has stopped mowing and is heading back to its charging station (battery may be running low)
                              CHARGING,   // mower is docked in its charging station and is currently charging
                              STUCK,      // mower is stuck somewhere and have shutdown (it may be stuck in a hole, the cutter may be stuck, or some other hardware issues...)
                              FLIPPED,    // mower is flipped upside down or tiled too much, stopp mowing and wait to be unflipped.
                              PAUSED,     // mower is paused from it's normal operation, when unpaused it should pursue last state.
                              TEST        // mower is in test mode.
                            };

    extern const int16_t BFW_INSIDE_SIGNAL;
    extern const int16_t BFW_OUTSIDE_SIGNAL;
};

#endif
