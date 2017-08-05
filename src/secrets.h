#ifndef _secrets_h
#define _secrets_h

#include <string>
#include <Arduino.h>

namespace Settings {
  extern const char* const SSID;
  extern const char* const WIFI_PASSWORD;
  extern const char* const OTA_PASSWORD;
  extern const char* const MQTT_SERVER;
  extern const uint16_t MQTT_PORT;
  extern const std::string MQTT_TOPIC;
};

#endif
