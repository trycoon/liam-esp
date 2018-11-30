#ifndef _utils_h
#define _utils_h

#include <string>
#include <Arduino.h>

/**
* Assorted utility functions.
*/
namespace Utils {

  extern String generateKey(uint8_t length);
  extern String uint64String(uint64_t value, uint8_t base = 10);
}

#endif