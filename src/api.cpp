#include "api.h"
#include "esp_log.h"
#include "definitions.h"
#include "io_accelerometer/io_accelerometer.h"

static const char *TAG = "API";

/**
* REST-API class defining all the endpoints.
* We are aiming for a self explaining API that resembles the HATEOAS specification.
* 
* Usually communication are driven by clients making HTTP-requests to this server,
* one exception to this is the /status endpoint that also is available for subscription using WebSockets.
* Since the /status endpoint is so commonly used and requested by clients we actively push status updates to the connected clients,
* making the clients more responsive and it also taxes the server less.
*/
Api::Api(StateController& stateController, Resources& resources) :
  stateController(stateController),
  resources(resources) {}

/**
 * Method for sending information over WebSocket.
 * @param json message to be sent
 * @param client [optional] client to send to, if none specified then we broadcast to everybody
 */
void Api::sendDataWebSocket(JsonObject& json, AsyncWebSocketClient* client) {
  auto len = json.measureLength();
  AsyncWebSocketMessageBuffer* buffer = websockeServer->makeBuffer(len); // creates a buffer (len + 1) for you.

  if (buffer) {
      json.printTo((char*)buffer->get(), len + 1);

      if (client) {
        client->text(buffer);
      } else {
        websockeServer->textAll(buffer);
      }
  }
}

void Api::statusToJson(statusResponse obj, JsonObject& json) {
    json["state"] = obj.state;
    json["batteryVoltage"] = obj.batteryVoltage;
    json["batteryLevel"] = obj.batteryLevel;
    json["isCharging"] = obj.isCharging;
    json["lastFullyChargeTime"] = obj.lastFullyChargeTime;
    json["lastChargeDuration"] = obj.lastChargeDuration;
    json["cutterLoad"] = obj.cutterLoad;
    json["cutterRotating"] = obj.cutterRotating;

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

  if (statusChanged) {
    DynamicJsonBuffer jsonBuffer(512);
    JsonObject& root = jsonBuffer.createObject();
    statusToJson(currentStatus, root);

    sendDataWebSocket(root);

    String jsonStr;
    root.printTo(jsonStr);
    Serial.println(jsonStr);
    resources.mqtt.publish_message(jsonStr.c_str(), "/status");
  }
}

