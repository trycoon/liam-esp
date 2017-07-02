#include "definitions.h"

/*
  Constants and other global stuff that you should probably never need to touch.
*/

namespace Definitions
{
  const char* const APP_NAME = "liam-esp";
  const char* const APP_VERSION = "1.0.0";

  // Signal the Border Wire Fence sensors should look for, as transmitted by the charging station.
  // When this signal is detected the sensor-part of the mower is on the INSIDE of the border fence.
  extern const int16_t BFW_INSIDE_SIGNAL = 0b10000101;

  // Signal the Border Wire Fence sensors should look for, as transmitted by the charging station.
  // When this signal is detected the sensor-part of the mower is on the OUTSIDE of the border fence.
  extern const int16_t BFW_OUTSIDE_SIGNAL = 0b00000101;

}
