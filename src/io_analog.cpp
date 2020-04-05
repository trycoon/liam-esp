#include "io_analog.h"

// Shunt sizing:
// https://www.spiria.com/en/blog/iot-m2m-embedded-solutions/measuring-small-currents-adc

// http://henrysbench.capnfatz.com/henrys-bench/arduino-voltage-measurements/arduino-ads1115-module-getting-started-tutorial/
// https://learn.adafruit.com/adafruit-4-channel-adc-breakouts/arduino-code

IO_Analog::IO_Analog() : adc1(Definitions::ADC1_ADDR), adc2(Definitions::ADC2_ADDR) {

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // adc.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // adc.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  adc1.setGain(GAIN_TWO);          // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // adc.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // adc.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // adc.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  adc2.setGain(GAIN_TWO);          // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  adc2.startContinuous_Differential_0_1();  // Hardwired for monitoring charge current.
}

float IO_Analog::getVoltageAdc1(uint8_t channel) {

  return adc1.readADC_SingleEnded_V(channel);

}

float IO_Analog::getChargeCurrent() {

  return ((float) adc2.getLastConversionResults()) * adc2.voltsPerBit() / Definitions::CHARGE_SHUNT_VALUE;

}
