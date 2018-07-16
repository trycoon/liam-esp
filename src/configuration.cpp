#include <Preferences.h>
#include <unordered_map>
#include "configuration.h"

namespace Configuration {

  Preferences preferences;
  // local caches to prevent us from reading from Flashmemory all the time.
  std::unordered_map<const char*, String> strConfig;
  std::unordered_map<const char*, int32_t> intConfig;

  void wipe() {
    preferences.begin("liam-esp", false);
    preferences.clear();
    strConfig.clear();
    intConfig.clear();
    preferences.end();
  }

  void clear(const char* key) {
    preferences.begin("liam-esp", false);
    preferences.remove(key);
    strConfig.erase(key);
    intConfig.erase(key);
    preferences.end();
  }

  void set(const char* key, String value) {
    preferences.begin("liam-esp", false);
    preferences.putString(key, value);
    strConfig[key] = value;
    preferences.end();
  }

  String getString(const char* key, const String defaultValue) {
    auto search = strConfig.find(key);

    if (search != strConfig.end()) {
      return search->second;  //second=value
    } else {
      preferences.begin("liam-esp", false);
      auto value = preferences.getString(key, defaultValue);
      strConfig[key] = value;
      preferences.end();

      return value;
    }
  }

  void set(const char* key, int32_t value) {
    preferences.begin("liam-esp", false);
    preferences.putInt(key, value);
    intConfig[key] = value;
    preferences.end();
  }

  int32_t getInt(const char* key, const int32_t defaultValue) {
    auto search = intConfig.find(key);

    if (search != intConfig.end()) {
      return search->second;  //second=value
    } else {
      preferences.begin("liam-esp", false);
      auto value = preferences.getInt(key, defaultValue);
      intConfig[key] = value;
      preferences.end();

      return value;
    }
  }
}
