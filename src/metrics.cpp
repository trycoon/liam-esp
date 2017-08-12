#include "metrics.h"

Metrics::Metrics(Battery& battery, Cutter& cutter, GPS& gps) : metricSamples(MAX_SAMPLES), battery(battery), cutter(cutter), gps(gps) {
  pollTimer.schedule([this]() {
    getSample();
  }, 5000, true);

  // ESP.getCycleCount() // "returns the cpu instruction cycle count since start as an unsigned 32-bit."
  /*
    ESP.getCycleCount() counts the instruction cycles since start, in an internal unsigned 32-bit variable, liable to overflow every 28 seconds or so. A one microsecond period will give 160 instruction cycles. Now to measure the software overhead required by your software, you need to only count the instruction cycles your routine counts for a 1 microsecond delay and subtract 160 from that count, to give you the software overhead in acquiring the count. For 1 microsecond delay, I got a count of 213. Which worked out to 213-160 = 53 counts (53 x 6.25 = 331.25 nanoseconds) software overhead to acquire the count. Subtracting 53 from every count gives me a count accurate to within a few tens of picoseconds, for periods from 30 microseconds to about 500 microseconds.
  */
}

void Metrics::getSample() {
  metricSample sample;

  // make sure sample list don't grow larger than MAX_SAMPLES. Older samples are removed.
  if (metricSamples.size() >= MAX_SAMPLES) {
    metricSamples.pop_front();
  }

  sample.time = millis();
  sample.batteryVoltage = battery.getBatteryVoltage();
  sample.cutterLoad = cutter.getLoad();
//  sample.lat = gps.getPosition();
//  sample.lng;
  sample.memory = ESP.getFreeHeap();

  metricSamples.push_back(sample);
}

void Metrics::process() {
  pollTimer.process();
}

std::list<batterySample> Metrics::getBatteryHistory() {
  std::list<batterySample> samples;

  for (auto &s: metricSamples){
    batterySample sample;
    sample.time = s.time;
    sample.batteryVoltage = s.batteryVoltage;

    samples.push_back(sample);
  }

  return samples;
}

std::list<cutterLoadSample> Metrics::getCutterLoadHistory() {
  std::list<cutterLoadSample> samples;

  for (auto &s: metricSamples){
    cutterLoadSample sample;
    sample.time = s.time;
    sample.cutterLoad = s.cutterLoad;

    samples.push_back(sample);
  }

  return samples;
}

std::list<gpsPosition> Metrics::getGpsPositionHistory() {
  std::list<gpsPosition> samples;

  for (auto &s: metricSamples){
    gpsPosition sample;
    sample.time = s.time;
    sample.lat = s.lat;
    sample.lng = s.lng;

    samples.push_back(sample);
  }

  return samples;
}

std::list<heapMemory> Metrics::getMemoryHistory() {
  std::list<heapMemory> samples;

  for (auto &s: metricSamples){
    heapMemory sample;
    sample.time = s.time;
    sample.memory = s.memory;

    samples.push_back(sample);
  }

  return samples;
}
