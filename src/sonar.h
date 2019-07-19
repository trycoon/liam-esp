#ifndef _sonar_h
#define _sonar_h

#include <vector>
#include <Arduino.h>
#include "processable.h"

struct SonarDevice {
  uint8_t pin;
  uint8_t sampleIndex = 0;
  uint16_t sampleDistances[5] = { 0, 0, 0, 0, 0 }; // Numer of samples before calculating distance, higher value means we get less affected of false readings (like reflecting grass) but consuming more memory and less responsive to sudden obstacles.
  uint16_t distance = 0;
};

class Sonar : public Processable {
  public:

    Sonar();
    uint16_t getObstacleDistance(uint8_t sonar_nr);
    /* Internal use only! */
    void process();

  private:
    volatile boolean pingInProgress = false;
    volatile uint8_t currentSonar = 0;
    volatile unsigned long startTime = 0;
    volatile uint32_t test1=0;
    volatile uint32_t test2=0;
    std::vector<SonarDevice> sonars;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    void ping();
    void IRAM_ATTR onPing();
};

#endif
