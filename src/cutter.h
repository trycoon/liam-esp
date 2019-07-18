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
    void stop(bool brake = true);
    bool isCutting();
    uint8_t getLoad();

  private:
    static const uint8_t LOAD_MEDIAN_SAMPLES = 5; // How many samples should we take to calculate a median value for cutter load. Don't fiddle with this unless needed.

    const uint8_t cutter_id;
    IO_Analog& io_analog;
    uint8_t cutterSpeed = 0;
    uint8_t load = 0;
    uint8_t loadMedian[LOAD_MEDIAN_SAMPLES] = {0};
    uint8_t loadMedianIndex = 0;
    void senseLoad();
    Ticker cutterLoadReadingTicker;
};

#endif
