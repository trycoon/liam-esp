#include <ArduinoLog.h>
#include <math.h>
#include "battery.h"
#include "definitions.h"
#include "configuration.h"
#include "utils.h"

Battery::Battery(IO_Analog& io_analog, TwoWire& w) : io_analog(io_analog), wire(w), lastChargeCurrentReading(0) {}

void Battery::start() {
  
  ina219.begin(&wire);
  
  // Set initial state.
  for (auto i = 0; i < CURRENT_MEDIAN_SAMPLES; i++) {
    updateChargeCurrent();
  }

  updateBatteryVoltage();
  Log.trace("Battery voltage: %F volt, charge current: %F mA" CR, batteryVoltage, lastChargeCurrentReading);

  // update battery voltage readings every XX second.
  batteryVoltageTicker.attach<Battery*>(BATTERY_VOLTAGE_DELAY, [](Battery* instance) {
    instance->updateBatteryVoltage();
  }, this);
  // update battery charge current readings every XXX milliseconds.
  chargeCurrentTicker.attach_ms<Battery*>(BATTERY_CHARGECURRENT_DELAY, [](Battery* instance) {
    instance->updateChargeCurrent();
  }, this);
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
  batterySample sample = {
    time: Utils::getEpocTime(),
    batteryVoltage: batteryVoltage
  };

  batterySamples.push_back(sample);
}

void Battery::updateChargeCurrent() {

  auto chargeCurrent = ina219.getCurrent_mA();
  _isDocked = ina219.getBusVoltage_V() > 5;

  currentMedian[currentMedianIndex++ % CURRENT_MEDIAN_SAMPLES] = chargeCurrent;  // enter new reading into array.
  // we can get some missreadings (1475.10) from time to time, so we record samples to an array an take the median value to filter out all noice.
  chargeCurrent = Utils::calculateMedian<float>(currentMedian);

  _isCharging = chargeCurrent >= Definitions::CHARGE_CURRENT_THRESHOLD;
  // if we just started charging
  if (_isCharging && lastChargeCurrentReading < Definitions::CHARGE_CURRENT_THRESHOLD) {
    Log.notice("Start charging battery." CR);
    Log.trace("Charge current: %F mA" CR, chargeCurrent);

    // don't overwrite already existing starttime, we could have been charging with mower turned off.
    if (Configuration::config.startChargeTime == 0) {
      Configuration::config.startChargeTime = Utils::getEpocTime();
      Configuration::save();
    }
  } else if (!_isCharging && lastChargeCurrentReading >= Definitions::CHARGE_CURRENT_THRESHOLD) {
    
    if (_isFullyCharged) {
      Log.notice("Done charging battery." CR);
      auto currEpocSeconds = Utils::getEpocTime();
      Configuration::config.lastFullyChargeTime = currEpocSeconds;
      if (Configuration::config.startChargeTime > 0) {
        Configuration::config.lastChargeDuration = currEpocSeconds - Configuration::config.startChargeTime;
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

float Battery::getChargeCurrent() const {
  // don't bother reporting stray-current when there is no charging going on.
  return lastChargeCurrentReading > 1.0 ? lastChargeCurrentReading : 0;
}

/*
* Get battery status in percent, 100% = fully charged.
*/
uint8_t Battery::getBatteryStatus() const {

  auto level = round((batteryVoltage - Definitions::BATTERY_EMPTY) / (Definitions::BATTERY_FULLY_CHARGED - Definitions::BATTERY_EMPTY) * 100);
  if (level < 0) {
    level = 0;
  } else if (level > 100) {
    level = 100;
  }

  return level;
}

bool Battery::isDocked() const {
  return _isDocked;
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
