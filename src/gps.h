#ifndef _gps_h
#define _gps_h

#include <Arduino.h>
//#include <NMEAGPS.h>

class GPS {
  public:
    GPS();
    bool isEnabled();
  private:
  //  NMEAGPS gps;
  //  gps_fix currentFix;
    void GPS_isr(uint8_t c);
};

#endif
