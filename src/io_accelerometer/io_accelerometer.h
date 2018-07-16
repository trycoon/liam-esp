#ifndef accelerometer_h
#define accelerometer_h

#include <Wire.h>
#include "EM7180.h"
#include "processable.h"
#include "scheduler/scheduler.h"

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
  float yaw;
};

class IO_Accelerometer : public Processable {
  public:
    IO_Accelerometer(TwoWire& w);
    bool isAvailable();
    bool isFlipped();
    orientation getOrientation();
    void start();
    void process();

  private:
    TwoWire& _Wire;
    EM7180_Master em7180 = EM7180_Master(ARES, GRES, MRES, MAG_RATE, ACCEL_RATE, GYRO_RATE, BARO_RATE, Q_RATE_DIVISOR);
    orientation currentOrientation;
    Scheduler sensorReadingTimer;
    void getReadings();
    bool available = false;
};

#endif
