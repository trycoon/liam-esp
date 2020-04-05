#ifndef io_analog_h
#define io_analog_h

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "definitions.h"


/**
* Analog to Digital converter, used to read battery voltage, charge voltage, cutter motor load, and more.
*/
class IO_Analog {
  public:
    IO_Analog();

    /**
    * Get the voltage readings from the specified channel of ADC #1.
    */
    float getVoltageAdc1(uint8_t channel);
    float getChargeCurrent();

  private:
    Adafruit_ADS1115 adc1;
    Adafruit_ADS1115 adc2;
};

#endif
