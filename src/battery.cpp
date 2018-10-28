#include <math.h>
#include <sys/time.h>
#include <Preferences.h>
#include "battery.h"
#include "settings.h"
#include "configuration.h"

Battery::Battery(IO_Analog& io_analog) : io_analog(io_analog) {
  // Set initial state.
  updateReadings();
  // update readings every 100 ms.
  batteryReadingTicker.attach_ms<Battery*>(100, [](Battery* instance) {
    instance->updateReadings();
  }, this);
}

int64_t Battery::getEpocTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

void Battery::updateReadings() {
  float adc_reading = io_analog.getVoltage(Settings::BATTERY_SENSOR_PIN);
  batteryVoltage = roundf((adc_reading * Settings::BATTERY_MULTIPLIER) * 100) / 100;  // adjust reading and round to two decimals.

  adc_reading = io_analog.getVoltage(Settings::CHARGER_SENSOR_PIN);
  chargerVoltage = roundf((adc_reading * Settings::CHARGER_MULTIPLIER) * 100) / 100;  // adjust reading and round to two decimals.

  _needRecharge = batteryVoltage <= Settings::BATTERY_EMPTY;
  _isFullyCharged = batteryVoltage >= Settings::BATTERY_FULLY_CHARGED;

  // if we detect more than 10 volts on charge pins then assume we are charging.
  if (!_isCharging && !_isFullyCharged && chargerVoltage >= 10.0) {
    _isCharging = true;
    // don't overwrite already existing starttime, we could have been charging with mower turned off.
    
    if (Configuration::config.startChargeTime == 0) {
      Configuration::config.startChargeTime = getEpocTime();
      Configuration::save();
    }
  } else if (_isCharging && chargerVoltage < 10.0) {
    _isCharging = false;
    Configuration::config.startChargeTime = 0;  // wipe starttime if charging was aborted.
    Configuration::save();
  }

  if (_isCharging && _isFullyCharged) {
    _isCharging = false;
    auto currMillis = getEpocTime();
    Configuration::config.lastFullyChargeTime = currMillis;
    Configuration::config.lastChargeDuration = currMillis - Configuration::config.startChargeTime;

    Configuration::config.startChargeTime = 0;
    Configuration::save();
  }
}

float Battery::getChargerVoltage() {
  return chargerVoltage;
}

float Battery::getBatteryVoltage() {
  return batteryVoltage;
}

/*
* Get battery status in percent, 100% = fully charged.
*/
uint8_t Battery::getBatteryStatus() {
  return round((batteryVoltage - Settings::BATTERY_EMPTY) / (Settings::BATTERY_FULLY_CHARGED - Settings::BATTERY_EMPTY) * 100);
}

bool Battery::isCharging() {
  return _isCharging;
}

bool Battery::needRecharge() {
  return _needRecharge;
}

bool Battery::isFullyCharged() {
  return _isFullyCharged;
}

uint32_t Battery::getLastFullyChargeTime() {
  return Configuration::config.lastFullyChargeTime;
}

uint32_t Battery::getLastChargeDuration() {
  return Configuration::config.lastChargeDuration;
}
