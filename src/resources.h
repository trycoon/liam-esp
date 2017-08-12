#ifndef resources_h
#define resources_h

#include "wifi.h"
#include "wheel_controller.h"
#include "cutter.h"
#include "bwf.h"
#include "battery.h"
#include "gps.h"

/**
* Container class for holding references to instances that should be shared between many classes.
* This is so that we can reduce the number of parameters each method must take, but also making it easier to add additional references in the future.
*/
class Resources {
  public:
    Resources(WiFi_Client& mqtt,
                           WheelController& wheelController,
                           Cutter& cutter,
                           BWF& bwf,
                           Battery& battery,
                           GPS& gps)
                           : mqtt(mqtt),
                             wheelController(wheelController),
                             cutter(cutter),
                             bwf(bwf),
                             battery(battery),
                             gps(gps) { }

    WiFi_Client& mqtt;
    WheelController& wheelController;
    Cutter& cutter;
    BWF& bwf;
    Battery& battery;
    GPS& gps;
};

#endif
