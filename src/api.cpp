#include "api.h"
#include <ArduinoJson.h>
#include "esp_log.h"
#include "definitions.h"
#include "io_accelerometer/io_accelerometer.h"

static const char *TAG = "API";

/**
* REST-API class defining all the endpoints.
* We are aiming for a self explaining API that resembles the HATEOAS constraints.
*/
Api::Api(StateController& stateController, Resources& resources) :
  stateController(stateController),
  resources(resources){}

void Api::setupApi(AsyncWebServer& web_server) {

  // HTTP basic authentication
  web_server.on("/api/v1/login", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    request->send(200, "text/plain", "Login Success!");
  });

  // respond to GET requests on URL /api/v1/history/battery
  web_server.on("/api/v1/history/battery", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
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
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history/position";
    self["method"] = "GET";

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: resources.metrics.getGpsPositionHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["lat"] = s.lat;
        sample["lng"] = s.lng;
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history
  web_server.on("/api/v1/history", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
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
  web_server.on("/api/v1/manual", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
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
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/status";
    self["method"] = "GET";

    root["state"] = stateController.getStateInstance()->getStateName();
    root["batteryVoltage"] = resources.battery.getBatteryVoltage();
    root["batteryLevel"] = resources.battery.getBatteryStatus();
    root["isCharging"] = resources.battery.isCharging();
    root["lastFullyChargeTime"] = resources.battery.getLastFullyChargeTime();
    root["lastChargeDuration"] = resources.battery.getLastChargeDuration();
    root["cutterLoad"] = resources.cutter.getLoad();

    orientation orient = resources.accelerometer.getOrientation();
    root["pitch"] = orient.pitch;
    root["roll"] = orient.roll;
    root["yaw"] = orient.heading;

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/system
  web_server.on("/api/v1/system", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/system";
    self["method"] = "GET";

    root["name"] = Definitions::APP_NAME;
    root["version"] = Definitions::APP_VERSION;
    root["cpuFreq"] = ESP.getCpuFreqMHz();
    root["flashChipSize"] = ESP.getFlashChipSize();
    root["freeHeap"] = ESP.getFreeHeap();
    root["wifiSignal"] = WiFi.RSSI();

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1
  web_server.on("/api/v1", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
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

  // PUT, POST requests.
  web_server.onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

    if (!request->authenticate(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str())) {
      return request->requestAuthentication();
    }

    if (request->method() == HTTP_PUT) {
      // respond to PUT requests on URL /api/v1/state, change state of mower.
      // example body: {"state": "TEST"}
      if (request->url() == "/api/v1/state") {
        DynamicJsonBuffer jsonBuffer;
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
      }

      // respond to PUT requests on URL /api/v1/manual/forward, drive mower forward.
      // example body: {"speed": 50, "turnrate": 0, "smooth": false}
      else if (request->url() == "/api/v1/manual/forward") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        DynamicJsonBuffer jsonBuffer;
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
          Serial.printf("root[turnrate]=%d, root[speed]=%d", atoi(root["turnrate"]), atoi(root["speed"]));
          resources.wheelController.forward(root["turnrate"], root["speed"], root["smooth"]);
          request->send(200);
        } else {
          request->send(400, "text/plain", "Bad Request");
        }
      }

      // respond to PUT requests on URL /api/v1/manual/backward, drive mower backward.
      else if (request->url() == "/api/v1/manual/backward") {
        // example body: {"speed": 50, "turnrate": 0, "smooth": false}
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        DynamicJsonBuffer jsonBuffer;
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
      }

      // respond to PUT requests on URL /api/v1/manual/turn, turn mower to specified direction (degrees 0-360).
      // example body: {"direction": 180}
      else if (request->url() == "/api/v1/manual/turn") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        DynamicJsonBuffer jsonBuffer;
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
      }

      // respond to PUT requests on URL /api/v1/manual/stop, stop mower movement.
      else if (request->url() == "/api/v1/manual/stop") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        resources.wheelController.stop(true);
        request->send(200);
      }

      // respond to PUT requests on URL /api/v1/manual/cutter_on, start mower cutter.
      else if (request->url() == "/api/v1/manual/cutter_on") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        resources.cutter.start();
        request->send(200);
      }

      // respond to PUT requests on URL /api/v1/manual/cutter_off, stop mower cutter.
      else if (request->url() == "/api/v1/manual/cutter_off") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        resources.cutter.stop(true);
        request->send(200);
      }

      // respond to PUT requests on URL /api/v1/reboot, restart mower.
      else if (request->url() == "/api/v1/reboot") {
        resources.cutter.stop(true);
        resources.wheelController.stop(false);
        Serial.println("Rebooting by API request");
        request->send(200);
        delay(1000);
        ESP.restart();
      }

      // respond to PUT requests on URL /api/v1/factoryreset, reset all setting and restart mower.
      else if (request->url() == "/api/v1/factoryreset") {
        resources.cutter.stop(true);
        resources.wheelController.stop(false);
        //presenent.clear();
        Serial.println("Factory reset by API request");
        request->send(200);
        delay(1000);
        ESP.restart();
      }
    }

    //Serial.printf("Resource not found: http://%s%s\n", request->host().c_str(), request->url().c_str());
    //request->send(404, "text/plain", "Not found");
  });
}
