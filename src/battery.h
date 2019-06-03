#ifndef _battery_h
#define _battery_h

#include <string>
#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include <deque>
#include <vector>
#include <Adafruit_INA219.h>
#include "io_analog.h"

struct batterySample {
  uint32_t time;
  float batteryVoltage;
};

class Battery {
  public:
    Battery(IO_Analog& io_analog, TwoWire& w);
    float getBatteryVoltage() const;
    uint8_t getBatteryStatus() const;
    uint32_t getLastFullyChargeTime() const;
    uint32_t getLastChargeDuration() const;
    const std::deque<batterySample>& getBatteryHistory() const;
    bool isDocked() const;
    bool isCharging() const;
    bool needRecharge() const;
    bool isFullyCharged() const;
    void start();

  private:
    static const uint16_t MAX_SAMPLES = 100;   // How much history are we going to keep? set too high will consume excessive memory and we may get out-of-memory related errors.
    static const uint16_t BATTERY_CHARGECURRENT_DELAY = 100; // milliseconds
    static const uint16_t BATTERY_VOLTAGE_DELAY = 20;  // seconds
    IO_Analog& io_analog;
    TwoWire& wire;
    Adafruit_INA219 ina219;
    float batteryVoltage;
    float lastChargeCurrentReading;
    bool _isDocked;
    bool _isCharging;
    bool _needRecharge;
    bool _isFullyCharged;
    std::vector<float> currentMedian;
    uint8_t currentMedianIndex;
    void updateBatteryVoltage();
    void updateChargeCurrent();
    Ticker batteryVoltageTicker;
    Ticker chargeCurrentTicker;
    std::deque<batterySample> batterySamples;
};

#endif
