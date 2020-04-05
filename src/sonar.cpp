#include <FunctionalInterrupt.h>
#include <ArduinoLog.h>
#include "sonar.h"
#include "utils.h"
#include "definitions.h"

// Idea taken from https://www.instructables.com/id/Non-blocking-Ultrasonic-Sensor-for-Arduino/

/**
 * Class for interfacing with a ultrasound sonar sensor, used for detecting obstacles around the mower before we bump into them.
 * Compatible sensors are HC-SR04. SRF06 is NOT supported since it's too slow.
 */
Sonar::Sonar() {
  // define available sensors...
  SonarDevice sonarFront;
  sonarFront.ping_pin = Definitions::SONAR_FRONT_PING_PIN;
  sonarFront.sense_pin = Definitions::SONAR_FRONT_SENSE_PIN;
  pinMode(sonarFront.ping_pin, OUTPUT);
  pinMode(sonarFront.sense_pin, INPUT);
  attachInterrupt(sonarFront.sense_pin, std::bind(&Sonar::onPing, this), CHANGE);
}

void Sonar::ping(SonarDevice device) {

  if (!pingInProgress) {

    pingInProgress = true;
    startTime = micros(); // just set it, we will update it later in ISR to be more current. This is needed to check for timeouts in process()

    digitalWrite(device.ping_pin, LOW);   // Set the trigger pin low, should already be low, but this will make sure it is.
    delayMicroseconds(4);                 // Wait for pin to go low.
    digitalWrite(device.ping_pin, HIGH);  // Set trigger pin high, this tells the sensor to send out a ping.
    delayMicroseconds(10);                // Wait long enough for the sensor to realize the trigger pin is high. Sensor specs say to wait 10uS.
    digitalWrite(device.ping_pin, LOW);   // Set trigger pin back to low.

  }
}

void IRAM_ATTR Sonar::onPing() {
  auto time = micros();

  portENTER_CRITICAL_ISR(&mux);
 
   // check if we went from HIGH to LOW, or from LOW to HIGH
  switch(digitalRead(sonarFront.sense_pin)) {
    case HIGH:
      // store starttime when we just received ping response
      //    _______________.... (duration, longer pule = obstacle far away)
      //    |
      //____|
      startTime = time;
      break;
    case LOW:
      // store ping duration in samplelist for current sonar
      // ____
      //    |
      //    |______________.... (when signal goes LOW, calculate duration of pulse and store as one sample)
      auto distance = (time - startTime) / 57; // divide with 57 to get distance in centimeters from microseconds.

      if (distance <= Definitions::SONAR_MAXDISTANCE) {
        sonarFront.sampleDistances[sonarFront.sampleIndex] = distance;
        sonarFront.sampleIndex = (sonarFront.sampleIndex + 1) % (sizeof(sonarFront.sampleDistances) / sizeof(uint16_t));
      }

      pingInProgress = false;
      break;
  }
    
  portEXIT_CRITICAL_ISR(&mux);
}

void Sonar::process() {

  portENTER_CRITICAL(&mux);
  
  if (pingInProgress && (micros() - startTime) > 30000L) {
    // if we have not received an response within 30ms since ping started then either our startsignal was not detected by the sonar-sensor or no obstacle was detected within 4,5 meters range.
    pingInProgress = false;
  }

  if (!pingInProgress) {
    sonarFront.distance = Utils::calculateMedian(sonarFront.sampleDistances);
  //  currentSonar = (currentSonar + 1) % sonars.size();
  //  Log.notice("t1: %d, t2: %d, d: %d %d %d %d %d" CR, test1, test2, sonar.sampleDistances[0], sonar.sampleDistances[1], sonar.sampleDistances[2], sonar.sampleDistances[3], sonar.sampleDistances[4]);

    ping(sonarFront);
  }
  portEXIT_CRITICAL(&mux);
}

/**
 * Get distances from mower to closest detected obstacle (in centimeters)
 */
SonarDistances Sonar::getObstacleDistances() {

  portENTER_CRITICAL(&mux);
  
  SonarDistances distances;
  distances.frontDistance = sonarFront.distance;
  
  portEXIT_CRITICAL(&mux);

  return distances;
}
