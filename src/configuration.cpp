#include "configuration.h"

Configuration::Configuration() {
  preferences.begin("liam-esp", false);
}

Configuration::~Configuration() {
  preferences.end();
}

bool Configuration::getBool(const char *key, bool defaultValue) {
  return preferences.getBool(key, defaultValue);
}

int32_t Configuration::getInt(const char *key, int32_t defaultValue) {
  return preferences.getInt(key, defaultValue);
}

String Configuration::getString(const char *key, String defaultValue) {
  return preferences.getString(key, defaultValue);
}

void Configuration::putBool(const char *key, bool value){
  preferences.putBool(key, value);
}

void Configuration::putInt(const char *key, int32_t value) {
  preferences.putInt(key, value);
}

void Configuration::putString(const char *key, String value) {
  preferences.putString(key, value);
}
