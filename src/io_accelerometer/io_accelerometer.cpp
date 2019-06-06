#include <Arduino.h>
#include <ArduinoLog.h>
#include <FunctionalInterrupt.h>
#include "definitions.h"
#include "io_accelerometer.h"

// https://github.com/kriswiner/EM7180_SENtral_sensor_hub/wiki/A.-A-Short-Survey-of-Sensor-Fusion-Solutions
// code exmaple from https://github.com/simondlevy/EM7180

// https://github.com/sparkfun/ESP32_Motion_Shield/tree/master/Software
// https://learn.sparkfun.com/tutorials/esp32-thing-motion-shield-hookup-guide/using-the-imu

static volatile bool newData;

IO_Accelerometer::IO_Accelerometer(TwoWire& w): _Wire(w) {
  //pinMode(Definitions::IO_ACCELEROMETER_INT_PIN, INPUT_PULLUP);

  // the device's communication mode and addresses:
  imu.settings.device.commInterface = IMU_MODE_I2C;
  //imu.settings.device.mAddress = LSM9DS1_M;
  //imu.settings.device.agAddress = LSM9DS1_AG;
}

IO_Accelerometer::~IO_Accelerometer() {
 // detachInterrupt(digitalPinToInterrupt(Definitions::IO_ACCELEROMETER_INT_PIN));
}

void IRAM_ATTR IO_Accelerometer::interruptHandler() { // IRAM_ATTR tells the complier, that this code Must always be in the ESP32's IRAM, the limited 128k IRAM. Use it sparingly.
  newData = true;
}

void IO_Accelerometer::start() {
 // attachInterrupt(digitalPinToInterrupt(Definitions::IO_ACCELEROMETER_INT_PIN), std::bind(&IO_Accelerometer::interruptHandler, this), RISING);

  if (!imu.begin()) {
    Log.error(F("Failed to initialize gyro/accelerometer/compass, check connections!"));
  } else {
    Log.notice(F("Gyro/accelerometer/compass init success." CR));
    available = true;

    sensorReadingTicker.attach_ms<IO_Accelerometer*>(200, [](IO_Accelerometer* instance) {
      instance->getReadings();
    }, this);
  }
}

bool IO_Accelerometer::isAvailable() const {
  return available;
}

const orientation& IO_Accelerometer::getOrientation() const {
  return currentOrientation;
}

bool IO_Accelerometer::isFlipped() const {
  if (available == false) {
    return false;
  } else {    
    return (abs(currentOrientation.pitch) > Definitions::TILT_ANGLE_MAX || abs(currentOrientation.roll) > Definitions::TILT_ANGLE_MAX);
  }
}

void IO_Accelerometer::getReadings() {
  
  if (available) {    
    // Update the sensor values whenever new data is available
    if ( imu.gyroAvailable() ) {
      // To read from the gyroscope,  first call the
      // readGyro() function. When it exits, it'll update the
      // gx, gy, and gz variables with the most current data.
      imu.readGyro();
    }
    if ( imu.accelAvailable() ) {
      // To read from the accelerometer, first call the
      // readAccel() function. When it exits, it'll update the
      // ax, ay, and az variables with the most current data.
      imu.readAccel();
    }
    if ( imu.magAvailable() ) {
      // To read from the magnetometer, first call the
      // readMag() function. When it exits, it'll update the
      // mx, my, and mz variables with the most current data.
      imu.readMag();
    }

    float roll = atan2(imu.ay, imu.az);
    float pitch = atan2(-imu.ax, sqrt(imu.ay * imu.ay + imu.az * imu.az));
    float yaw;

    if (-imu.my == 0) {
      yaw = (-imu.mx < 0) ? PI : 0;
    } else {
      yaw = atan2(-imu.mx, -imu.my);
    }
  
    // Convert everything from radians to degrees:
    yaw *= 180.0f / PI;
    pitch *= 180.0f / PI;
    roll  *= 180.0f / PI;
  
    yaw -= DECLINATION;

    if (yaw < 0) {
      yaw += 360.0f; // Ensure yaw stays between 0 and 360
    }

    //Log.notice("Pitch: %s, Roll: %s, Heading: %s" CR, String(pitch).c_str(), String(roll).c_str(), String(yaw).c_str());

    currentOrientation.roll = roundf(roll);
    currentOrientation.pitch = roundf(pitch);
    currentOrientation.heading = roundf(yaw);
  }
}

