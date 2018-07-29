#ifndef _cutter_h
#define _cutter_h

#include <Arduino.h>
#include <Ticker.h>
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
    bool isCutting();
    uint8_t getLoad();

  private:
    const uint8_t CHANNEL = 0;
    IO_Analog& io_analog;
    bool cutting;
    uint8_t load;
    void senseLoad();
    Ticker cutterLoadReadingTicker;
};

#endif
