#ifndef _configuration_h
#define _configuration_h

#include <Arduino.h>
#include <Preferences.h>

/**
* Class handling user configurable parameters of the application. It also takes care of storing and retereiving these from flash memory.
*/
namespace Configuration {

  struct configObject {
    String mowerId;
    String username;
    String password;
    int8_t logLevel;
    uint32_t startChargeTime = 0;
    uint32_t lastFullyChargeTime = 0;
    uint32_t lastChargeDuration = 0;
    String lastState;
    String mqttServer;
    String mqttPort;
    String mqttTopic;
    String mqttTopicCommand;
    String ntpServer;
    String gmt;
    String wifiPassword;
    String ssid;
    String apiKey;
    bool setupDone = false;
  };

  extern Preferences preferences;
  extern configObject config;
  extern void load();
  extern void save();
  extern void wipe();
}

#endif
