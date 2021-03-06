#ifndef resources_h
#define resources_h

#include "wheel_controller.h"
#include "cutter.h"
#include "battery.h"
#include "gps.h"
#include "sonar.h"
#include "configuration.h"
#include "io_accelerometer/io_accelerometer.h"
#include "log_store.h"
#include "mowing_schedule.h"


/**
* Container class for holding references to instances that should be shared between many classes.
* This is so that we can reduce the number of parameters each method must take, but also making it easier to add additional references in the future.
*/
class Resources {
  public:
    Resources(             WheelController& wheelController,
                           Cutter& cutter,
                           Battery& battery,
                           GPS& gps,
                           Sonar& sonar,
                           IO_Accelerometer& accelerometer,
                           LogStore& logStore,
                           MowingSchedule& mowingSchedule)
                           : wheelController(wheelController),
                             cutter(cutter),
                             battery(battery),
                             gps(gps),
                             sonar(sonar),
                             accelerometer(accelerometer),
                             logStore(logStore),
                             mowingSchedule(mowingSchedule) { }

    WheelController& wheelController;
    Cutter& cutter;
    Battery& battery;
    GPS& gps;
    Sonar& sonar;
    IO_Accelerometer& accelerometer;
    LogStore& logStore;
    MowingSchedule& mowingSchedule;
};

#endif
