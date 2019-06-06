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
    ~IO_Accelerometer();
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
    bool available = false;
    void IRAM_ATTR interruptHandler();
    void getReadings();
};

#endif
