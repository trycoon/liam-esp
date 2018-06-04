#ifndef _configuration_h
#define _configuration_h

/**
* Class handling user configurable parameters of the application. It also takes care of storing and retereiving these from flash memory.
*/
namespace Configuration {
  extern void clear(const char* key);
  extern void set(const char* key, String value);
  extern void set(const char* key, int32_t value);
  extern String getString(const char* key, const String defaultValue = "");
  extern int32_t getInt(const char* key, const int32_t defaultValue = 0);
}

#endif
