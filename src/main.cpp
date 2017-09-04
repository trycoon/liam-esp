#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "definitions.h"
#include "settings.h"
#include "resources.h"
#include "io_analog.h"
#include "io_digital.h"
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
#include "api.h"

/*
 * Software to control a LIAM robot mower using a NodeMCU/ESP-12E (or similar ESP8266) microcontroller.
 */

extern "C" {
  #include "user_interface.h" // needed for system_update_cpu_freq()
}

IO_Analog io_analog;
IO_Digital io_digital;
IO_Accelerometer io_accelerometer;
WiFi_Client wifi;
OTA ota(wifi);
Wheel leftWheel(Settings::LEFT_WHEEL_MOTOR_PIN, Settings::LEFT_WHEEL_MOTOR_DIRECTION_PIN, Settings::LEFT_WHEEL_MOTOR_INVERTED, Settings::LEFT_WHEEL_MOTOR_SPEED);
Wheel rightWheel(Settings::RIGHT_WHEEL_MOTOR_PIN, Settings::RIGHT_WHEEL_MOTOR_DIRECTION_PIN, Settings::RIGHT_WHEEL_MOTOR_INVERTED, Settings::RIGHT_WHEEL_MOTOR_SPEED);
Cutter cutter(io_analog, io_digital);
BWF bwf;
Battery battery(io_analog);
GPS gps;
WheelController wheelController(leftWheel, rightWheel);
Resources resources(wifi, wheelController, cutter, bwf, battery, gps);
StateController stateController(Definitions::MOWER_STATES::DOCKED, resources);  // initialize state controller, assume we are DOCKED to begin with.
Metrics metrics(battery, cutter, gps);
Api api(stateController, wheelController, battery, cutter, gps, io_accelerometer, metrics);


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
  system_update_cpu_freq(80);   // Run CPU at 80 MHz, default.
  //system_update_cpu_freq(160);  // Run CPU at 160 MHz, untested.

  Serial.begin(115200);
  Serial.print(Definitions::APP_NAME);
  Serial.print(" v");
  Serial.println(Definitions::APP_VERSION);

  scan_I2C();
  api.setupApi(wifi.getWebServer());
  wifi.connect();
  ota.start();
}

void loop() {
  ota.handle();

  // always check if we are flipped.
  if (io_accelerometer.isFlipped() && stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::FLIPPED) {
    stateController.setState(Definitions::MOWER_STATES::FLIPPED);
  }

  yield();
  stateController.getStateInstance()->process();
  yield();
  wheelController.process();
  yield();
  battery.process();
  yield();
  cutter.process();
  yield();
  metrics.process();

  yield();
}
