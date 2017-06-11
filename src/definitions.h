#ifndef definitions_h
#define definitions_h

/*
  Constants and other global stuff that you should probably never need to touch.
*/

const char* APP_NAME = "liam-esp";
const char* APP_VERSION = "1.0.0";

enum class MOWER_STATES {
                          DOCKED,     // mower is docked in its charging station and is fully charged
                          LAUNCHING,  // mower is leaving its charging station to head out for mowing
                          MOWING,     // mower is currently mowing
                          DOCKING,    // mower has stopped mowing and is heading back to its charging station (battery may be running low)
                          CHARGING,   // mower is docked in its charging station and is currently charging
                          STUCK       // mower is stuck somewhere and have shutdown (it may be stuck in a hole, the cutter may be stuck, the mower may be flipped over...)
                        };

enum class BATTERY_TYPE {
                          LEAD_ACID,
                          NIMH,       // nickel–metal hydride battery
                          LIPO        // lithium polymer battery
                        };

const int16_t BFW_INSIDE_SIGNAL = 0b10000101;
const int16_t BFW_OUTSIDE_SIGNAL = 0b00000101;

#endif
