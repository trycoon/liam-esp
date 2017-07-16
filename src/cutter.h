#ifndef _cutter_h
#define _cutter_h

#include <Arduino.h>
#include "io_analog.h"

/**
* Class handling the cutter (the motor with knives) of the mower
*/
class Cutter {
  public:
    Cutter(IO_Analog& io_analog);
    ~Cutter();
    void setSpeed(uint8_t speed);
    void stop();
    uint16_t getLoad();
    
  private:
    IO_Analog& io_analog;
};

#endif
