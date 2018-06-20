#include <Arduino.h>
#include <EEPROM.h>
#include "io_accelerometer.h"
#include "configuration.h"
#include "settings.h"

// https://github.com/kriswiner/EM7180_SENtral_sensor_hub/wiki/A.-A-Short-Survey-of-Sensor-Fusion-Solutions

IO_Accelerometer::IO_Accelerometer(TwoWire& w): _Wire(w) {
  accel = new Adafruit_BNO055(w, 10, BNO055_ADDRESS_B);
}

void IO_Accelerometer::initialize() {

  if (!initialized) {
    available = accel->begin();

    initialized = true;

    if (!available) {
      Serial.println("No BNO055 device detected... Check your wiring or I2C ADDR!");
      return;
    }

    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;

    accel->getSensor(&sensor);
    // Check if we have calibration settings available for this sensor. This will not be true at first time use,
    // sensor replacement or if we do a "factory reset".
    if (Configuration::getInt("ACCEL_CALIB", -1) != sensor.sensor_id) {
      Serial.println("This BNO055 sensor has not been calibrated.");
      isCalibrated = false;
      return;
    }

    Serial.println("Found calibration for BNO055 sensor.");
    EEPROM.get(0, calibrationData);
    accel->setSensorOffsets(calibrationData);
    isCalibrated = true;
    //Crystal must be configured AFTER loading calibration data into BNO055.
    accel->setExtCrystalUse(true);
  }
}

bool IO_Accelerometer::needCalibration() {

  if (initialized && available && !isCalibrated) {
    if (!accel->isFullyCalibrated()) {
      sensors_event_t event;
      accel->getEvent(&event);
      delay(100);
      return true;
    } else {
      // we are now calibrated.
      adafruit_bno055_offsets_t newCalib;
      accel->getSensorOffsets(newCalib);

      sensor_t sensor;
      accel->getSensor(&sensor);

      EEPROM.put(0, newCalib);
      Configuration::set("ACCEL_CALIB", sensor.sensor_id);
      isCalibrated = true;

      Serial.println("Calibration of compass done, saved calibration to flash memory.");
    }
  }

  return false;
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
