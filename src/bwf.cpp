#include "bwf.h"

BWF::BWF() {
  // http://grauonline.de/wordpress/?page_id=364
  // https://www.youtube.com/watch?v=NIer_kITelc
  // https://en.wikipedia.org/wiki/Barker_code
  // http://www.grauonline.de/alexwww/ardumower/filter/filter.html
}

// USE Remote Controll: https://esp-idf.readthedocs.io/en/v3.0/api-reference/peripherals/rmt.html?highlight=remote%20controll
// https://github.com/G6EJD/ESP32-8-Octave-Audio-Spectrum-Display
bool BWF::isLeftInsideFence() {
  return true;
}

bool BWF::isRightInsideFence() {
  return true;
}
