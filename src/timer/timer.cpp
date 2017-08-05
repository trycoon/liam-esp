#include <Arduino.h>
#include "timer.h"

Timer::Timer() {
  previousMillis = 0;
}

unsigned long Timer::startTimer() {
  previousMillis = millis();
  return previousMillis;
}

unsigned long Timer::millisSinceLastCheck() {
  // Get snapshot of time
  unsigned long currentMillis = millis();
  // How much time has passed, accounting for rollover with subtraction!
  unsigned long diffMillis = (unsigned long)(currentMillis - previousMillis);
  previousMillis = currentMillis;

  return diffMillis;
}

bool Timer::hasAmountTimePassed(unsigned long time) {
  // Get snapshot of time
  unsigned long currentMillis = millis();
  // How much time has passed, accounting for rollover with subtraction!
  if ((unsigned long)(millis() - previousMillis) >= time) {
    previousMillis = currentMillis; // reset timer so that we can call upon this method once more without running startTimer() first.
    return true;
  } else {
    return false;
  }
}
