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
    float getBatteryStatus();
    bool isCharging();
    bool needRecharge();
    bool isFullyCharged();
    void process();

  private:
    IO_Analog& io_analog;
    float chargerVoltage;
    float batteryVoltage;
    bool _isCharging;
    bool _needRecharge;
    bool _isFullyCharged;
    Scheduler pollTimer;
    void updateReadings();
};

#endif
