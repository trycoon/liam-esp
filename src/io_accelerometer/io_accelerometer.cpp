#include <Arduino.h>
#include <EEPROM.h>
#include "io_accelerometer.h"
#include "configuration.h"
#include "settings.h"

// https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/pinouts

IO_Accelerometer::IO_Accelerometer(TwoWire& w): _Wire(w) {
  accel = new Adafruit_BNO055(w);
}

void IO_Accelerometer::initialize() {
  if (!initialized) {
    available = accel->begin();

    if (!available) {
      Serial.println("No BNO055 device detected... Check your wiring or I2C ADDR!");
    }

    initialized = true;
    auto sensorId = Configuration::getInt("ACCEL_CALIB", -1);
    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;

    accel->getSensor(&sensor);
    // Check if we have calibration settings available for this sensor. This will not be true at first time use,
    // sensor replacement or if we do a "factory reset".
    if (sensorId != sensor.sensor_id) {
      Serial.println("This BNO055 sensor has not been calibrated.");
    } else {
      Serial.println("Found calibration for BNO055 sensor.");
      EEPROM.get(0, calibrationData);
      accel->setSensorOffsets(calibrationData);
    }
    //Crystal must be configured AFTER loading calibration data into BNO055.  
    accel->setExtCrystalUse(true);
  }
}

bool IO_Accelerometer::isAvailable() {
  initialize();
  return available;
}

orientation IO_Accelerometer::getOrientation() {
  initialize();

  if (available == false) {

    orientation empty;
    return empty;

  } else {
    sensors_event_t event;
    accel->getEvent(&event);

    orientation result;
    result.pitch = event.orientation.pitch;
    result.roll = event.orientation.roll;
    result.heading = event.orientation.heading;

    return result;
  }
}

bool IO_Accelerometer::isFlipped() {
  if (available == false) {
    return false;
  } else {
    sensors_event_t event;
    accel->getEvent(&event);

    return (abs(event.orientation.pitch) > Settings::TILT_ANGLE_MAX || abs(event.orientation.roll) > Settings::TILT_ANGLE_MAX);
  }
}
