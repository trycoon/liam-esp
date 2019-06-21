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
  extern int64_t getEpocTime();
  
  template <typename Cont, typename Pred>
  extern Cont filter(const Cont &container, Pred predicate);

  /**
   * Filter data structure using predicate
   * e.g. std::vector<int> myVec = {1,4,7,8,9,0}; auto filteredVec = filter(myVec, [](int a) { return a > 5; });
   */
  template <typename Cont, typename Pred>
  Cont filter(const Cont &container, Pred predicate) {
      Cont result;
      std::copy_if(container.begin(), container.end(), std::back_inserter(result), predicate);
      return result;
  }
  
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