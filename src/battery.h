#ifndef _battery_h
#define _battery_h

#include<string>
#include <Arduino.h>
#include "io_analog.h"

class Battery {
  public:
    Battery(IO_Analog& io_analog);
    std::string getChargerVoltage();
    std::string getBatteryVoltage();
    bool isCharging();
    bool needRecharge();
    bool isFullyCharged();
    std::string lastBatteryRunTime();
    std::string lastBatteryChargeTime();

  private:
    IO_Analog& io_analog;
    uint16_t chargerVoltage;
    uint16_t batteryVoltage;
    uint32_t lastBatteryChargeTimestamp;
    uint32_t lastBatteryChargeElapseTimestamp;
};

#endif
