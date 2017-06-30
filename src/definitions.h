#ifndef _definitions_h
#define _definitions_h

#include <Arduino.h>

/*
  Constants and other global stuff that you should probably never need to touch.
*/

class Definitions {
  public:
    static const char* APP_NAME;
    static const char* APP_VERSION;
    enum class MOWER_STATES;
    enum class BATTERY_TYPE;
    static const int16_t BFW_INSIDE_SIGNAL;
    static const int16_t BFW_OUTSIDE_SIGNAL;
};

#endif
