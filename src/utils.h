#ifndef _utils_h
#define _utils_h

#include <string>
#include <Arduino.h>


/**
* Assorted utility functions.
*/
namespace Utils {

  extern bool isTimeAvailable;

  extern String generateKey(uint8_t length);
  extern String uint64String(uint64_t value, uint8_t base = 10);
  extern int64_t getEpocTime();
  extern String getTime(String format = "%d %b %Y, %H:%M:%S%z", uint32_t timeout = 5000);

  template<typename T>
  extern T calculateMedian(std::vector<T> entries);
  template<typename T, size_t size>
  extern T calculateMedian(T(&entries)[size]);
  
  /**
   * Get median value from an list of values
   */
  template<typename T>
  T calculateMedian(std::vector<T> entries) {
      auto size = entries.size();

      if (size == 0) {
          return 0;  // Undefined, really.
      } else {
          sort(entries.begin(), entries.end());

          if (size % 2 == 0) {
              return (entries[size / 2 - 1] + entries[size / 2]) / 2;
          } else {
              return entries[size / 2];
          }
      }
  }

  /**
   * Get median value from an array of values
   */
  template<typename T, size_t size>
  T calculateMedian(T(&entries)[size]) {

      if (size == 0) {
          return 0;  // Undefined, really.
      } else {
          std::sort(&entries[0], entries + size);

          if (size % 2 == 0) {
              return (entries[size / 2 - 1] + entries[size / 2]) / 2;
          } else {
              return entries[size / 2];
          }
      }
  }
}

#endif