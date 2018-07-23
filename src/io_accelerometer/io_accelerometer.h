#ifndef accelerometer_h
#define accelerometer_h

#include <Wire.h>
#include <Ticker.h>
#include "EM7180.h"

static const uint8_t ARES = 8;           // Gs
static const uint16_t GRES = 2000;       // degrees per second
static const uint16_t MRES = 1000;       // microTeslas
static const uint8_t MAG_RATE = 100;     // Hz
static const uint16_t ACCEL_RATE = 200;  // Hz
static const uint16_t GYRO_RATE = 200;   // Hz
static const uint8_t BARO_RATE = 50;     // Hz
static const uint8_t Q_RATE_DIVISOR = 3; // 1/3 gyro rate

struct orientation {
  float pitch;
  float roll;
  float heading;
};

class IO_Accelerometer {
  public:
    IO_Accelerometer(TwoWire& w);
    bool isAvailable();
    bool isFlipped();
    orientation getOrientation();
    void start();

  private:
    TwoWire& _Wire;
    Ticker sensorReadingTicker;
    EM7180_Master em7180;
    orientation currentOrientation;
    void getReadings();
    bool available = false;
};

#endif
