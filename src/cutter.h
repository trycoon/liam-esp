#ifndef _cutter_h
#define _cutter_h

#include <Ticker.h>
#include <Arduino.h>
#include "io_analog.h"

/**
* Class handling the cutter (the motor with knives) of the mower
*/
class Cutter {
  public:
    Cutter(IO_Analog& io_analog);
    ~Cutter();
    void start();
    void stop(bool brake);
    uint16_t getLoad();

  private:
    IO_Analog& io_analog;
    Ticker brakeTimer;
};

#endif
