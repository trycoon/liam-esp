#ifndef _gps_h
#define _gps_h

#include <Arduino.h>
#include <UbxGpsNavPvt.h>
#include <deque>

// Default baudrate is determined by the receiver manufacturer.
#define GPS_DEFAULT_BAUDRATE 9600L
// Note, if changing this then also change code in changeBaudrate()!
#define GPS_BAUDRATE 115200L

// Array of possible baudrates that can be used by the receiver, sorted descending to prevent excess Serial flush/begin
// after restoring defaults. You can uncomment values that can be used by your receiver before the auto-configuration.
const long possibleBaudrates[] = {
    //921600L,
    //460800L,
    //230400L,
    115200L,
    //57600L,
    //38400L,
    //19200L,
    9600L,
    //4800L,
};

struct gpsPosition {
  uint32_t time;
  double lat;
  double lng;
};

class GPS {
  public:
    GPS();
    void init();
    void start();
    bool isEnabled();
    const std::deque<gpsPosition>& getGpsPositionHistory() const;
  private:
    static const uint16_t MAX_SAMPLES = 100;   // How much history are we going to keep? set too high will consume excessive memory and we may get out-of-memory related errors.
    HardwareSerial gpsSerial;
    UbxGpsNavPvt<HardwareSerial> gps;
    void restoreDefaults();
    void disableNmea();
    void changeBaudrate();
    void changeFrequency();
    void enableNavPvt();
    void sendPacket(const byte *packet, byte len);
    std::deque<gpsPosition> gpsPosistionSamples;
    gpsPosition lastMowingPosition;
};

#endif
