#include <Wire.h>

#include "io_analog.h"
#include "settings.h"
#include "esp32-hal-adc.h"

// ADS1115 external I2C ADC:
// http://arduinotronics.blogspot.se/2015/05/reading-current-shunt-with-arduino.html
// https://learn.adafruit.com/adafruit-4-channel-adc-breakouts/programming
// http://henrysbench.capnfatz.com/henrys-bench/arduino-voltage-measurements/arduino-ads1115-module-getting-started-tutorial/
// https://www.spiria.com/en/blog/iot-m2m-embedded-solutions/measuring-small-currents-adc

// http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/adc.html
// https://www.esp32.com/viewtopic.php?t=1045
// https://www.youtube.com/watch?v=RlKMJknsNpo
IO_Analog::IO_Analog() {

  // gives full-scale voltage 1.1V (100 and 950mV), max voltage is 3.3v everything between 1.1 and 3.3v will give max value.
  analogSetAttenuation(adc_attenuation_t::ADC_0db);
}

float IO_Analog::getVoltage(uint8_t pin) {
  //TODO: use capacitor to GND to filter noise.
  return analogRead(pin) * VOLTAGE_CONVERSION;
}
