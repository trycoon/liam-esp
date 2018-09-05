#include "battery.h"
#include "settings.h"
#include "configuration.h"
#include "time.h"

Battery::Battery(IO_Analog& io_analog) : io_analog(io_analog) {
  // Set initial state.
  updateReadings();
  // update readings every 100 ms.
  batteryReadingTicker.attach_ms<Battery*>(100, [](Battery* instance) {
    instance->updateReadings();
  }, this);
}

void Battery::updateReadings() {
  time_t now;

  float battery = io_analog.getVoltage(Settings::BATTERY_SENSOR_PIN);
  batteryVoltage = roundf((battery / Settings::BATTERY_MULTIPLIER) * 100) / 100;  // adjust reading and round to two decimals.

  float charger = io_analog.getVoltage(Settings::CHARGER_SENSOR_PIN);
  chargerVoltage = roundf((charger / Settings::CHARGER_MULTIPLIER) * 100) / 100;  // adjust reading and round to two decimals.

  _needRecharge = batteryVoltage <= Settings::BATTERY_EMPTY;
  _isFullyCharged = batteryVoltage >= Settings::BATTERY_FULLY_CHARGED;

  // if we detect more than 10 volts on charge pins then assume we are charging.
  if (!_isCharging && !_isFullyCharged && chargerVoltage >= 10.0) {
    _isCharging = true;
    // don't overwrite already existing starttime, we could have been charging with mower turned off.
    if(Configuration::getInt("StartChargeTime", 0) == 0) {
      time(&now);
      Configuration::set("StartChargeTime", now);
    }
  } else if (_isCharging && chargerVoltage < 10.0) {
    _isCharging = false;
    Configuration::clear("StartChargeTime");  // wipe starttime if charging was aborted.
  }

  if (_isCharging && _isFullyCharged) {
    _isCharging = false;
    time(&now);
    Configuration::set("LastFullyChargeTime", now);
    Configuration::set("LastChargeDuration", now - Configuration::getInt("StartChargeTime", 0));
    Configuration::clear("StartChargeTime");
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
  return (batteryVoltage - Settings::BATTERY_EMPTY) / (Settings::BATTERY_FULLY_CHARGED - Settings::BATTERY_EMPTY);
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
  return Configuration::getInt("LastFullyChargeTime", 0);
}

uint32_t Battery::getLastChargeDuration() {
  return Configuration::getInt("LastChargeDuration", 0);
}
