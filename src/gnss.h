#ifndef _gnss_h
#define _gnss_h

#include <Arduino.h>
#include <Wire.h>

#include <deque>

#include "SparkFun_Ublox_Arduino_Library.h"

struct gnssPosition {
  uint32_t time;
  long lat;
  long lng;
};

class GNSS {
 public:
  GNSS(TwoWire &w);
  void init();
  void start();
  const std::deque<gnssPosition> &getGnssPositionHistory() const;

 private:
  static const uint16_t MAX_SAMPLES =
      100;  // How much history are we going to keep? set too high will consume
            // excessive memory and we may get out-of-memory related errors.
  SFE_UBLOX_GPS gnss;
  TwoWire &_Wire;
  long lastTime =
      0;  // Simple local timer. TODO: remove this when done debugging.
  std::deque<gnssPosition> gnssPosistionSamples;
  gnssPosition lastMowingPosition{};
};

#endif
