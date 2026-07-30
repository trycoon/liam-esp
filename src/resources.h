#ifndef resources_h
#define resources_h

#include "battery.h"
#include "configuration.h"
#include "cutter.h"
#include "gnss.h"
#include "io_accelerometer/io_accelerometer.h"
#include "log_store.h"
#include "mowing_schedule.h"
#include "sonar.h"
#include "wheel_controller.h"

/**
 * Container class for holding references to instances that should be shared
 * between many classes. This is so that we can reduce the number of parameters
 * each method must take, but also making it easier to add additional references
 * in the future.
 */
class Resources
{
public:
    Resources(WheelController& wheelController,
              Cutter& cutter,
              Battery& battery,
              GNSS& gnss,
              Sonar& sonar,
              IO_Accelerometer& accelerometer,
              LogStore& logStore,
              MowingSchedule& mowingSchedule)
        : wheelController(wheelController)
        , cutter(cutter)
        , battery(battery)
        , gnss(gnss)
        , sonar(sonar)
        , accelerometer(accelerometer)
        , logStore(logStore)
        , mowingSchedule(mowingSchedule)
    {
    }

    WheelController& wheelController;
    Cutter& cutter;
    Battery& battery;
    GNSS& gnss;
    Sonar& sonar;
    IO_Accelerometer& accelerometer;
    LogStore& logStore;
    MowingSchedule& mowingSchedule;
};

#endif
