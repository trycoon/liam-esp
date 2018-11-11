#include <Wire.h>

#include "io_analog.h"
#include "definitions.h"
#include "esp32-hal-adc.h"

// ADS1115 external I2C ADC:
// http://arduinotronics.blogspot.se/2015/05/reading-current-shunt-with-arduino.html
// https://learn.adafruit.com/adafruit-4-channel-adc-breakouts/programming
// http://henrysbench.capnfatz.com/henrys-bench/arduino-voltage-measurements/arduino-ads1115-module-getting-started-tutorial/
// https://www.spiria.com/en/blog/iot-m2m-embedded-solutions/measuring-small-currents-adc

// ESP32 ADC information and issues:
// http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/adc.html
// https://www.esp32.com/viewtopic.php?t=1045
IO_Analog::IO_Analog() {

  // max pin voltage is 3.3v!
  analogSetAttenuation(adc_attenuation_t::ADC_11db);
}

float IO_Analog::getVoltage(uint8_t pin) {
  //TODO: use capacitor to GND to filter noise.
  double reading = analogRead(pin);
  if (reading < 1 || reading > 4095) {
    return 0;
  } 
  // https://www.youtube.com/watch?v=RlKMJknsNpo
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
}
