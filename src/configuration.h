#ifndef _configuration_h
#define _configuration_h

#include <Preferences.h>

/**
* Class handling user configurabable parameters of the application. It also takes care of storing and retereiving these from flash memory.
*/
class Configuration {
  public:
    Configuration();
    ~Configuration();
    bool getBool(const char *key, bool defaultValue);
    int32_t getInt(const char *key, int32_t defaultValue);
    String getString(const char *key, String defaultValue);
    void putBool(const char *key, bool value);
    void putInt(const char *key, int32_t value);
    void putString(const char *key, String value);

  private:
    Preferences preferences;
};

#endif
