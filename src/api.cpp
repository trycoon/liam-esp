#include <ArduinoLog.h>
#include "api.h"
#include "esp_log.h"
#include "definitions.h"
#include "utils.h"
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

void Api::statusToJson(statusResponse& obj, JsonObject& json) {
    json["state"] = obj.state;
    json["batteryVoltage"] = obj.batteryVoltage;
    json["batteryLevel"] = obj.batteryLevel;
    json["batteryChargeCurrent"] = obj.batteryChargeCurrent;
    json["isCharging"] = obj.isCharging;
    json["lastFullyChargeTime"] = obj.lastFullyChargeTime;
    json["lastChargeDuration"] = obj.lastChargeDuration;
    json["cutterLoad"] = obj.cutterLoad;
    json["cutterRotating"] = obj.cutterRotating;
    json["uptime"] = obj.uptime;
    json["wifiSignal"] = obj.wifiSignal;
    json["leftWheelSpd"] = obj.leftWheelSpd;
    json["rightWheelSpd"] = obj.rightWheelSpd;
    json["pitch"] = obj.pitch;
    json["roll"] = obj.roll;
    json["heading"] = obj.heading;
    JsonArray& obstacles = json.createNestedArray("obstacles");
    obstacles.add(obj.obstacleDistance1);
    obstacles.add(obj.obstacleDistance2);
    obstacles.add(obj.obstacleDistance3);
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
  if (currentStatus.batteryChargeCurrent != resources.battery.getChargeCurrent()) {
    currentStatus.batteryChargeCurrent = resources.battery.getChargeCurrent();
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
  auto obstacleDistance1 = resources.sonar.getObstacleDistance(0);
  auto obstacleDistance2 = resources.sonar.getObstacleDistance(1);
  auto obstacleDistance3 = resources.sonar.getObstacleDistance(2);
  if (currentStatus.obstacleDistance1 != obstacleDistance1) {
    currentStatus.obstacleDistance1 = obstacleDistance1;
    statusChanged = true;
  }
  if (currentStatus.obstacleDistance2 != obstacleDistance2) {
    currentStatus.obstacleDistance2 = obstacleDistance2;
    statusChanged = true;
  }
  if (currentStatus.obstacleDistance3 != obstacleDistance3) {
    currentStatus.obstacleDistance3 = obstacleDistance3;
    statusChanged = true;
  }

  // we have to check that we are connected before we try to get WiFi signal, otherwise it will freeze up.
  if (WiFi.status() == WL_CONNECTED) {
    auto wifiSignal = WiFi.RSSI();
    if (currentStatus.wifiSignal != wifiSignal) {
      currentStatus.wifiSignal = wifiSignal;
      statusChanged = true;
    }
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

    resources.wlan.sendDataWebSocket("status", root);

    // MQTT updates don't have to be "realtime", we can settle with an update every 10 sec to not spam server.
    if (lastMQTT_push < currentStatus.uptime - 10) {
      String jsonStr;
      root.printTo(jsonStr);
      resources.wlan.publish_mqtt(jsonStr.c_str(), "/status");
      lastMQTT_push = currentStatus.uptime;
    }
  }
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
    Configuration::config.apiKey = Utils::generateKey(16);
    Configuration::save();
  }

  AsyncWebServer& web_server = resources.wlan.getWebServer();
  resources.wlan.registerMqttMessageCallback(
    [this](char* topic, char* payload, size_t length) {
      onMqttMessage(topic, payload, length);
    }
  );

  // collect and check if new status should be pushed every XXX ms.
  pushNewInfoTicker.attach_ms<Api*>(400, [](Api* instance) {
    instance->collectAndPushNewStatus();     
  }, this);

  // start listening on WebSocket events.
  resources.wlan.getWebSocketServer().onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      //client connected
      Log.trace(F("ws[%s][%u] connected" CR), server->url(), client->id());
    } else if (type == WS_EVT_DISCONNECT) {
      //client disconnected
      Log.trace(F("ws[%s][%u] disconnected" CR), server->url(), client->id());
    } else if (type == WS_EVT_ERROR) {
      //error was received from the other end
      Log.error(F("ws[%s][%u] error(%u): %s" CR), server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    } else if (type == WS_EVT_DATA) {
       //data packet
      AwsFrameInfo* info = (AwsFrameInfo*)arg;

      if (info->final && info->index == 0 && info->len == len) {
        //the whole message is in a single frame and we got all of it's data
        if (info->opcode == WS_TEXT) {
          data[len] = 0;
          Log.trace("received ws-msg:\"%s\", size: %d" CR, (char*)data, len);

          DynamicJsonBuffer jsonBuffer(len);
          JsonObject& root = jsonBuffer.parseObject((const char*)data);

          if (root.success() && root.containsKey("type") && root.containsKey("payload")) {
            actOnWsRequest(root["type"], root["payload"]);
          }
        }
      }
    }
  });

  // respond to GET requests on URL /api/v1/history/battery
  web_server.on("/api/v1/history/battery", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    auto response = new AsyncJsonResponse();
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    JsonObject& root = response->getRoot();

    JsonObject& samples = root.createNestedObject("samples");
    JsonArray& time = samples.createNestedArray("time");
    JsonArray& value = samples.createNestedArray("value");
    for (auto &s: resources.battery.getBatteryHistory()) {
      time.add(s.time);
      value.add(s.batteryVoltage);
    }

    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/position
  web_server.on("/api/v1/history/position", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    auto response = new AsyncJsonResponse();
    response->addHeader("Cache-Control", "no-store, must-revalidate");    
    JsonObject& root = response->getRoot();

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: resources.gps.getGpsPositionHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["lt"] = s.lat;
        sample["lg"] = s.lng;
    }

    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history
  web_server.on("/api/v1/history", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }
    
    const String host = "http://" + WiFi.localIP().toString();
    auto response = new AsyncJsonResponse();
    JsonObject& root = response->getRoot();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& battery = links.createNestedObject("battery");
    battery["href"] = host + "/api/v1/history/battery";
    battery["method"] = "GET";

    JsonObject& position = links.createNestedObject("position");
    position["href"] = host + "/api/v1/history/position";
    position["method"] = "GET";

    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/manual
  web_server.on("/api/v1/manual", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    const String host = "http://" + WiFi.localIP().toString();
    auto response = new AsyncJsonResponse();
    JsonObject& root = response->getRoot();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& forward = links.createNestedObject("forward");
    forward["href"] = host + "/api/v1/manual/forward";
    forward["method"] = "PUT";

    JsonObject& backward = links.createNestedObject("backward");
    backward["href"] = host + "/api/v1/manual/backward";
    backward["method"] = "PUT";

    JsonObject& stop = links.createNestedObject("stop");
    stop["href"] = host + "/api/v1/manual/stop";
    stop["method"] = "PUT";

    JsonObject& cutter_on = links.createNestedObject("cutter_on");
    cutter_on["href"] = host + "/api/v1/manual/cutter_on";
    cutter_on["method"] = "PUT";

    JsonObject& cutter_off = links.createNestedObject("cutter_off");
    cutter_off["href"] = host + "/api/v1/manual/cutter_off";
    cutter_off["method"] = "PUT";

    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/status
  web_server.on("/api/v1/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    auto response = new AsyncJsonResponse();
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    
    JsonObject& root = response->getRoot();

    statusToJson(currentStatus, root);

    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/system
  web_server.on("/api/v1/system", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    auto response = new AsyncJsonResponse();
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    JsonObject& root = response->getRoot();

    root["name"] = Definitions::APP_NAME;
    root["version"] = Definitions::APP_VERSION;
    root["mowerId"] = Configuration::config.mowerId;
    root["totalHeap"] = ESP.getHeapSize();      // total heap size
    root["freeHeap"] = ESP.getFreeHeap();       // available heap
    root["minFreeHeap"] = ESP.getMinFreeHeap(); // lowest level of free heap we had since boot
    root["getMaxAllocHeap"] = ESP.getMaxAllocHeap();   // largest block of heap that can be allocated at once (heap is usually fragmented, a large value indicated low fragmentation which is good)
    root["apiKey"] = Configuration::config.apiKey;
    root["localTime"] = Utils::getTime();
    JsonObject& settings = root.createNestedObject("settings");
    settings["batteryFullVoltage"] = Definitions::BATTERY_FULLY_CHARGED;
    settings["batteryEmptyVoltage"] = Definitions::BATTERY_EMPTY;

    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/loglevel
  web_server.on("/api/v1/loglevel", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    auto response = new AsyncJsonResponse();
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    JsonObject& root = response->getRoot();

    root["level"] = Configuration::config.logLevel;

    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/logmessages?lastnr=<number>
  web_server.on("/api/v1/logmessages", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    auto response = new AsyncJsonResponse();
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    JsonObject& root = response->getRoot();
   
    uint16_t lastnrParam = request->hasParam("lastnr") ? request->getParam("lastnr")->value().toInt() : 0;
    uint16_t lastFoundMessageNr = lastnrParam;

    JsonArray& logMessages = root.createNestedArray("messages");
    auto logPage = resources.logStore.getLogMessages();

    uint16_t nrLogPosts = 0;

    for (auto &entry: logPage.messages) {
      // don't return too big sets of log messages, to keep memory and network bandwidth down we only allow 20 messages at a time.
      if (nrLogPosts >= 20) {
        break;
      }
      // only return the messages we are interested in (later than the last one we received)
      if (entry.id > lastnrParam) {
        logMessages.add(entry.message);
        lastFoundMessageNr = entry.id;
        nrLogPosts++;
      }
    }

    root["lastnr"] = lastFoundMessageNr;
    root["total"] = logPage.total;
    
    response->setLength();
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/session
  web_server.on("/api/v1/session", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (resources.wlan.isAuthenticatedSession(request)) {
      request->send(200, "text/plain", "Authorized");
    } else {
      request->send(401, "text/plain", "Unauthorized");
    }
  });

  // respond to GET requests on URL /api/v1/basic_auth
  web_server.on("/api/v1/basic_auth", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (resources.wlan.isAuthenticated(request)) {
      request->send(200, "text/plain", "Authorized");
    } else {
      request->requestAuthentication();
    }
  });

  // respond to DELETE requests on URL /api/v1/session
  web_server.on("/api/v1/session", HTTP_DELETE, [this](AsyncWebServerRequest *request) {

    resources.wlan.removeAuthenticatedSession(request);

    auto response = request->beginResponse(200);
    response->addHeader("Set-Cookie", "liam-" + Configuration::config.mowerId + "=null; HttpOnly; Path=/api; Max-Age=0");
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/schedules
  web_server.on("/api/v1/schedules", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    } else {
      auto response = new AsyncJsonResponse(true);
      response->addHeader("Cache-Control", "no-store, must-revalidate");
      JsonArray& root = response->getRoot();

      for (auto schedule : resources.mowingSchedule.getScheduleEntries()) {
        JsonObject& entry = root.createNestedObject();
        
        JsonArray& activeWeekdays = entry.createNestedArray("activeWeekdays");

        for (auto day : schedule.activeWeekdays) {
          activeWeekdays.add(day);
        }

        entry["startTime"] = schedule.startTime;
        entry["stopTime"] = schedule.stopTime;
      }

      response->setLength();
      request->send(response);
    }
  });

  // respond to DELETE requests on URL /api/v1/schedules/{position}
  web_server.on("/api/v1/schedules", HTTP_DELETE, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    } else {
      auto pos = request->url().substring(request->url().lastIndexOf("/") + 1).toInt();

      resources.mowingSchedule.removeScheduleEntry(pos);

      Log.notice(F("Removed schedule entry #%d" CR), pos);

      request->send(200);
    }
  });

  //
  // THE FOLLOWING REST-ENDPOINT SHOULD ALWAYS BE THE LAST ONE REGISTERED OF THE GET-ENDPOINTS !!!
  // As it's the least specific one it will otherwise catch the other requests.
  //

  // respond to GET requests on URL /api/v1
  web_server.on("/api/v1", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    const String host = "http://" + WiFi.localIP().toString();
    auto response = new AsyncJsonResponse();
    JsonObject& root = response->getRoot();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& history = links.createNestedObject("history");
    history["href"] = host + "/api/v1/history";
    history["method"] = "GET";

    JsonObject& login = links.createNestedObject("session");
    login["href"] = host + "/api/v1/session";
    login["method"] = "POST|GET|DELETE";

    JsonObject& basicAuth = links.createNestedObject("basic_auth");
    basicAuth["href"] = host + "/api/v1/basic_auth";
    basicAuth["method"] = "GET";

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

    JsonObject& schedule = links.createNestedObject("schedules");
    schedule["href"] = host + "/api/v1/schedules";
    schedule["method"] = "POST|GET|DELETE";

    response->setLength();
    request->send(response);
  });

  web_server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/api/v1");
  });

  // respond to PUT requests on URL /api/v1/state, change state of mower.
  // example body: {"state": "TEST"}
  web_server.on("/api/v1/state", HTTP_PUT, [this](AsyncWebServerRequest *request) {}, NULL,
  [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    DynamicJsonBuffer jsonBuffer(100);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (root.containsKey("state")) {
        String state = root["state"].as<char*>();

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

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

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
      
      // don't allow us to drive forward when we are already docked, this could damage mower or chargestation.
      if (!resources.battery.isDocked()) {
        resources.wheelController.forward(root["turnrate"], root["speed"], root["smooth"]);
      }

      request->send(200);
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // respond to PUT requests on URL /api/v1/manual/backward, drive mower backward.
  // example body: {"speed": 50, "turnrate": 0, "smooth": false}
  web_server.on("/api/v1/manual/backward", HTTP_PUT, [this](AsyncWebServerRequest *request) {

    stateController.setState(Definitions::MOWER_STATES::MANUAL);

  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

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

  // respond to PUT requests on URL /api/v1/manual/stop, stop mower movement.
  web_server.on("/api/v1/manual/stop", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);
    resources.wheelController.stop(true);

    request->send(200);
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/manual/cutter_on, start mower cutter.
  web_server.on("/api/v1/manual/cutter_on", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);
    resources.cutter.start();

    request->send(200);    
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/manual/cutter_off, stop mower cutter.
  web_server.on("/api/v1/manual/cutter_off", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    stateController.setState(Definitions::MOWER_STATES::MANUAL);
    resources.cutter.stop(true);

    request->send(200);
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/reboot, restart mower.
  web_server.on("/api/v1/reboot", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    resources.cutter.stop(true);
    resources.wheelController.stop(false);
    Log.notice(F("Rebooting by API request" CR));
    request->send(200);
    delay(100);
    ESP.restart();
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/factoryreset, reset all setting and restart mower.
  web_server.on("/api/v1/factoryreset", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    resources.cutter.stop(true);
    resources.wheelController.stop(false);
    Configuration::wipe();
    
    Log.notice(F("Factory reset by API request" CR));    
    request->send(200);
    delay(100);
    ESP.restart();
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  }); 

  // respond to PUT requests on URL /api/v1/loglevel, set loglevel for mower (useful for fault finding).
  web_server.on("/api/v1/loglevel", HTTP_PUT, [this](AsyncWebServerRequest *request) {}, NULL,
  [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }

    DynamicJsonBuffer jsonBuffer(30);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (!root.containsKey("level")) {
        request->send(400, "text/plain", F("Bad Request - missing 'level' property"));
        return;
      }

      Configuration::config.logLevel = atoi(root["level"]);
      Configuration::save();
      Log.notice(F("Set loglevel to %i" CR), Configuration::config.logLevel);

      request->send(200);
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // respond to POST requests on URL /api/v1/session, login in user and set authentication-cookie.
  web_server.on("/api/v1/session", HTTP_POST, [this](AsyncWebServerRequest *request) {}, NULL,
  [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

    DynamicJsonBuffer jsonBuffer(100);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (!root.containsKey("username")) {
        request->send(400, "text/plain", F("Bad Request - missing 'username' parameter"));
        return;
      }
      if (!root.containsKey("password")) {
        request->send(400, "text/plain", F("Bad Request - missing 'password' parameter"));
        return;
      }

      auto sessionId = resources.wlan.authenticateSession(root["username"], root["password"]);

      if (sessionId.length() > 0) {
        auto response = new AsyncJsonResponse();
        response->addHeader("Cache-Control", "no-store, must-revalidate");
        response->addHeader("Set-Cookie", "liam-" + Configuration::config.mowerId + "=" + sessionId + "; HttpOnly; Path=/api");
        response->setCode(201);
        response->setLength();
        request->send(response);
      } else {
        request->send(401, "text/plain", "Unauthorized");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // respond to POST requests on URL /api/v1/apikey, trigger generation of new API key.
  web_server.on("/api/v1/apikey", HTTP_POST, [this](AsyncWebServerRequest *request) {}, NULL,
  [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }
        
    Configuration::config.apiKey = Utils::generateKey(16);
    Configuration::save();
    Log.notice(F("Generated a new API key." CR));

    request->send(201);
  });

  // respond to POST requests on URL /api/v1/schedules, add mower schedule entry.
  web_server.on("/api/v1/schedules", HTTP_POST, [this](AsyncWebServerRequest *request) {}, NULL,
  [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!resources.wlan.isAuthenticated(request)) {
      return request->send(401, "text/plain", "Unauthorized");
    }
    
    DynamicJsonBuffer jsonBuffer(20);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (!root.containsKey("activeWeekdays")) {
        request->send(400, "text/plain", F("Bad Request - missing 'activeWeekdays'"));
        return;
      }
      if (!root.containsKey("startTime")) {
        request->send(400, "text/plain", F("Bad Request - missing 'startTime'"));
        return;
      }
      if (!root.containsKey("stopTime")) {
        request->send(400, "text/plain", F("Bad Request - missing 'stopTime'"));
        return;
      }

      std::deque<bool> activeWeekdays;

      for (const auto& day : root["activeWeekdays"].as<JsonArray>()) {
        activeWeekdays.push_back(day);
      }
 
      auto result = resources.mowingSchedule.addScheduleEntry(activeWeekdays, root["startTime"], root["stopTime"]);

      if (result >= 0) {
        Log.notice(F("Added a new schedule entry." CR));
        request->send(201);
        return;
      } else if (result == -1) {
        request->send(400, "text/plain", F("Bad Request - malformed parameter 'activeWeekdays'"));
        return;
      } else if (result == -2) {
        request->send(400, "text/plain", F("Bad Request - malformed parameter 'startTime'"));
        return;
      } else if (result == -3) {
        request->send(400, "text/plain", F("Bad Request - malformed parameter 'stopTime'"));
        return;
      } else if (result == -4) {
        request->send(400, "text/plain", F("Schedule limit reached, max 10 entries allowed!"));
        return;
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });
}

void Api::actOnWsRequest(String type, JsonObject& payload) {
  if (type == "forward") {
    resources.wheelController.forward(payload["turnrate"], payload["speed"], payload["smooth"]);
  } else if (type == "backward") {
    resources.wheelController.backward(payload["turnrate"], payload["speed"], payload["smooth"]);
  } else {
    Log.notice(F("unknown ws-request received: %s" CR), type);
  }
}
