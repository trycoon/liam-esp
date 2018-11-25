#include <math.h>
#include <sys/time.h>
#include <Preferences.h>
#include <ArduinoLog.h>
#include <vector>
#include "battery.h"
#include "definitions.h"
#include "configuration.h"

Battery::Battery(IO_Analog& io_analog, TwoWire& w) : io_analog(io_analog), wire(w), lastChargeCurrentReading(0), currentMedian(11, 0), currentMedianIndex(0) {}

void Battery::start() {
  ina219.begin(&wire);
  
  // Set initial state.
  for (auto i: currentMedian) {
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

int64_t Battery::getEpocTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

template<typename T>
T calculateMedian(std::vector<T> entries) {
  size_t size = entries.size();

  if (size == 0) {
    return 0;  // Undefined, really.
  } else {
    sort(entries.begin(), entries.end());
    if (size % 2 == 0) {
      return (entries[size / 2 - 1] + entries[size / 2]) / 2;
    } else {
      return entries[size / 2];
    }
  }
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
  currentMedian[currentMedianIndex++%currentMedian.size()] = chargeCurrent;  // enter new reading into array.
  // we can get some missreadings (1475.10) from time to time, so we record samples to an array an take the median value to filter out all noice.
  chargeCurrent = calculateMedian<float>(currentMedian);

  _isCharging = chargeCurrent >= Definitions::CHARGE_CURRENT_THRESHOLD;
  // if we just started charging
  if (_isCharging && lastChargeCurrentReading < Definitions::CHARGE_CURRENT_THRESHOLD) {
    Log.notice("Start charging battery." CR);
    Log.trace("Charge current: %F mA" CR, chargeCurrent);

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
  auto level = round((batteryVoltage - Definitions::BATTERY_EMPTY) / (Definitions::BATTERY_FULLY_CHARGED - Definitions::BATTERY_EMPTY) * 100);
  return level > 100 ? 100 : level;
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
