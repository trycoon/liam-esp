#ifndef _battery_h
#define _battery_h

#include <string>
#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include <deque>
#include "io_analog.h"

struct batterySample {
  uint32_t time;
  float batteryVoltage;
};

class Battery {
  public:
    Battery(IO_Analog& io_analog, TwoWire& w);
    float getBatteryVoltage() const;
    float getChargeCurrent() const;
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
    static const uint16_t BATTERY_CHARGECURRENT_DELAY = 100; // Read charge current every XXX milliseconds.
    static const uint16_t BATTERY_VOLTAGE_DELAY = 20;        // Read battery voltage every XXX seconds.
    static const uint8_t CURRENT_MEDIAN_SAMPLES = 11;        // How many samples should we take to calculate a median value for charge current. Don't fiddle with this unless needed.

    IO_Analog& io_analog;
    TwoWire& wire;
    float batteryVoltage = 0;
    float lastChargeCurrentReading = 0;
    bool _isDocked = false;
    bool _isCharging = false;
    bool _needRecharge = false;
    bool _isFullyCharged = false;
    float currentMedian[CURRENT_MEDIAN_SAMPLES] = {0};
    uint8_t currentMedianIndex = 0;
    void updateBatteryVoltage();
    void updateChargeCurrent();
    Ticker batteryVoltageTicker;
    Ticker chargeCurrentTicker;
    std::deque<batterySample> batterySamples;
};

#endif
