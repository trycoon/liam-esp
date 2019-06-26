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
  SonarDevice sonar1;
  sonar1.pin = Definitions::SONAR1_PIN;
  //sonars.push_back(sonar1);
  /*SonarDevice sonar2;
  sonar2.pin = Definitions::SONAR2_PIN;
  sonars.push_back(sonar2);*/
  /*SonarDevice sonar3;
  sonar3.pin = Definitions::SONAR3_PIN;
  sonars.push_back(sonar3);*/
  //pinMode(digitalPinToInterrupt(23), INPUT);
  //attachInterrupt(digitalPinToInterrupt(23), std::bind(&Sonar::onPing, this), CHANGE);
}

void Sonar::ping() {

  if (!pingInProgress) {

    pingInProgress = true;
    startTime = micros(); // just set it, we will update it later in ISR to be more current. This is needed to check for timeouts in process()

    auto sonar_pin = sonars[currentSonar].pin;

    //detachInterrupt(digitalPinToInterrupt(sonar_pin));  // Don't listen for changes when using pin to send ping.
    pinMode(sonar_pin, OUTPUT);
    digitalWrite(sonar_pin, LOW);   // Set the trigger pin low, should already be low, but this will make sure it is.
    delayMicroseconds(4);           // Wait for pin to go low.
    digitalWrite(sonar_pin, HIGH);  // Set trigger pin high, this tells the sensor to send out a ping.
    delayMicroseconds(10);          // Wait long enough for the sensor to realize the trigger pin is high. Sensor specs say to wait 10uS.
    digitalWrite(sonar_pin, LOW);   // Set trigger pin back to low.
   // pinMode(sonar_pin, INPUT);      // Set trigger pin to input (when using one Arduino pin, this is technically setting the echo pin to input as both are tied to the same Arduino pin).
    //attachInterrupt(digitalPinToInterrupt(sonar_pin), std::bind(&Sonar::onPing, this), CHANGE); // Listen for changes.
    test1++;
  }
}

void IRAM_ATTR Sonar::onPing() {
  auto time = micros();

  portENTER_CRITICAL_ISR(&mux);
test2++;

 /*  auto sonar = sonars[currentSonar];
  // check if we went from HIGH to LOW, or from LOW to HIGH
  switch(digitalRead(sonar.pin)) {
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
        sonar.sampleDistances[sonar.sampleIndex] = distance;
        sonar.sampleIndex = (sonar.sampleIndex + 1) % (sizeof(sonar.sampleDistances) / sizeof(uint16_t));
      }

      pingInProgress = false;
      break;
  } */
    
  portEXIT_CRITICAL_ISR(&mux);
}

void Sonar::process() {

  if (sonars.size() > 0) {

    portENTER_CRITICAL(&mux);
    
    auto sonar = sonars[currentSonar];

    if (pingInProgress && (micros() - startTime) > 30000L) {
      // if we have not received an response within 30ms since ping started then either our startsignal was not detected by the sonar-sensor or no obstacle was detected within 4,5 meters range.
      pingInProgress = false;
    }

    if (!pingInProgress) {
      sonar.distance = Utils::calculateMedian(sonar.sampleDistances);
      currentSonar = (currentSonar + 1) % sonars.size();
      Log.notice("t1: %d, t2: %d, d: %d %d %d %d %d" CR, test1, test2, sonar.sampleDistances[0], sonar.sampleDistances[1], sonar.sampleDistances[2], sonar.sampleDistances[3], sonar.sampleDistances[4]);

      ping();
    }
    portEXIT_CRITICAL(&mux);
  }
}

/**
 * Get distance from mower to closest detected obstacle (in centimeters)
 */
uint16_t Sonar::getObstacleDistance(uint8_t sonar_nr) {
  if (sonar_nr > sonars.size() - 1) {
    return 0;
  }

  portENTER_CRITICAL(&mux);
  
  auto distance = sonars[sonar_nr].distance;
  
  portEXIT_CRITICAL(&mux);

  return 0;
}
