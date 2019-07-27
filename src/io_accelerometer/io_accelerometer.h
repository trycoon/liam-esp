#ifndef accelerometer_h
#define accelerometer_h

#include <Wire.h>
#include <Ticker.h>
#include <SparkFunLSM9DS1.h>
#include "madgwick_filters.h"

struct Orientation {
  int16_t pitch = 0;
  int16_t roll = 0;
  uint16_t heading = 0;
};

class IO_Accelerometer {
  public:
    IO_Accelerometer(TwoWire& w);
    bool isAvailable() const;
    bool isFlipped() const;
    const Orientation& getOrientation() const;
    void start();

  private:
    // Earth's magnetic field varies by location. Add or subtract 
    // a declination to get a more accurate heading of true magnetic north.
    // Use an compass for reference.
    float DECLINATION = -50.0f; // Declination (degrees) in Gotland, SE.

   
    LSM9DS1 imu;
    TwoWire& _Wire;
    Ticker sensorReadingTicker;
    Orientation currentOrientation;
    MadgwickFilters filter;

    bool available = false;
    unsigned long lastUpdate = 0;
    unsigned long now = 0;
    float deltaTime = 0.0f;
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;
    float gx = 0.0f;
    float gy = 0.0f;
    float gz = 0.0f;
    float mx = 0.0f;
    float my = 0.0f;
    float mz = 0.0f;
    void getReadings();
};

#endif
