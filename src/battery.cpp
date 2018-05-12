#include "battery.h"
#include "settings.h"

Battery::Battery(IO_Analog& io_analog) : io_analog(io_analog) {
  // Set initial state.
  updateReadings();
  // update readings every 100 ms.
  pollTimer.schedule([this]() {
    updateReadings();
  }, 100, true);
}

void Battery::updateReadings() {
  float battery = io_analog.getChannelVoltage(Settings::BATTERY_SENSOR_CHANNEL);
  batteryVoltage = battery / Settings::BATTERY_RESISTOR_DIVISOR;

  // TODO: only sample charger when we are in states DOCKING and DOCKED.
  float charger = io_analog.getChannelVoltage(Settings::CHARGER_SENSOR_CHANNEL);
  chargerVoltage = charger / Settings::CHARGER_RESISTOR_DIVISOR;

  _needRecharge = batteryVoltage <= Settings::BATTERY_EMPTY;
  _isFullyCharged = batteryVoltage >= Settings::BATTERY_FULLY_CHARGED;

  // if we detect more than 10 volts on charge pins then assume we are charging.
  if (!_isCharging && !_isFullyCharged && chargerVoltage >= 10.0) {
    _isCharging = true;
    //TODO: save timestamp when we started charging
  } else if (_isCharging && chargerVoltage < 10.0) {
    _isCharging = false;
  }

  if (_isCharging && _isFullyCharged) {
    _isCharging = false;
    //TODO: save timestamp when we stopped charging
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
float Battery::getBatteryStatus() {
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

void Battery::process() {
  pollTimer.process();
}
