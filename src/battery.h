#ifndef _battery_h
#define _battery_h

#include <string>
#include <Arduino.h>
#include "io_analog.h"
#include "scheduler/scheduler.h"
#include "processable.h"

class Battery : public Processable {
  public:
    Battery(IO_Analog& io_analog);
    float getChargerVoltage();
    float getBatteryVoltage();
    bool isCharging();
    bool needRecharge();
    bool isFullyCharged();
    unsigned long lastBatteryChargePeriod();
    void process();

  private:
    IO_Analog& io_analog;
    float chargerVoltage;
    float batteryVoltage;
    bool _isCharging;
    bool _needRecharge;
    bool _isFullyCharged;
    unsigned long _lastBatteryChargePeriod;
    unsigned long _lastBatteryStartChargeTimestamp;
    Scheduler pollTimer;
    uint32_t chargeStartTime;
    void updateReadings();
};

#endif
