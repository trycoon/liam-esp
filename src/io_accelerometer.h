#ifndef accelerometer_h
#define accelerometer_h

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

class IO_Accelerometer {
  public:
    IO_Accelerometer();
    bool isAvailable();
    bool isFlipped();
    sensors_vec_t getOrientation();

  private:
    Adafruit_ADXL345_Unified accel;
    bool available = false;
};

#endif
