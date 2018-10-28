#include <ArduinoLog.h>
#include "api.h"
#include "esp_log.h"
#include "definitions.h"
#include "settings.h"
#include "io_accelerometer/io_accelerometer.h"

/**
* Class defining all the REST endpoints and other public available APIs.
* We are aiming for a self explaining REST-API that resembles the HATEOAS specification.
* 
* Usually communication are driven by clients making HTTP-requests to this server,
* one exception to this is the /status endpoint that also is available for subscription using WebSockets.
* Since the /status endpoint is so commonly used and requested by clients we actively push status updates to the connected clients,
* making the clients more responsive and it also taxes the server less.
*/
Api::Api(StateController& stateController, Resources& resources) :
  stateController(stateController),
  resources(resources) {}

void Api::statusToJson(statusResponse obj, JsonObject& json) {
    json["state"] = obj.state;
    json["batteryVoltage"] = obj.batteryVoltage;
    json["batteryLevel"] = obj.batteryLevel;
    json["isCharging"] = obj.isCharging;
    json["lastFullyChargeTime"] = obj.lastFullyChargeTime;
    json["lastChargeDuration"] = obj.lastChargeDuration;
    json["cutterLoad"] = obj.cutterLoad;
    json["cutterRotating"] = obj.cutterRotating;
    json["uptime"] = obj.uptime;
    json["wifiSignal"] = obj.wifiSignal;
    json["leftWheelSpd"] = obj.leftWheelSpd;
    json["rightWheelSpd"] = obj.rightWheelSpd;
    json["targetHeading"] = obj.targetHeading;
    json["pitch"] = obj.pitch;
    json["roll"] = obj.roll;
    json["heading"] = obj.heading;
}

/**
 * Collect status information from subsystems and push it to clients, if information has changed.
 */
void Api::collectAndPushNewStatus() {
  bool statusChanged = false;

  if (currentStatus.state != stateController.getStateInstance()->getStateName()) {
    currentStatus.state = stateController.getStateInstance()->getStateName();
    statusChanged = true;
  }
  if (currentStatus.batteryVoltage != resources.battery.getBatteryVoltage()) {
    currentStatus.batteryVoltage = resources.battery.getBatteryVoltage();
    statusChanged = true;
  }
  if (currentStatus.batteryLevel != resources.battery.getBatteryStatus()) {
    currentStatus.batteryLevel = resources.battery.getBatteryStatus();
    statusChanged = true;
  }
  if (currentStatus.isCharging != resources.battery.isCharging()) {
    currentStatus.isCharging = resources.battery.isCharging();
    statusChanged = true;
  }
  if (currentStatus.lastFullyChargeTime != resources.battery.getLastFullyChargeTime()) {
    currentStatus.lastFullyChargeTime = resources.battery.getLastFullyChargeTime();
    statusChanged = true;
  }
  if (currentStatus.lastChargeDuration != resources.battery.getLastChargeDuration()) {
    currentStatus.lastChargeDuration = resources.battery.getLastChargeDuration();
    statusChanged = true;
  }
  if (currentStatus.cutterLoad != resources.cutter.getLoad()) {
    currentStatus.cutterLoad = resources.cutter.getLoad();
    statusChanged = true;
  }
  if (currentStatus.cutterRotating != resources.cutter.isCutting()) {
    currentStatus.cutterRotating = resources.cutter.isCutting();
    statusChanged = true;
  }

  auto stat = resources.wheelController.getStatus();
  if (currentStatus.leftWheelSpd != stat.leftWheelSpeed) {
    currentStatus.leftWheelSpd = stat.leftWheelSpeed;
    statusChanged = true;
  }
  if (currentStatus.rightWheelSpd != stat.rightWheelSpeed) {
    currentStatus.rightWheelSpd = stat.rightWheelSpeed;
    statusChanged = true;
  }
  if (currentStatus.targetHeading != stat.targetHeading) {
    currentStatus.targetHeading = stat.targetHeading;
    statusChanged = true;
  }
  auto wifiSignal = WiFi.RSSI();
  if (currentStatus.wifiSignal != wifiSignal) {
    currentStatus.wifiSignal = wifiSignal;
    statusChanged = true;
  }

  auto orient = resources.accelerometer.getOrientation();
  if (currentStatus.pitch != orient.pitch) {
    currentStatus.pitch = orient.pitch;
    statusChanged = true;
  }
  if (currentStatus.roll != orient.roll) {
    currentStatus.roll = orient.roll;
    statusChanged = true;
  }
  if (currentStatus.heading != orient.heading) {
    currentStatus.heading = orient.heading;
    statusChanged = true;
  }

  // These change so often that we don't set statusChanged for these, otherwise we would push everytime.
  currentStatus.uptime = (uint32_t)(esp_timer_get_time() / 1000000); // uptime in microseconds so we divide to seconds.

  if (statusChanged) {
    DynamicJsonBuffer jsonBuffer(380);
    JsonObject& root = jsonBuffer.createObject();
    statusToJson(currentStatus, root);

    resources.wifi.sendDataWebSocket("status", root);

    // MQTT updates don't have to be "realtime", we can settle with an update every 10 sec to not spam server.
    if (lastMQTT_push < currentStatus.uptime - 10) {
      String jsonStr;
      root.printTo(jsonStr);
      resources.wifi.publish_mqtt(jsonStr.c_str(), "/status");
      lastMQTT_push = currentStatus.uptime;
    }
  }
}

