#include <iomanip>
#include <sstream>
#include <ctime>
#include "battery.h"
#include "settings.h"

Battery::Battery(IO_Analog& io_analog) : io_analog(io_analog) {
  // http://arduinotronics.blogspot.se/2015/05/reading-current-shunt-with-arduino.html
  // https://learn.adafruit.com/adafruit-4-channel-adc-breakouts
  // https://www.spiria.com/en/blog/iot-m2m-embedded-solutions/measuring-small-currents-adc

  // http://henrysbench.capnfatz.com/henrys-bench/arduino-voltage-measurements/arduino-ads1115-differential-voltmeter-tutorial/

}

//https://github.com/Toshik/TickerScheduler
//https://github.com/Makuna/Task

std::string Battery::getChargerVoltage() {
  std::ostringstream out;
  out << std::fixed << std::setprecision(2) << chargerVoltage;
  return out.str();
}

std::string Battery::getBatteryVoltage() {
  std::ostringstream out;
  out << std::fixed << std::setprecision(2) << batteryVoltage;

  return out.str();
}

bool Battery::isCharging() {
  return false;
}

bool Battery::needRecharge() {
  return batteryVoltage < Settings::BATTERY_EMPTY;
}

bool Battery::isFullyCharged() {
  return batteryVoltage > Settings::BATTERY_FULLY_CHARGED;
}

std::string Battery::lastBatteryRunTime() {
  return "1:10:10";
}

std::string Battery::lastBatteryChargeTime() {
  std::time_t t = lastBatteryChargeTimestamp;
  char mbstr[100];
  // http://en.cppreference.com/w/cpp/chrono/c/strftime
  std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&t));

  return std::string(mbstr);
}
