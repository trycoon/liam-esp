#ifndef _gps_h
#define _gps_h

#include <Arduino.h>
#include <deque>
//#include <NMEAGPS.h>

struct gpsPosition {
  uint32_t time;
  double lat;
  double lng;
};

class GPS {
  public:
    GPS();
    bool isEnabled();
    const std::deque<gpsPosition>& getGpsPositionHistory() const;
  private:
  //  NMEAGPS gps;
  //  gps_fix currentFix;
    static const uint16_t MAX_SAMPLES = 100;   // How much history are we going to keep? set too high will consume excessive memory and we may get out-of-memory related errors.
    std::deque<gpsPosition> gpsPosistionSamples;
    gpsPosition lastMowingPosition;
    void GPS_isr(uint8_t c);
};

#endif