/**
 * Generate a 128 bit long API key of ASCII characters
 */
String Api::generateApikey() {
  const String CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  String key = "";
  // get true random number from the hardware RNG. this must be called after WiFi has been enabled to be truly random.
  for (int i = 0; i < 16; i++) {
    key += CHARS[random(0, CHARS.length())];
  }
  
  return key;
}

/**
 * Receives commands from MQTT broker that we could act upon.
 */
void Api::onMqttMessage(char* topic, char* payload, size_t length) {
  DynamicJsonBuffer jsonBuffer(length);
  JsonObject& root = jsonBuffer.parseObject((const char*)payload);
  
  if (root.success()) {
    if (root.containsKey("state")) {
      auto state = root["state"].as<char*>();
      
      if (!stateController.setUserChangableState(state)) {
        Log.notice(F("Unknown state \"%s\" received on MQTT command topic." CR), state);
      }
    } 
  } else {
    Log.notice(F("Failed to parse MQTT command." CR));
  }
}

void Api::setupApi() {
  // alternative to Basic authentication, API key should be included in each API request.
  if (Configuration::config.apiKey.length() == 0) {
    Configuration::config.apiKey = generateApikey();
    Configuration::save();
  }

  AsyncWebServer& web_server = resources.wifi.getWebServer();
  resources.wifi.registerMqttMessageCallback(
    [this](char* topic, char* payload, size_t length) {
      onMqttMessage(topic, payload, length);
    }
  );

  // collect and check if new status should be pushed every XXX ms.
  pushNewInfoTicker.attach_ms<Api*>(400, [](Api* instance) {
    instance->collectAndPushNewStatus();     
  }, this);

  // HTTP basic authentication
  web_server.on("/api/v1/login", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str())) {
      return request->requestAuthentication();
    }

    auto *response = request->beginResponse(200, "text/plain", "Login Success!");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/battery
  web_server.on("/api/v1/history/battery", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    DynamicJsonBuffer jsonBuffer(20000);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "http://" + WiFi.localIP().toString() + "/api/v1/history/battery";
    self["method"] = "GET";

    JsonObject& samples = root.createNestedObject("samples");
    JsonArray& time = samples.createNestedArray("time");
    JsonArray& value = samples.createNestedArray("value");
    for (auto &s: resources.metrics.getBatteryHistory()) {
      time.add(s.time);
      value.add(s.batteryVoltage);
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/position
  web_server.on("/api/v1/history/position", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");    
    DynamicJsonBuffer jsonBuffer(60000);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "http://" + WiFi.localIP().toString() + "/api/v1/history/position";
    self["method"] = "GET";

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: resources.metrics.getGpsPositionHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["lt"] = s.lat;
        sample["lg"] = s.lng;
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history
  web_server.on("/api/v1/history", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }
    
    const String host = "http://" + WiFi.localIP().toString();
    auto *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer(350);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = host + "/api/v1/history";
    self["method"] = "GET";

    JsonObject& battery = links.createNestedObject("battery");
    battery["href"] = host + "/api/v1/history/battery";
    battery["method"] = "GET";

    JsonObject& position = links.createNestedObject("position");
    position["href"] = host + "/api/v1/history/position";
    position["method"] = "GET";

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/manual
  web_server.on("/api/v1/manual", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    const String host = "http://" + WiFi.localIP().toString();
    auto *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer(750);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = host + "/api/v1/manual";
    self["method"] = "GET";

    JsonObject& forward = links.createNestedObject("forward");
    forward["href"] = host + "/api/v1/manual/forward";
    forward["method"] = "PUT";

    JsonObject& backward = links.createNestedObject("backward");
    backward["href"] = host + "/api/v1/manual/backward";
    backward["method"] = "PUT";

    /*JsonObject& left = links.createNestedObject("turn");
    left["href"] = host + "/api/v1/manual/turn";
    left["method"] = "PUT";*/

    JsonObject& stop = links.createNestedObject("stop");
    stop["href"] = host + "/api/v1/manual/stop";
    stop["method"] = "PUT";

    JsonObject& cutter_on = links.createNestedObject("cutter_on");
    cutter_on["href"] = host + "/api/v1/manual/cutter_on";
    cutter_on["method"] = "PUT";

    JsonObject& cutter_off = links.createNestedObject("cutter_off");
    cutter_off["href"] = host + "/api/v1/manual/cutter_off";
    cutter_off["method"] = "PUT";

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/status
  web_server.on("/api/v1/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    
    DynamicJsonBuffer jsonBuffer(600);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "http://" + WiFi.localIP().toString() + "/api/v1/status";
    self["method"] = "GET";

    statusToJson(currentStatus, root);

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/system
  web_server.on("/api/v1/system", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");    
    DynamicJsonBuffer jsonBuffer(400);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "http://" + WiFi.localIP().toString() + "/api/v1/system";
    self["method"] = "GET";
    
    root["name"] = Definitions::APP_NAME;
    root["version"] = Definitions::APP_VERSION;
    root["cpuFreq"] = ESP.getCpuFreqMHz();
    root["flashChipSize"] = ESP.getFlashChipSize();
    root["freeHeap"] = ESP.getFreeHeap();
    root["apiKey"] = Configuration::config.apiKey.c_str();
    
    JsonObject& settings = root.createNestedObject("settings");
    settings["batteryFullVoltage"] = Settings::BATTERY_FULLY_CHARGED;
    settings["batteryEmptyVoltage"] = Settings::BATTERY_EMPTY;

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/loglevel
  web_server.on("/api/v1/loglevel", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    DynamicJsonBuffer jsonBuffer(50);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "http://" + WiFi.localIP().toString() + "/api/v1/loglevel";
    self["method"] = "GET";

    root["level"] = Configuration::config.logLevel;

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/logmessages
  web_server.on("/api/v1/logmessages", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    DynamicJsonBuffer jsonBuffer(Settings::LOGMESSAGES_LENGTH + 30);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "http://" + WiFi.localIP().toString() + "/api/v1/logmessages";
    self["method"] = "GET";

    //root["messages"] = String(log_messages.size());
    root["messages"] = resources.logStore.getLogMessages();

    root.printTo(*response);
    request->send(response);
  });

  //
  // THE FOLLOWING REST-ENDPOINT SHOULD ALWAYS BE THE LAST ONE REGISTERED OF THE GET-ENDPOINTS !!!
  // As it's the least specific one it will otherwise catch the others requests.
  //

  // respond to GET requests on URL /api/v1
  web_server.on("/api/v1", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    const String host = "http://" + WiFi.localIP().toString();
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer(900);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = host + "/api/v1";
    self["method"] = "GET";

    JsonObject& history = links.createNestedObject("history");
    history["href"] = host + "/api/v1/history";
    history["method"] = "GET";

    JsonObject& login = links.createNestedObject("login");
    login["href"] = host + "/api/v1/login";

    JsonObject& manual = links.createNestedObject("manual");
    manual["href"] = host + "/api/v1/manual";
    manual["method"] = "GET";

    JsonObject& reboot = links.createNestedObject("reboot");
    reboot["href"] = host + "/api/v1/reboot";
    reboot["method"] = "PUT";

    JsonObject& factoryreset = links.createNestedObject("factoryreset");
    factoryreset["href"] = host + "/api/v1/factoryreset";
    factoryreset["method"] = "PUT";

    JsonObject& setloglevel = links.createNestedObject("loglevel");
    setloglevel["href"] = host + "/api/v1/loglevel";
    setloglevel["method"] = "GET|PUT";

    JsonObject& logmessages = links.createNestedObject("logmessages");
    logmessages["href"] = host + "/api/v1/logmessages";
    logmessages["method"] = "GET";    

    JsonObject& state = links.createNestedObject("state");
    state["href"] = host + "/api/v1/state";
    state["method"] = "PUT";

    JsonObject& status = links.createNestedObject("status");
    status["href"] = host + "/api/v1/status";
    status["method"] = "GET";

    JsonObject& system = links.createNestedObject("system");
    system["href"] = host + "/api/v1/system";
    system["method"] = "GET";

    root.printTo(*response);
    request->send(response);
  });

  web_server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/api/v1");
  });

  // respond to PUT requests on URL /api/v1/state, change state of mower.
  // example body: {"state": "TEST"}
  web_server.on("/api/v1/state", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }
  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    DynamicJsonBuffer jsonBuffer(100);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (root.containsKey("state")) {
        String state = root["state"].asString();

        if (stateController.setUserChangableState(state)) {
          request->send(200);
        } else {
          request->send(422, "text/plain", "unknown state: " + state);
        }
      } else {
        request->send(400, "text/plain", "Bad Request");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // respond to PUT requests on URL /api/v1/manual/forward, drive mower forward.
  // example body: {"speed": 50, "turnrate": 0, "smooth": false}
  web_server.on("/api/v1/manual/forward", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    DynamicJsonBuffer jsonBuffer(100);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (!root.containsKey("speed")) {
        request->send(400, "text/plain", "Bad Request - missing 'speed' parameter");
        return;
      }
      if (!root.containsKey("turnrate")) {
        request->send(400, "text/plain", "Bad Request - missing 'turnrate' parameter");
        return;
      }
      if (!root.containsKey("smooth")) {
        request->send(400, "text/plain", "Bad Request - missing 'smooth' parameter");
        return;
      }
      
      resources.wheelController.forward(root["turnrate"], root["speed"], root["smooth"]);
      request->send(200);
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // respond to PUT requests on URL /api/v1/manual/backward, drive mower backward.
  // example body: {"speed": 50, "turnrate": 0, "smooth": false}
  web_server.on("/api/v1/manual/backward", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    DynamicJsonBuffer jsonBuffer(100);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (!root.containsKey("speed")) {
        request->send(400, "text/plain", "Bad Request - missing 'speed' parameter");
        return;
      }
      if (!root.containsKey("turnrate")) {
        request->send(400, "text/plain", "Bad Request - missing 'turnrate' parameter");
        return;
      }
      if (!root.containsKey("smooth")) {
        request->send(400, "text/plain", "Bad Request - missing 'smooth' parameter");
        return;
      }
      
      resources.wheelController.backward(root["turnrate"], root["speed"], root["smooth"]);
      request->send(200);
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // respond to PUT requests on URL /api/v1/manual/turn, turn mower to specified direction (degrees 0-360).
  // example body: {"direction": 180}
 /* web_server.on("/api/v1/manual/turn", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    DynamicJsonBuffer jsonBuffer(100);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (!root.containsKey("direction")) {
        request->send(400, "text/plain", "Bad Request - missing 'direction' parameter");
        return;
      }

      resources.wheelController.turn(root["direction"]);
      request->send(200);
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });*/

  // respond to PUT requests on URL /api/v1/manual/stop, stop mower movement.
  web_server.on("/api/v1/manual/stop", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

    resources.wheelController.stop(true);
    request->send(200);
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/manual/cutter_on, start mower cutter.
  web_server.on("/api/v1/manual/cutter_on", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

    resources.cutter.start();
    request->send(200);    
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/manual/cutter_off, stop mower cutter.
  web_server.on("/api/v1/manual/cutter_off", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

    resources.cutter.stop(true);
    request->send(200);
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/reboot, restart mower.
  web_server.on("/api/v1/reboot", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    resources.cutter.stop(true);
    resources.wheelController.stop(false);
    Log.notice(F("Rebooting by API request" CR));
    request->send(200);
    delay(1000);
    ESP.restart();
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/factoryreset, reset all setting and restart mower.
  web_server.on("/api/v1/factoryreset", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }

    resources.cutter.stop(true);
    resources.wheelController.stop(false);
    Configuration::wipe();
    
    Log.notice(F("Factory reset by API request" CR));
    request->send(200);
    delay(1000);
    ESP.restart();
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  }); 

  // respond to PUT requests on URL /api/v1/loglevel, set loglevel for mower (useful for fault finding).
  web_server.on("/api/v1/loglevel", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    DynamicJsonBuffer jsonBuffer(30);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (!root.containsKey("level")) {
        request->send(400, "text/plain", "Bad Request - missing 'level' property");
        return;
      }

      Configuration::config.logLevel = atoi(root["level"]);
      Configuration::save();
      Log.notice(F("Set loglevel to %s" CR), root["level"]);

      request->send(200);
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // respond to PUT requests on URL /api/v1/apikey, trigger generation of new API key.
  web_server.on("/api/v1/apikey", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if ((request->hasParam("apiKey") &&
         request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
        !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
        {
          return request->requestAuthentication();
        }
  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    Configuration::config.apiKey = generateApikey();
    Configuration::save();
    Log.notice(F("Generated a new API key." CR));

    request->send(200);
  });
}
