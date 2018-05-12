
#ifndef _metrics_h
#define _metrics_h

#include <Arduino.h>
#include <list>
#include "processable.h"
#include "scheduler/scheduler.h"
#include "battery.h"
#include "gps.h"

struct metricSample {
  uint32_t time;
  float batteryVoltage;
  double lat;
  double lng;
};

struct batterySample {
  uint32_t time;
  float batteryVoltage;
};

struct gpsPosition {
  uint32_t time;
  double lat;
  double lng;
  uint16_t dir;
};

/**
* Class that keep track of historical metrics, and constantly samples sensors and bookeep readings.
*/
class Metrics : Processable {
  public:
    Metrics(Battery& battery, GPS& gps);
    std::list<batterySample> getBatteryHistory();
    std::list<gpsPosition> getGpsPositionHistory();

    void process();

  private:
    const uint16_t MAX_SAMPLES = 100;   // How much history reading are we going to keep? set too high will consume excessive memory and we may get out-of-memory related errors.
    Battery& battery;
    GPS& gps;
    Scheduler pollTimer;
    std::list<metricSample> metricSamples;
    void getSample();
};

#endif
