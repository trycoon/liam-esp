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
#include "wifi/wifi.h"
#include "ota/ota.h"
#include "wheel_controller.h"
#include "wheel.h"
#include "cutter.h"
#include "bwf.h"
#include "battery.h"
#include "gps.h"
#include "state_controller.h"

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
  wifi.connect();
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

  // ESP.getCycleCount() // "returns the cpu instruction cycle count since start as an unsigned 32-bit."
  /*
    ESP.getCycleCount() counts the instruction cycles since start, in an internal unsigned 32-bit variable, liable to overflow every 28 seconds or so. A one microsecond period will give 160 instruction cycles. Now to measure the software overhead required by your software, you need to only count the instruction cycles your routine counts for a 1 microsecond delay and subtract 160 from that count, to give you the software overhead in acquiring the count. For 1 microsecond delay, I got a count of 213. Which worked out to 213-160 = 53 counts (53 x 6.25 = 331.25 nanoseconds) software overhead to acquire the count. Subtracting 53 from every count gives me a count accurate to within a few tens of picoseconds, for periods from 30 microseconds to about 500 microseconds.
  */
  // ESP.getFreeHeap() // "returns the free heap size."
  yield();

//TODO: remove
  if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::TEST) {
    stateController.setState(Definitions::MOWER_STATES::TEST);
  }
}
