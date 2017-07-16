#ifndef accelerometer_h
#define accelerometer_h

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

class IO_Accelerometer {
  public:
    IO_Accelerometer();
    bool isAvailable();
    bool isFlipped();
    //acceleration_t getAcceleration();

  private:
    Adafruit_ADXL345_Unified accel;
    bool isAvailable;
};

#endif
