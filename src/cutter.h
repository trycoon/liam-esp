#ifndef _cutter_h
#define _cutter_h

#include <Arduino.h>
#include <Ticker.h>
#include "io_analog.h"
#include "io_digital.h"

/**
* Class handling the cutter (the motor with knives) of the mower
*/
class Cutter {
  public:
    Cutter(IO_Analog& io_analog, IO_Digital& io_digital);
    ~Cutter();
    void start();
    void stop(bool brake);
    bool isCutting();
    uint8_t getLoad();

  private:
    IO_Analog& io_analog;
    IO_Digital& io_digital;
    bool cutting;
    uint8_t load;
    Ticker senseLoadTimer;
    void senseLoad();
};

#endif
