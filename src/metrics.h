
#ifndef _metrics_h
#define _metrics_h

#include <Arduino.h>
#include <list>
#include "processable.h"
#include "scheduler/scheduler.h"
#include "cutter.h"
#include "battery.h"
#include "gps.h"

struct metricSample {
  uint32_t time;
  float batteryVoltage;
  uint8_t cutterLoad;
  double lat;
  double lng;
  uint32_t memory;
};

struct batterySample {
  uint32_t time;
  float batteryVoltage;
};

struct cutterLoadSample {
  uint32_t time;
  uint8_t cutterLoad;
};

struct gpsPosition {
  uint32_t time;
  double lat;
  double lng;
};

struct heapMemory {
  uint32_t time;
  uint32_t memory;
};

/**
* Class that keep track of historical metrics, and constantly samples sensors and bookeep readings.
*/
class Metrics : Processable {
  public:
    Metrics(Battery& battery, Cutter& cutter, GPS& gps);
    std::list<batterySample> getBatteryHistory();
    std::list<cutterLoadSample> getCutterLoadHistory();
    std::list<gpsPosition> getGpsPositionHistory();
    std::list<heapMemory> getMemoryHistory();

    void process();

  private:
    const uint16_t MAX_SAMPLES = 100;   // How much history reading are we going to keep? set too high will consume excessive memory and we may get out-of-memory errors.
    Battery& battery;
    Cutter& cutter;
    GPS& gps;
    Scheduler pollTimer;
    std::list<metricSample> metricSamples;
    void getSample();
};

#endif
