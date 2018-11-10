#include <math.h>
#include <sys/time.h>
#include <Preferences.h>
#include <ArduinoLog.h>
#include "battery.h"
#include "definitions.h"
#include "configuration.h"

Battery::Battery(IO_Analog& io_analog, TwoWire& w) : io_analog(io_analog), wire(w), lastChargeCurrentReading(0) {}

void Battery::start() {
  ina219.begin(&wire);

  // Set initial state.
  updateChargeCurrent();
  updateBatteryVoltage();
  Log.trace("Battery voltage: %Fv, charge current: %FmA" CR, batteryVoltage, lastChargeCurrentReading);

  // update battery voltage readings every XX second.
  batteryVoltageTicker.attach<Battery*>(BATTERY_VOLTAGE_DELAY, [](Battery* instance) {
    instance->updateBatteryVoltage();
  }, this);
  // update battery charge current readings every XXX milliseconds.
  chargeCurrentTicker.attach_ms<Battery*>(BATTERY_CHARGECURRENT_DELAY, [](Battery* instance) {
    instance->updateChargeCurrent();
  }, this);
}

int64_t Battery::getEpocTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

void Battery::updateBatteryVoltage() {
  float adc_reading = io_analog.getVoltage(Definitions::BATTERY_SENSOR_PIN);
  batteryVoltage = roundf((adc_reading * Definitions::BATTERY_MULTIPLIER) * 100) / 100;  // adjust reading and round to two decimals.

  _needRecharge = batteryVoltage <= Definitions::BATTERY_EMPTY;
  _isFullyCharged = batteryVoltage >= Definitions::BATTERY_FULLY_CHARGED && !_isCharging;

  // make sure sample list don't grow larger than MAX_SAMPLES. Older samples are removed.
  if (batterySamples.size() >= MAX_SAMPLES) {
    batterySamples.pop_front();
  }
  batterySample sample;
  sample.time = getEpocTime();
  sample.batteryVoltage = batteryVoltage;
  batterySamples.push_back(sample);
}

void Battery::updateChargeCurrent() {
  auto chargeCurrent = ina219.getCurrent_mA();

  _isCharging = chargeCurrent >= Definitions::CHARGE_CURRENT_THRESHOLD;
  // if we just started charging
  if (_isCharging && lastChargeCurrentReading < Definitions::CHARGE_CURRENT_THRESHOLD) {
    Log.notice("Start charging battery." CR);
    
    // don't overwrite already existing starttime, we could have been charging with mower turned off.
    if (Configuration::config.startChargeTime == 0) {
      Configuration::config.startChargeTime = getEpocTime();
      Configuration::save();
    }
  } else if (!_isCharging && lastChargeCurrentReading >= Definitions::CHARGE_CURRENT_THRESHOLD) {
    
    if (_isFullyCharged) {
      Log.notice("Done charging battery." CR);
      auto currMillis = getEpocTime();
      Configuration::config.lastFullyChargeTime = currMillis;
      if (Configuration::config.startChargeTime > 0) {
        Configuration::config.lastChargeDuration = currMillis - Configuration::config.startChargeTime;
      }
    } else {
      Log.notice("Charging of battery was aborted." CR);
    }
    // wipe starttime if charging was aborted or if we are done.
    Configuration::config.startChargeTime = 0;
    Configuration::save();
  }

  lastChargeCurrentReading = chargeCurrent;
}

float Battery::getBatteryVoltage() const {
  return batteryVoltage;
}

/*
* Get battery status in percent, 100% = fully charged.
*/
uint8_t Battery::getBatteryStatus() const {
  return round((batteryVoltage - Definitions::BATTERY_EMPTY) / (Definitions::BATTERY_FULLY_CHARGED - Definitions::BATTERY_EMPTY) * 100);
}

bool Battery::isCharging() const {
  return _isCharging;
}

bool Battery::needRecharge() const {
  return _needRecharge;
}

bool Battery::isFullyCharged() const {
  return _isFullyCharged;
}

uint32_t Battery::getLastFullyChargeTime() const {
  return Configuration::config.lastFullyChargeTime;
}

uint32_t Battery::getLastChargeDuration() const {
  return Configuration::config.lastChargeDuration;
}

const std::deque<batterySample>& Battery::getBatteryHistory() const {
  return batterySamples;
}
