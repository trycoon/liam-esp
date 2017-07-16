#ifndef _controller_h
#define _controller_h

#include "mqtt/mqtt.h"
#include "wheel.h"
#include "cutter.h"
#include "bwf.h"
#include "battery.h"
#include "gps.h"
#include "definitions.h"
#include "resources.h"
#include "io_accelerometer.h"
#include "state_controller.h"

/**
* The main controller (or "brain" if you like) of the mower.
*/
class Controller {
  public:
    Controller(Resources& resources, IO_Accelerometer& io_accelerometer);
    void run();

  private:
    Resources& resources;
    IO_Accelerometer& io_accelerometer;
    StateController stateController;
};

#endif
