#include "definitions.h"

/*
  Constants and other global stuff that you should probably never need to touch.
*/

const char* Definitions::APP_NAME = "liam-esp";
const char* Definitions::APP_VERSION = "1.0.0";

enum class MOWER_STATES {
                          DOCKED,     // mower is docked in its charging station and is fully charged
                          LAUNCHING,  // mower is leaving its charging station to head out for mowing
                          MOWING,     // mower is currently mowing
                          DOCKING,    // mower has stopped mowing and is heading back to its charging station (battery may be running low)
                          CHARGING,   // mower is docked in its charging station and is currently charging
                          STUCK,      // mower is stuck somewhere and have shutdown (it may be stuck in a hole, the cutter may be stuck, the mower may be flipped over...)
                          PAUSED      // mower is paused from it's normal operation, when unpaused it should pursue last state.
                        };

enum class BATTERY_TYPE {
                          LEAD_ACID,
                          NIMH,       // nickel–metal hydride battery
                          LIION       // lithium-ion/lithium polymer (LiPo) battery
                        };

// Signal the Border Wire Fence sensors should look for, as transmitted by the charging station.
// When this signal is detected the sensor-part of the mower is on the INSIDE of the border fence.
const int16_t Definitions::BFW_INSIDE_SIGNAL = 0b10000101;

// Signal the Border Wire Fence sensors should look for, as transmitted by the charging station.
// When this signal is detected the sensor-part of the mower is on the OUTSIDE of the border fence.
const int16_t Definitions::BFW_OUTSIDE_SIGNAL = 0b00000101;
