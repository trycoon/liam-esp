#include <Wire.h>

#include "io_analog.h"
#include "definitions.h"
#include "esp32-hal-adc.h"

// Shunt sizing:
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
