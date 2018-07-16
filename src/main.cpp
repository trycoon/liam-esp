#include <Arduino.h>
#include <Wire.h>
#include "definitions.h"
#include "configuration.h"
#include "settings.h"
#include "resources.h"
#include "io_analog.h"
#include "io_accelerometer/io_accelerometer.h"
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
 * Software to control a LIAM robotmower using a NodeMCU/ESP-32 (or similar ESP32) microcontroller.
 */

// Give us an warning if main loop is delayed more than this value. This could be an indication of hidden "delay" calls in our code.
const uint32_t LOOP_DELAY_WARNING = 100000; // 100 ms
// Don't spam us with warnings, wait this period before issuing a new warning
const uint32_t LOOP_DELAY_WARNING_COOLDOWN = 10000000; // 10 sec

IO_Analog io_analog;
IO_Accelerometer io_accelerometer(Wire);
WiFi_Client wifi;
//OTA ota(wifi);
Wheel leftWheel(1, Settings::LEFT_WHEEL_MOTOR_PIN, Settings::LEFT_WHEEL_MOTOR_DIRECTION_PIN, Settings::LEFT_WHEEL_MOTOR_INVERTED, Settings::LEFT_WHEEL_MOTOR_SPEED);
Wheel rightWheel(2, Settings::RIGHT_WHEEL_MOTOR_PIN, Settings::RIGHT_WHEEL_MOTOR_DIRECTION_PIN, Settings::RIGHT_WHEEL_MOTOR_INVERTED, Settings::RIGHT_WHEEL_MOTOR_SPEED);
WheelController wheelController(leftWheel, rightWheel, io_accelerometer);
Cutter cutter(io_analog);
BWF bwf;
GPS gps;
Battery battery(io_analog);
Metrics metrics(battery, gps);
Resources resources(wifi, wheelController, cutter, bwf, battery, gps, io_accelerometer, metrics);
StateController stateController(Definitions::MOWER_STATES::DOCKED, resources);  // initialize state controller, assume we are DOCKED to begin with.
Api api(stateController, resources);

uint64_t loopDelayWarningTime;

void scan_I2C() {
  Wire.begin(Settings::SDA_PIN, Settings::SCL_PIN, 400000);
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

  io_accelerometer.start();
  wifi.start();
  api.setupApi(wifi.getWebServer());
  //ota.start();
}

// Main program loop
void loop() {
  uint64_t loopStartTime = esp_timer_get_time();
  //  ota.handle();

  io_accelerometer.process();
  
  // always check if we are flipped.
  if (io_accelerometer.isFlipped() && stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::FLIPPED) {
    stateController.setState(Definitions::MOWER_STATES::FLIPPED);
  }

  stateController.getStateInstance()->process();
  wheelController.process();
  battery.process();
  cutter.process();
  //metrics.process();
  
  uint64_t currentTime = esp_timer_get_time();
  uint32_t loopDelay = currentTime - loopStartTime;

  if (loopDelay > LOOP_DELAY_WARNING && (currentTime - loopDelayWarningTime) > LOOP_DELAY_WARNING_COOLDOWN) {
    loopDelayWarningTime = currentTime;

    Serial.print("Main loop running slow due to long delay(");
    Serial.print((uint32_t)loopDelay);
    Serial.println(" us)! Make sure thread is not blocked by delays().");
  }
}
