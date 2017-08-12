#ifndef accelerometer_h
#define accelerometer_h

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

struct orientation {
  float pitch;
  float roll;
  float heading;
};

class IO_Accelerometer {
  public:
    IO_Accelerometer();
    bool isAvailable();
    bool isFlipped();
    orientation getOrientation();

  private:
    Adafruit_ADXL345_Unified accel;
    bool available = false;
};

#endif
