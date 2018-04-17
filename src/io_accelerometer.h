#ifndef accelerometer_h
#define accelerometer_h

#include <MPU9250_asukiaaa.h>

struct orientation {
  float pitch;
  float roll;
  float heading;
};
//https://github.com/asukiaaa/MPU9250_asukiaaa
class IO_Accelerometer {
  public:
    IO_Accelerometer();
    bool isAvailable();
    bool isFlipped();
    orientation getOrientation();

  private:
    MPU9250 sensor;
    bool available = false;
};

#endif
