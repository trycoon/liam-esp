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

  // if we detect more than 10 volts on charge pins then assume we are charging.
  if (_isCharging == false && chargerVoltage >= 10.0) {
    _isCharging = true;
    chargeStartTime = millis();
  } else if (_isCharging == true && chargerVoltage < 10.0) {
    _isCharging = false;
    _lastBatteryChargePeriod = millis() - chargeStartTime;
  }

  _needRecharge = batteryVoltage <= Settings::BATTERY_EMPTY;
  _isFullyCharged = batteryVoltage >= Settings::BATTERY_FULLY_CHARGED;
}

float Battery::getChargerVoltage() {
  return chargerVoltage;
}

float Battery::getBatteryVoltage() {
  return batteryVoltage;
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

/**
* The time that has elapsed between we started charging until we have finished charging, in milliseconds.
*/
unsigned long Battery::lastBatteryChargePeriod() {
  return _lastBatteryChargePeriod;
}
