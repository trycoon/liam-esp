#ifndef _cutter_h
#define _cutter_h

#include <Arduino.h>
#include "io_analog.h"
#include "io_digital.h"
#include "scheduler/scheduler.h"
#include "processable.h"

/**
* Class handling the cutter (the motor with knives) of the mower
*/
class Cutter : public Processable {
  public:
    Cutter(IO_Analog& io_analog, IO_Digital& io_digital);
    ~Cutter();
    void start();
    void stop(bool brake);
    bool isCutting();
    uint8_t getLoad();
    void process();
    
  private:
    IO_Analog& io_analog;
    IO_Digital& io_digital;
    bool cutting;
    uint8_t load;
    Scheduler senseLoadTimer;
    uint16_t senseLoadTimerId;
    void senseLoad();
};

#endif
