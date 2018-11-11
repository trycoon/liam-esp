#ifndef io_analog_h
#define io_analog_h

#include <Arduino.h>

/**
* Analog to Digital converter, used to read battery voltage, charge voltage, cutter motor load, and more.
*/
class IO_Analog {
  public:
    IO_Analog();

    /**
    * Get the voltage readings from the specified pin.
    */
    float getVoltage(uint8_t pin);
};

#endif
