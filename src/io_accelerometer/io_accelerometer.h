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

    // How many samples should we take to calculate a median value for the gyro/accelerometer/compass. Don't fiddle with this unless needed.
    const static uint8_t GYRO_MEDIAN_SAMPLES = 5;

    LSM9DS1 imu;
    TwoWire& _Wire;
    Ticker sensorReadingTicker;
    orientation currentOrientation;

    // All these arrays NEEDS to be of the same size and type!
    int16_t ax_sample[GYRO_MEDIAN_SAMPLES] = {0};
    int16_t ay_sample[GYRO_MEDIAN_SAMPLES] = {0};
    int16_t az_sample[GYRO_MEDIAN_SAMPLES] = {0};
    int16_t mx_sample[GYRO_MEDIAN_SAMPLES] = {0};
    int16_t my_sample[GYRO_MEDIAN_SAMPLES] = {0};

    uint8_t medianIndex = 0;

    bool available = false;
    void getReadings();
};

#endif
