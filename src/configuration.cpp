#include <ArduinoLog.h>
#include "configuration.h"
#include "log_store.h"
#include "utils.h"

namespace Configuration {

  static const char* MQTT_TOPIC = "home/liam-esp/subscribe";
  static const char* MQTT_TOPIC_COMMAND = "home/liam-esp/command";
  static const char* DEFAULT_NTP = "pool.ntp.org";
  Preferences preferences;
  configObject config;

  void load() {
    // this one is never saved/loaded from preferences, we just cache it here.
    // it's an unique id for every ESP32, also used as MAC-address for network.
    config.mowerId = Utils::uint64String(ESP.getEfuseMac());

    preferences.begin("liam-esp", true);
    auto jsonString = preferences.getString("config", "{}");
    preferences.end();
    DynamicJsonBuffer jsonBuffer(300);
    JsonObject& json = jsonBuffer.parseObject(jsonString);

    if (json.success()) {
      config.username = "admin";
      if (json.containsKey("username")) {
        config.username = json["username"].as<String>();
      }

      config.password = "liam";
      if (json.containsKey("password")) {
        config.password = json["password"].as<String>();
      }

      config.logLevel = LOG_LEVEL_NOTICE;
      if (json.containsKey("logLevel")) {
        config.logLevel = json["logLevel"];
      }

      config.startChargeTime = json["startChargeTime"];
      config.lastFullyChargeTime = json["lastFullyChargeTime"];
      config.lastChargeDuration = json["lastChargeDuration"];
      
      if (json.containsKey("lastState")) {
        config.lastState = json["lastState"].as<String>();
      }

      if (json.containsKey("mqttServer")) {
        config.mqttServer = json["mqttServer"].as<String>();
      }

      config.mqttPort = "1883";
      if (json.containsKey("mqttPort")) {
        config.mqttPort = json["mqttPort"].as<String>();
      }

      config.mqttTopic = MQTT_TOPIC;
      if (json.containsKey("mqttTopic")) {
        config.mqttTopic = json["mqttTopic"].as<String>();
      }

      config.mqttTopicCommand = MQTT_TOPIC_COMMAND;
      if (json.containsKey("mqttTopicCommand")) {
        config.mqttTopicCommand = json["mqttTopicCommand"].as<String>();
      }

      config.ntpServer = DEFAULT_NTP;
      if (json.containsKey("ntpServer")) {
        config.ntpServer = json["ntpServer"].as<String>();
      }

      config.gmt = "0";
      if (json.containsKey("gmt")) {
        config.gmt = json["gmt"].as<String>();
      }

      if (json.containsKey("wifiPassword")) {
        config.wifiPassword = json["wifiPassword"].as<String>();
      }

      if (json.containsKey("ssid")) {
        config.ssid = json["ssid"].as<String>();
      }

      if (json.containsKey("apiKey")) {
        config.apiKey = json["apiKey"].as<String>();
      }

      config.setupDone = false;
      if (json.containsKey("setupDone")) {
        config.setupDone = json["setupDone"];
      }
    }

    Log.trace("Loaded settings from Flash: %s" CR, jsonString.c_str());
  }
  
  void save() {    
    DynamicJsonBuffer jsonBuffer(300);
    JsonObject& json = jsonBuffer.createObject();

    json["username"] = config.username;
    json["password"] = config.password;
    json["logLevel"] = config.logLevel;
    json["startChargeTime"] = config.startChargeTime;
    json["lastFullyChargeTime"] = config.lastFullyChargeTime;
    json["lastChargeDuration"] = config.lastChargeDuration;
    json["lastState"] = config.lastState;
    json["mqttServer"] = config.mqttServer;
    json["mqttPort"] = config.mqttPort;
    json["mqttTopic"] = config.mqttTopic;
    json["mqttTopicCommand"] = config.mqttTopicCommand;    
    json["ntpServer"] = config.ntpServer;
    json["gmt"] = config.gmt;
    json["wifiPassword"] = config.wifiPassword;
    json["ssid"] = config.ssid;
    json["apiKey"] = config.apiKey;
    json["setupDone"] = config.setupDone;

    String jsonString;
    json.printTo(jsonString);

    preferences.begin("liam-esp", false);
    preferences.putString("config", jsonString);
    preferences.end();

    Log.trace("Saved settings to Flash: %s" CR, jsonString.c_str());
  }

  void wipe() {
    preferences.begin("liam-esp", false);
    preferences.clear();
    preferences.end();    
  }
}
