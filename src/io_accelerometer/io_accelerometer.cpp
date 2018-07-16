#include <Arduino.h>
#include "settings.h"
#include "io_accelerometer.h"
#include "configuration.h"

// https://github.com/kriswiner/EM7180_SENtral_sensor_hub/wiki/A.-A-Short-Survey-of-Sensor-Fusion-Solutions
// code exmaple from https://github.com/simondlevy/EM7180
static const uint8_t INTERRUPT_PIN = 23;
static volatile bool newData;

IO_Accelerometer::IO_Accelerometer(TwoWire& w): _Wire(w) {
  pinMode(INTERRUPT_PIN, INPUT);
}

static void interruptHandler() {
  newData = true;
}

void IO_Accelerometer::start() {
  attachInterrupt(INTERRUPT_PIN, interruptHandler, RISING);
  available = em7180.begin();

  if (!available) {
    Serial.println(em7180.getErrorString());
  } else {
    Serial.println("Gyro/accelerometer/compass init success.");

    sensorReadingTimer.schedule([this]() {
      getReadings();
    }, 100, true);
  }
}

bool IO_Accelerometer::isAvailable() {
  return available;
}

orientation IO_Accelerometer::getOrientation() {
  return currentOrientation;
}

bool IO_Accelerometer::isFlipped() {
  if (available == false) {
    return false;
  } else {    
    return (abs(currentOrientation.pitch) > Settings::TILT_ANGLE_MAX || abs(currentOrientation.roll) > Settings::TILT_ANGLE_MAX);
  }
}

void IO_Accelerometer::getReadings() {
  if (available && newData) {
    newData = false;

    em7180.checkEventStatus(); // this also clears the interrupt

    if (em7180.gotError()){
      Serial.print("IO_accelerometer error: ");
      Serial.println(em7180.getErrorString());
      //TODO: implement some kind of recovery code here!
    }

    /*
      Define output variables from updated quaternion---these are Tait-Bryan
      angles, commonly used in aircraft orientation.  In this coordinate
      system, the positive z-axis is down toward Earth.  Yaw is the angle
      between Sensor x-axis and Earth magnetic North (or true North if
      corrected for local declination, looking down on the sensor positive
      yaw is counterclockwise.  Pitch is angle between sensor x-axis and
      Earth ground plane, toward the Earth is positive, up toward the sky is
      negative.  Roll is angle between sensor y-axis and Earth ground plane,
      y-axis up is positive roll.  These arise from the definition of the
      homogeneous rotation matrix constructed from q.  Tait-Bryan
      angles as well as Euler angles are non-commutative; that is, the get
      the correct orientation the rotations must be applied in the correct
      order which for this configuration is yaw, pitch, and then roll.  For
      more see http://en.wikipedia.org/wiki/Conversion_between_q_and_Euler_angles 
      which has additional links.
    */

    if (em7180.gotQuaternion()) {

      float qw, qx, qy, qz;

      em7180.readQuaternion(qw, qx, qy, qz);

      float roll = atan2(2.0f * (qw * qx + qy * qz), qw * qw - qx * qx - qy * qy + qz * qz);
      float pitch = -asin(2.0f * (qx * qz - qw * qy));
      float yaw = atan2(2.0f * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);

      roll *= 180.0f / PI;  // Radians to Degree
      pitch *= 180.0f / PI; // Radians to Degree
      yaw *= 180.0f / PI;   // Radians to Degree
      if (yaw < 0) {
        yaw += 360.0f; // Ensure yaw stays between 0 and 360
      }

      currentOrientation.roll = roll;
      currentOrientation.pitch = pitch;
      currentOrientation.yaw = yaw;
    }

    if (em7180.gotBarometer()) {
      float temperature, pressure;

      em7180.readBarometer(pressure, temperature);

      /*
      // TODO: implement weather support later.
      Serial.println("Barometer:");
      Serial.print("  Altimeter temperature = ");
      Serial.print(temperature, 2);
      Serial.println(" C");
      Serial.print("  Altimeter pressure = ");
      Serial.print(pressure, 2);
      Serial.println(" mbar");
      float altitude = (1.0f - powf(pressure / 1013.25f, 0.190295f)) * 44330.0f;
      Serial.print("  Altitude = ");
      Serial.print(altitude, 2);
      Serial.println(" m\n");*/
    }
  }
}

void IO_Accelerometer::process() {
  sensorReadingTimer.process();
}
