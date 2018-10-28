#include <Arduino.h>
#include <Wire.h>
#include <rom/rtc.h>
#include <ArduinoLog.h>
#include "definitions.h"
#include "configuration.h"
#include "settings.h"
#include "log_store.h"
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
 * 
 * Congratulation, you have just found the starting point of the program!
 */

// Give us an warning if main loop is delayed more than this value. This could be an indication of hidden "delay" calls in our code.
const uint32_t LOOP_DELAY_WARNING = 200000; // 200 ms
// Don't spam us with warnings, wait this period before issuing a new warning
const uint32_t LOOP_DELAY_WARNING_COOLDOWN = 10000000; // 10 sec

// Setup references between all classes.
LogStore logstore;
IO_Analog io_analog;
IO_Accelerometer io_accelerometer(Wire);
WiFi_Client wifi;
OTA ota(wifi);
Wheel leftWheel(1, Settings::LEFT_WHEEL_MOTOR_PIN, Settings::LEFT_WHEEL_MOTOR_DIRECTION_PIN, Settings::LEFT_WHEEL_MOTOR_INVERTED, Settings::LEFT_WHEEL_MOTOR_SPEED);
Wheel rightWheel(2, Settings::RIGHT_WHEEL_MOTOR_PIN, Settings::RIGHT_WHEEL_MOTOR_DIRECTION_PIN, Settings::RIGHT_WHEEL_MOTOR_INVERTED, Settings::RIGHT_WHEEL_MOTOR_SPEED);
WheelController wheelController(leftWheel, rightWheel, io_accelerometer);
Cutter cutter(io_analog);
BWF bwf;
GPS gps;
Battery battery(io_analog);
Metrics metrics(battery, gps);
Resources resources(wifi, wheelController, cutter, bwf, battery, gps, io_accelerometer, metrics, logstore);
StateController stateController(resources);
Api api(stateController, resources);

uint64_t loopDelayWarningTime;

void scan_I2C() {
  byte error, address;
  int devices = 0;

  Log.trace(F("Scanning for I2C devices..." CR));

  for (address = 1; address < 127; address++ ) {

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Log.trace(F("I2C device found at address %X" CR), address);
      devices++;
    } else if (error == 4) {
      Log.warning(F("Unknown error at address %X" CR), address);
    }
  }

  if (devices == 0) {
    Log.warning(F("No I2C devices found"));
  } else {
    Log.trace(F("scanning done." CR));
  }
}

void setup() {
  logstore.begin(115200);

  logstore.printf("\n=== %s v%s ===\n\n", Definitions::APP_NAME, Definitions::APP_VERSION);

  Configuration::load();
  
  // setup Log library so that we can start logging events using e.g. Log.notice()...
  Log.begin(Configuration::config.logLevel, &logstore, true);

  // setup I2C
  Wire.begin(Settings::SDA_PIN, Settings::SCL_PIN);
  Wire.setTimeout(500); // milliseconds
  delay(100);
  scan_I2C();

  io_accelerometer.start();

  auto lastState = Configuration::config.lastState;
  // initialize state controller, assume we are DOCKED unless there is a saved state.
  if (rtc_get_reset_reason(0) == SW_CPU_RESET && lastState.length() > 0) {
    Log.notice(F("Returning to last state \"%s\" after software crash!" CR), lastState.c_str());
    stateController.setState(lastState);
  } else {
    stateController.setState(Definitions::MOWER_STATES::DOCKED);
  }

  wifi.start();

  if (Configuration::config.setupDone) {
    api.setupApi();
  } else {
    Log.notice(F("Starting mower for first time. Please connect to WiFi accesspoint \"%s\" and run installation wizard!" CR), Definitions::APP_NAME);
  }

  ota.start();
}

// Main program loop
void loop() {
  uint64_t loopStartTime = esp_timer_get_time();
  
  ota.handle();
  wifi.process();
  
  if (Configuration::config.setupDone) {
    // always check if we are flipped.
    if (io_accelerometer.isFlipped() && stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::FLIPPED) {
      stateController.setState(Definitions::MOWER_STATES::FLIPPED);
    }

    stateController.getStateInstance()->process();
    wheelController.process();
    metrics.process();
  }

  uint64_t currentTime = esp_timer_get_time();
  uint32_t loopDelay = currentTime - loopStartTime;

  if (loopDelay > LOOP_DELAY_WARNING && (currentTime - loopDelayWarningTime) > LOOP_DELAY_WARNING_COOLDOWN) {
    loopDelayWarningTime = currentTime;

    Log.warning(F("Main loop running slow due to long delay(%l us)! Make sure thread is not blocked by delays()." CR), (uint32_t)loopDelay);
  }
}