void Api::setupApi(AsyncWebServer& web_server, AsyncWebSocket& websocket_server) {

  websockeServer = &websocket_server;

    // collect and check if new status should be pushed every XXX ms.
  pushNewInfoTicker.attach_ms<Api*>(400, [](Api* instance) {
    instance->collectAndPushNewStatus();     
  }, this);

  //HTTP Authenticate before switch to Websocket protocol
  //websocket_server.setAuthentication(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str());

  // HTTP basic authentication
  web_server.on("/api/v1/login", HTTP_GET, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    auto *response = request->beginResponse(200, "text/plain", "Login Success!");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/battery
  web_server.on("/api/v1/history/battery", HTTP_GET, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    DynamicJsonBuffer jsonBuffer(20000);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history/battery";
    self["method"] = "GET";

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: resources.metrics.getBatteryHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["v"] = s.batteryVoltage;
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/position
  web_server.on("/api/v1/history/position", HTTP_GET, [this](AsyncWebServerRequest *request) {
   /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");    
    DynamicJsonBuffer jsonBuffer(60000);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history/position";
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
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    auto *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer(350);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history";
    self["method"] = "GET";

    JsonObject& battery = links.createNestedObject("battery");
    battery["href"] = "/api/v1/history/battery";
    battery["method"] = "GET";

    JsonObject& position = links.createNestedObject("position");
    position["href"] = "/api/v1/history/position";
    position["method"] = "GET";

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/manual
  web_server.on("/api/v1/manual", HTTP_GET, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    auto *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer(750);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/manual";
    self["method"] = "GET";

    JsonObject& forward = links.createNestedObject("forward");
    forward["href"] = "/api/v1/manual/forward";
    forward["method"] = "PUT";

    JsonObject& backward = links.createNestedObject("backward");
    backward["href"] = "/api/v1/manual/backward";
    backward["method"] = "PUT";

    JsonObject& left = links.createNestedObject("turn");
    left["href"] = "/api/v1/manual/turn";
    left["method"] = "PUT";

    JsonObject& stop = links.createNestedObject("stop");
    stop["href"] = "/api/v1/manual/stop";
    stop["method"] = "PUT";

    JsonObject& cutter_on = links.createNestedObject("cutter_on");
    cutter_on["href"] = "/api/v1/manual/cutter_on";
    cutter_on["method"] = "PUT";

    JsonObject& cutter_off = links.createNestedObject("cutter_off");
    cutter_off["href"] = "/api/v1/manual/cutter_off";
    cutter_off["method"] = "PUT";

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/status
  web_server.on("/api/v1/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");
    
    DynamicJsonBuffer jsonBuffer(600);
    JsonObject& root = jsonBuffer.createObject();
    statusToJson(currentStatus, root);

    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/status";
    self["method"] = "GET";

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/system
  web_server.on("/api/v1/system", HTTP_GET, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    auto *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "no-store, must-revalidate");    
    DynamicJsonBuffer jsonBuffer(400);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/system";
    self["method"] = "GET";
    root["name"] = Definitions::APP_NAME;
    root["version"] = Definitions::APP_VERSION;
    root["uptime"] = (uint32_t)(esp_timer_get_time() / 1000000); // uptime in microseconds so we divide.
    root["cpuFreq"] = ESP.getCpuFreqMHz();
    root["flashChipSize"] = ESP.getFlashChipSize();
    root["freeHeap"] = ESP.getFreeHeap();
    root["wifiSignal"] = WiFi.RSSI();

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1
  web_server.on("/api/v1", HTTP_GET, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer(860);
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1";
    self["method"] = "GET";

    JsonObject& history = links.createNestedObject("history");
    history["href"] = "/api/v1/history";
    history["method"] = "GET";

    JsonObject& login = links.createNestedObject("login");
    login["href"] = "/api/v1/login";

    JsonObject& manual = links.createNestedObject("manual");
    manual["href"] = "/api/v1/manual";
    manual["method"] = "GET";

    JsonObject& reboot = links.createNestedObject("reboot");
    reboot["href"] = "/api/v1/reboot";
    reboot["method"] = "PUT";

    JsonObject& factoryreset = links.createNestedObject("factoryreset");
    factoryreset["href"] = "/api/v1/factoryreset";
    factoryreset["method"] = "PUT";

    JsonObject& state = links.createNestedObject("state");
    state["href"] = "/api/v1/state";
    state["method"] = "PUT";

    JsonObject& status = links.createNestedObject("status");
    status["href"] = "/api/v1/status";
    status["method"] = "GET";

    JsonObject& system = links.createNestedObject("system");
    system["href"] = "/api/v1/system";
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
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/
  }, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    DynamicJsonBuffer jsonBuffer(100);
    JsonObject& root = jsonBuffer.parseObject((const char*)data);

    if (root.success()) {
      if (root.containsKey("state")) {
        String state = root["state"].asString();

        if (state == "LAUNCHING") {
          stateController.setState(Definitions::MOWER_STATES::LAUNCHING);
          request->send(200);
        } else if (state == "MOWING") {
          stateController.setState(Definitions::MOWER_STATES::MOWING);
          request->send(200);
        } else if (state == "DOCKING") {
          stateController.setState(Definitions::MOWER_STATES::DOCKING);
          request->send(200);
        } else if (state == "STOP") {
          stateController.setState(Definitions::MOWER_STATES::STOP);
          request->send(200);
        } else if (state == "TEST") {
          stateController.setState(Definitions::MOWER_STATES::TEST);
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
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
      stateController.setState(Definitions::MOWER_STATES::MANUAL);
    }
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
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
      stateController.setState(Definitions::MOWER_STATES::MANUAL);
    }
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
  web_server.on("/api/v1/manual/turn", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
      stateController.setState(Definitions::MOWER_STATES::MANUAL);
    }
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
  });

  // respond to PUT requests on URL /api/v1/manual/stop, stop mower movement.
  web_server.on("/api/v1/manual/stop", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
      stateController.setState(Definitions::MOWER_STATES::MANUAL);
    }

    resources.wheelController.stop(true);
    request->send(200);
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/manual/cutter_on, start mower cutter.
  web_server.on("/api/v1/manual/cutter_on", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
      stateController.setState(Definitions::MOWER_STATES::MANUAL);
    }

    resources.cutter.start();
    request->send(200);    
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/manual/cutter_off, stop mower cutter.
  web_server.on("/api/v1/manual/cutter_off", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
      stateController.setState(Definitions::MOWER_STATES::MANUAL);
    }

    resources.cutter.stop(true);
    request->send(200);
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/reboot, restart mower.
  web_server.on("/api/v1/reboot", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    resources.cutter.stop(true);
    resources.wheelController.stop(false);
    Serial.println("Rebooting by API request");
    request->send(200);
    delay(1000);
    ESP.restart();
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  });

  // respond to PUT requests on URL /api/v1/factoryreset, reset all setting and restart mower.
  web_server.on("/api/v1/factoryreset", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    /*if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }*/

    resources.cutter.stop(true);
    resources.wheelController.stop(false);
    Configuration::wipe();
    
    Serial.println("Factory reset by API request");
    request->send(200);
    delay(1000);
    ESP.restart();
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    // nothing.
  }); 
}
