#ifndef _gps_h
#define _gps_h

#include <Arduino.h>
#include "SparkFun_Ublox_Arduino_Library.h"
#include <deque>

struct gpsPosition {
  uint32_t time;
  long lat;
  long lng;
};

class GPS {
  public:
    GPS();
    void init();
    void start();
    const std::deque<gpsPosition>& getGpsPositionHistory() const;
  private:
    static const uint16_t MAX_SAMPLES = 100;   // How much history are we going to keep? set too high will consume excessive memory and we may get out-of-memory related errors.
    SFE_UBLOX_GPS gps;
    long lastTime = 0; //Simple local timer. TODO: remove this when done debugging.
    std::deque<gpsPosition> gpsPosistionSamples;
    gpsPosition lastMowingPosition;
};

#endif
