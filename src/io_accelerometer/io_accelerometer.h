#ifndef accelerometer_h
#define accelerometer_h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BNO055.h"

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

  private:
    TwoWire& _Wire;
    Adafruit_BNO055* accel;
    bool initialized = false;
    bool available = false;
    void initialize();
};

#endif
