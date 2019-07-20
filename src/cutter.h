#ifndef _cutter_h
#define _cutter_h

#include <Arduino.h>
#include <Ticker.h>
#include "io_analog.h"
#include "processable.h"


/**
* Class handling the cutter (the motor with knives) of the mower
*/
class Cutter : public Processable {
  public:
    Cutter(IO_Analog& io_analog);
    ~Cutter();
    void start();
    void stop(bool brake = true);
    bool isCutting();
    uint8_t getLoad();
    /* Internal use only! */
    void process();

  private:
    static const uint8_t LOAD_MEDIAN_SAMPLES = 5; // How many samples should we take to calculate a median value for cutter load. Don't fiddle with this unless needed.

    const uint8_t cutter_id;
    IO_Analog& io_analog;
    uint8_t cutterSpeed = 0;        // target speed
    uint8_t cutterCurrentSpeed = 0; // current speed, when ramping up to cutterSpeed.
    long cutterLastSpeedRamp = 0;
    uint8_t load = 0;
    uint8_t loadMedian[LOAD_MEDIAN_SAMPLES] = {0};
    uint8_t loadMedianIndex = 0;
    Ticker cutterLoadReadingTicker;
    void senseLoad();
    void setCutterSpeed(uint8_t speed);
};

#endif
