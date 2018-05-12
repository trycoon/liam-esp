#include <Arduino.h>
#include <Wire.h>
#include "definitions.h"
#include "settings.h"
#include "resources.h"
#include "io_analog.h"
#include "io_accelerometer.h"
#include "wifi.h"
#include "ota/ota.h"
#include "wheel_controller.h"
#include "wheel.h"
#include "cutter.h"
#include "bwf.h"
#include "battery.h"
#include "gps.h"
#include "metrics.h"
#include "state_controller.h"
#include "configuration.h"
#include "api.h"

/*
 * Software to control a LIAM robotmower using a NodeMCU/ESP-32 (or similar ESP32) microcontroller.
 */

IO_Analog io_analog;
IO_Accelerometer io_accelerometer;
WiFi_Client wifi;
OTA ota(wifi);
Wheel leftWheel(Settings::LEFT_WHEEL_MOTOR_PIN, Settings::LEFT_WHEEL_MOTOR_DIRECTION_PIN, Settings::LEFT_WHEEL_MOTOR_INVERTED, Settings::LEFT_WHEEL_MOTOR_SPEED);
Wheel rightWheel(Settings::RIGHT_WHEEL_MOTOR_PIN, Settings::RIGHT_WHEEL_MOTOR_DIRECTION_PIN, Settings::RIGHT_WHEEL_MOTOR_INVERTED, Settings::RIGHT_WHEEL_MOTOR_SPEED);
Cutter cutter(io_analog);
BWF bwf;
Battery battery(io_analog);
GPS gps;
WheelController wheelController(leftWheel, rightWheel, io_accelerometer);
Metrics metrics(battery, gps);
Configuration configuration;
Resources resources(wifi, wheelController, cutter, bwf, battery, gps, configuration, io_accelerometer, metrics);
StateController stateController(Definitions::MOWER_STATES::DOCKED, resources);  // initialize state controller, assume we are DOCKED to begin with.
Api api(stateController, resources);


void scan_I2C() {
  Wire.begin(Settings::SDA_PIN, Settings::SCL_PIN);
  byte error, address;
  int devices = 0;

  Serial.println("Scanning for I2C devices...");

  for (address = 1; address < 127; address++ ) {

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(address, HEX);
      devices++;

    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(address, HEX);
    }
  }

  if (devices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("scanning done.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.print(Definitions::APP_NAME);
  Serial.print(" v");
  Serial.println(Definitions::APP_VERSION);
  scan_I2C();
  api.setupApi(wifi.getWebServer());
  wifi.connect();
  ota.start();
}

// Main program loop
void loop() {
  ota.handle();

  // always check if we are flipped.
  if (io_accelerometer.isFlipped() && stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::FLIPPED) {
    stateController.setState(Definitions::MOWER_STATES::FLIPPED);
  }

  stateController.getStateInstance()->process();
  wheelController.process();
  battery.process();
  cutter.process();
  metrics.process();
}
