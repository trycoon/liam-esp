#ifndef _sonar_h
#define _sonar_h

#include <Arduino.h>
#include "processable.h"

struct SonarDevice {
  uint8_t ping_pin;
  uint8_t sense_pin;
  uint8_t sampleIndex = 0;
  uint16_t sampleDistances[5] = { 0, 0, 0, 0, 0 }; // Numer of samples before calculating distance, higher value means we get less affected of false readings (like reflecting grass) but consuming more memory and less responsive to sudden obstacles.
  uint16_t distance = 0;
};

struct SonarDistances {
  uint16_t frontDistance = 0;
};

class Sonar : public Processable {
  public:

    Sonar();
    SonarDistances getObstacleDistances();
    /* Internal use only! */
    void process();

  private:
    volatile boolean pingInProgress = false;
    volatile unsigned long startTime = 0;
    SonarDevice sonarFront;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    void ping(SonarDevice device);
    void IRAM_ATTR onPing();
};

#endif
