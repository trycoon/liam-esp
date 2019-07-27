#include <Arduino.h>
#include <ArduinoLog.h>
#include <FunctionalInterrupt.h>
#include "definitions.h"
#include "io_accelerometer.h"
#include "utils.h"

// https://github.com/sparkfun/ESP32_Motion_Shield/tree/master/Software
// https://learn.sparkfun.com/tutorials/esp32-thing-motion-shield-hookup-guide/using-the-imu

IO_Accelerometer::IO_Accelerometer(TwoWire& w): _Wire(w) {

  // the device's communication mode and addresses:
  imu.settings.device.commInterface = IMU_MODE_I2C;
  //imu.settings.device.mAddress = LSM9DS1_M;
  //imu.settings.device.agAddress = LSM9DS1_AG;
}

void IO_Accelerometer::start() {

  if (!imu.begin()) {
    Log.error(F("Failed to initialize gyro/accelerometer/compass, check connections!"));
  } else {
    Log.notice(F("Gyro/accelerometer/compass init success." CR));
    available = true;

    imu.calibrate(true);
    //imu.calibrateMag(true);   //TODO: check why this crashes with: Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.

    sensorReadingTicker.attach_ms<IO_Accelerometer*>(20, [](IO_Accelerometer* instance) {
      instance->getReadings();
    }, this);
  }
}

bool IO_Accelerometer::isAvailable() const {
  return available;
}

const Orientation& IO_Accelerometer::getOrientation() const {
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
    if ( imu.accelAvailable() ) {
      // To read from the accelerometer, first call the
      // readAccel() function. When it exits, it'll update the
      // ax, ay, and az variables with the most current data.
      imu.readAccel();
      ax = imu.calcAccel(imu.ax);
      ay = imu.calcAccel(imu.ay);
      az = imu.calcAccel(imu.az);
    }
    if ( imu.gyroAvailable() ) {
      // To read from the gyroscope,  first call the
      // readGyro() function. When it exits, it'll update the
      // gx, gy, and gz variables with the most current data.
      imu.readGyro();
      gx = imu.calcGyro(imu.gx) * PI / 180.0f;  // convert from radians to degrees
      gy = imu.calcGyro(imu.gy) * PI / 180.0f;
      gz = imu.calcGyro(imu.gz) * PI / 180.0f;
    }
    if ( imu.magAvailable() ) {
      // To read from the magnetometer, first call the
      // readMag() function. When it exits, it'll update the
      // mx, my, and mz variables with the most current data.
      imu.readMag();
      mx = imu.calcMag(imu.mx);
      my = imu.calcMag(imu.my);
      mz = imu.calcMag(imu.mz);      
    }

    for (uint8_t i = 0; i < 10; i++) { // iterate a fixed number of times per data read cycle
      now = micros();
 
      deltaTime = (now - lastUpdate) / 1000000.0f; // set integration time by time elapsed since last filter update
      lastUpdate = now;

      filter.madgwickQuaternionUpdate(deltaTime, -ax, +ay, +az, gx, -gy, -gz, my, -mx, mz);
    }

    auto quaternion = filter.getQuaternions();

    auto a12 = 2.0f * (quaternion.q2 * quaternion.q3 + quaternion.q1 * quaternion.q4);
    auto a22 = quaternion.q1 * quaternion.q1 + quaternion.q2 * quaternion.q2 - quaternion.q3 * quaternion.q3 - quaternion.q4 * quaternion.q4;
    auto a31 = 2.0f * (quaternion.q1 * quaternion.q2 + quaternion.q3 * quaternion.q4);
    auto a32 = 2.0f * (quaternion.q2 * quaternion.q4 - quaternion.q1 * quaternion.q3);
    auto a33 = quaternion.q1 * quaternion.q1 - quaternion.q2 * quaternion.q2 - quaternion.q3 * quaternion.q3 + quaternion.q4 * quaternion.q4;
    auto pitch = -asinf(a32);
    auto roll  = atan2f(a31, a33);
    auto yaw   = atan2f(a12, a22);

    // Convert everything from radians to degrees:
    pitch *= 180.0f / PI;
    roll  *= 180.0f / PI;
    yaw   *= 180.0f / PI; 

    yaw -= DECLINATION;
    
    if (yaw < 0) {
      yaw += 360.0f; // Ensure yaw stays between 0 and 360
    }
    
    // acceleration without gravity involved, could be used to detect when we bump into obstacles.
    //auto lin_ax1 = ax + a31;
    //auto lin_ay1 = ay + a32;
    //auto lin_az1 = az - a33;

    currentOrientation.roll = roundf(roll);
    currentOrientation.pitch = roundf(pitch);
    currentOrientation.heading = roundf(yaw);

    //Log.notice("Roll: %d, Pitch: %d, Heading: %d" CR, currentOrientation.roll, currentOrientation.pitch, currentOrientation.heading);
  }
}
