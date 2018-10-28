#ifndef _battery_h
#define _battery_h

#include <string>
#include <Arduino.h>
#include <Ticker.h>
#include "io_analog.h"

class Battery {
  public:
    Battery(IO_Analog& io_analog);
    float getChargerVoltage();
    float getBatteryVoltage();
    uint8_t getBatteryStatus();
    uint32_t getLastFullyChargeTime();
    uint32_t getLastChargeDuration();
    bool isCharging();
    bool needRecharge();
    bool isFullyCharged();

  private:
    IO_Analog& io_analog;
    float chargerVoltage;
    float batteryVoltage;
    bool _isCharging;
    bool _needRecharge;
    bool _isFullyCharged;
    void updateReadings();
    int64_t getEpocTime();
    Ticker batteryReadingTicker;
};

#endif
