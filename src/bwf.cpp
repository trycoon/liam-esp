#include "bwf.h"

BWF::BWF() {
  // http://grauonline.de/wordpress/?page_id=364
  // https://www.youtube.com/watch?v=NIer_kITelc
  // https://en.wikipedia.org/wiki/Barker_code
  // http://www.grauonline.de/alexwww/ardumower/filter/filter.html
}

bool BWF::isLeftInsideFence() {
  return true;
}

bool BWF::isRightInsideFence() {
  return true;
}
