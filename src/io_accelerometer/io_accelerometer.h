#ifndef accelerometer_h
#define accelerometer_h

#include <Wire.h>
#include <Ticker.h>
#include <SparkFunLSM9DS1.h>

struct orientation {
  int16_t pitch;
  int16_t roll;
  uint16_t heading;
};

class IO_Accelerometer {
  public:
    IO_Accelerometer(TwoWire& w);
    bool isAvailable() const;
    bool isFlipped() const;
    const orientation& getOrientation() const;
    void start();

  private:
    // Earth's magnetic field varies by location. Add or subtract 
    // a declination to get a more accurate heading of true magnetic north.
    // Use an compass for reference.
    float DECLINATION = -50.0f; // Declination (degrees) in Gotland, SE.

    LSM9DS1 imu;
    TwoWire& _Wire;
    Ticker sensorReadingTicker;
    orientation currentOrientation;
    uint16_t ax_sample[5] = {0, 0, 0, 0, 0};
    uint16_t ay_sample[5] = {0, 0, 0, 0, 0};
    uint16_t az_sample[5] = {0, 0, 0, 0, 0};
    uint16_t mx_sample[5] = {0, 0, 0, 0, 0};
    uint16_t my_sample[5] = {0, 0, 0, 0, 0};
    bool available = false;
    void getReadings();
};

#endif
