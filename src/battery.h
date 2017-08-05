#ifndef _battery_h
#define _battery_h

#include<string>
#include <Arduino.h>
#include <Ticker.h>
#include "io_analog.h"
#include "timer/timer.h"

class Battery {
  public:
    Battery(IO_Analog& io_analog);
    float getChargerVoltage();
    float getBatteryVoltage();
    bool isCharging();
    bool needRecharge();
    bool isFullyCharged();
    unsigned long lastBatteryChargePeriod();

  private:
    IO_Analog& io_analog;
    float chargerVoltage;
    float batteryVoltage;
    bool _isCharging;
    bool _needRecharge;
    bool _isFullyCharged;
    unsigned long _lastBatteryChargePeriod;
    unsigned long _lastBatteryStartChargeTimestamp;
    Ticker pollTimer;
    Timer chargeTimer;
    void updateReadings();
};

#endif
