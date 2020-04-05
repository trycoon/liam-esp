#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include "configuration.h"
#include "log_store.h"
#include "utils.h"

namespace Configuration {

  Preferences preferences;
  configObject config;

  void load() {
    // this one is never saved/loaded from preferences, we just cache it here.
    // it's an unique id for every ESP32, also used as MAC-address for network.
    config.mowerId = Utils::uint64String(ESP.getEfuseMac());

    preferences.begin("liam-esp", false);
    auto jsonString = preferences.getString("config", "{}");
    DynamicJsonBuffer jsonBuffer(200);
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
    DynamicJsonBuffer jsonBuffer(200);
    JsonObject& json = jsonBuffer.createObject();

    json["username"] = config.username;
    json["password"] = config.password;
    json["logLevel"] = config.logLevel;
    json["startChargeTime"] = config.startChargeTime;
    json["lastFullyChargeTime"] = config.lastFullyChargeTime;
    json["lastChargeDuration"] = config.lastChargeDuration;
    json["lastState"] = config.lastState;
    json["gmt"] = config.gmt;
    json["wifiPassword"] = config.wifiPassword;
    json["ssid"] = config.ssid;
    json["apiKey"] = config.apiKey;
    json["setupDone"] = config.setupDone;

    String jsonString;
    json.printTo(jsonString);

    preferences.begin("liam-esp", false);
    preferences.putString("config", jsonString);

    Log.trace("Saved settings to Flash: %s" CR, jsonString.c_str());
  }

  void wipe() {
    preferences.begin("liam-esp", false);
    preferences.clear();
  }
}
