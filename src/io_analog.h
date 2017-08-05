#ifndef io_analog_h
#define io_analog_h

#include <Adafruit_ADS1015.h>

/**
* Analog to Digital converter, used to read battery voltage, charge voltage, cutter motor load, and more.
*/
class IO_Analog {
  public:
    IO_Analog();

    /**
    * Get the voltage readings from the specified channel.
    * There is a 8 ms delay built into this method, so calling this method too often will limit the speed of your program!
    */
    float getChannelVoltage(uint8_t channel);

  private:
    Adafruit_ADS1115 adc;
    //Adafruit_ADS1015 adc;     // Use this for the 12-bit version
};

#endif