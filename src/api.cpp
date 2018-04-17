#include "api.h"
#include "ArduinoJson.h"
#include "definitions.h"
#include "io_accelerometer.h"


Api::Api(StateController& stateController, WheelController& wheelController, Battery& battery, Cutter& cutter, GPS& gps, IO_Accelerometer& tilt, Metrics& metrics) :
  stateController(stateController),
  wheelController(wheelController),
  battery(battery),
  cutter(cutter),
  gps(gps),
  tilt(tilt),
  metrics(metrics) {}

void Api::setupApi(AsyncWebServer& web_server) {

  // HTTP basic authentication
  web_server.on("/api/v1/login", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate("admin", "pass")) {
        return request->requestAuthentication();
    }

    request->send(200, "text/plain", "Login Success!");
  });

  // respond to GET requests on URL /api/v1/history/battery
  web_server.on("/api/v1/history/battery", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history/battery";

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: metrics.getBatteryHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["v"] = s.batteryVoltage;
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/cutter_load
  web_server.on("/api/v1/history/cutter_load", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history/cutter_load";

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: metrics.getCutterLoadHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["v"] = s.cutterLoad;
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/position
  web_server.on("/api/v1/history/position", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history/position";

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: metrics.getGpsPositionHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["lat"] = s.lat;
        sample["lng"] = s.lng;
        sample["dir"] = 0;
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history/memory
  web_server.on("/api/v1/history/memory", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history/memory";

    JsonArray& samples = root.createNestedArray("samples");
    for (auto &s: metrics.getMemoryHistory()) {
        JsonObject& sample = samples.createNestedObject();
        sample["t"] = s.time;
        sample["v"] = s.memory;
    }

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/history
  web_server.on("/api/v1/history", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/history";

    JsonObject& battery = links.createNestedObject("battery");
    battery["href"] = "/api/v1/history/battery";

    JsonObject& cutterLoad = links.createNestedObject("cutterLoad");
    cutterLoad["href"] = "/api/v1/history/cutter_load";

    JsonObject& position = links.createNestedObject("position");
    position["href"] = "/api/v1/history/position";

    JsonObject& memory = links.createNestedObject("memory");
    memory["href"] = "/api/v1/history/memory";

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/manual
  web_server.on("/api/v1/manual", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/manual";

    JsonObject& forward = links.createNestedObject("forward");
    forward["href"] = "/api/v1/manual/forward";

    JsonObject& backward = links.createNestedObject("backward");
    backward["href"] = "/api/v1/manual/backward";

    JsonObject& left = links.createNestedObject("left");
    left["href"] = "/api/v1/manual/left";

    JsonObject& right = links.createNestedObject("right");
    right["href"] = "/api/v1/manual/right";

    JsonObject& stop = links.createNestedObject("stop");
    stop["href"] = "/api/v1/manual/stop";

    JsonObject& cutter_on = links.createNestedObject("cutter_on");
    cutter_on["href"] = "/api/v1/manual/cutter_on";

    JsonObject& cutter_off = links.createNestedObject("cutter_off");
    cutter_off["href"] = "/api/v1/manual/cutter_off";

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/status
  web_server.on("/api/v1/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/status";

    root["state"] = stateController.getStateInstance()->getStateName();
    root["batteryVoltage"] = battery.getBatteryVoltage();
    root["batteryLevel"] = 0;
    root["cutterLoad"] = cutter.getLoad();
    root["gps_latitude"] = "0";
    root["gps_longitude"] = "0";
    root["gps_heading"] = "0";

    orientation orient = tilt.getOrientation();
    root["tilt_pitch"] = orient.pitch;
    root["tilt_roll"] = orient.roll;

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1/system
  web_server.on("/api/v1/system", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");
    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1/system";

    root["name"] = Definitions::APP_NAME;
    root["version"] = Definitions::APP_VERSION;
    root["coreVersion"] = ESP.getCoreVersion();
    root["cpuFreq"] = ESP.getCpuFreqMHz();
    root["flashChipSize"] = ESP.getFlashChipSize();
    root["freeHeap"] = ESP.getFreeHeap();
    root["freeSketchSpace"] = ESP.getFreeSketchSpace();
    root["sketchSize"] = ESP.getSketchSize();
    root["resetReason"] = ESP.getResetReason();
    root["resetInfo"] = ESP.getResetInfo();
    root["wifiSignal"] = WiFi.RSSI();

    root.printTo(*response);
    request->send(response);
  });

  // respond to GET requests on URL /api/v1
  web_server.on("/api/v1", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& links = root.createNestedObject("_links");

    JsonObject& self = links.createNestedObject("self");
    self["href"] = "/api/v1";

    JsonObject& history = links.createNestedObject("history");
    history["href"] = "/api/v1/history";

    JsonObject& login = links.createNestedObject("login");
    login["href"] = "/api/v1/login";

    JsonObject& manual = links.createNestedObject("manual");
    manual["href"] = "/api/v1/manual";

    JsonObject& reboot = links.createNestedObject("reboot");
    reboot["href"] = "/api/v1/reboot";

    JsonObject& state = links.createNestedObject("state");
    state["href"] = "/api/v1/state";

    JsonObject& status = links.createNestedObject("status");
    status["href"] = "/api/v1/status";

    JsonObject& system = links.createNestedObject("system");
    system["href"] = "/api/v1/system";

    root.printTo(*response);
    request->send(response);
  });

  web_server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/api/v1");
  });

  // PUT, POST request.
  web_server.onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

    if (request->method() == HTTP_PUT) {
      // respond to PUT requests on URL /api/v1/state, change state of mower. e.g. {"state": "TEST"}
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
            } else if (state == "PAUSED") {
              stateController.setState(Definitions::MOWER_STATES::PAUSED);
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

        return;
      }

      // respond to PUT requests on URL /api/v1/manual/forward, drive mower forward.
      if (request->url() == "/api/v1/manual/forward") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        wheelController.forward(true);
        request->send(200);

        return;
      }

      // respond to PUT requests on URL /api/v1/manual/backward, drive mower backward.
      if (request->url() == "/api/v1/manual/backward") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        wheelController.backward(true);
        request->send(200);

        return;
      }

      // respond to PUT requests on URL /api/v1/manual/left, turn mower left.
      if (request->url() == "/api/v1/manual/left") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        wheelController.turnLeft(true);
        request->send(200);

        return;
      }

      // respond to PUT requests on URL /api/v1/manual/right, turn mower right.
      if (request->url() == "/api/v1/manual/right") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        wheelController.turnRight(true);
        request->send(200);

        return;
      }

      // respond to PUT requests on URL /api/v1/manual/stop, stop mower movement.
      if (request->url() == "/api/v1/manual/stop") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        wheelController.stop(true);
        request->send(200);

        return;
      }

      // respond to PUT requests on URL /api/v1/manual/cutter_on, start mower cutter.
      if (request->url() == "/api/v1/manual/cutter_on") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        cutter.start();
        request->send(200);

        return;
      }

      // respond to PUT requests on URL /api/v1/manual/cutter_off, stop mower cutter.
      if (request->url() == "/api/v1/manual/cutter_off") {
        if (stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::MANUAL) {
          stateController.setState(Definitions::MOWER_STATES::MANUAL);
        }

        cutter.stop(true);
        request->send(200);

        return;
      }

      // respond to PUT requests on URL /api/v1/reboot, restart mower.
      if (request->url() == "/api/v1/reboot") {
        Serial.println("Rebooting by API request");
        request->send(200);
        delay(1000);
        ESP.restart();

        return;
      }
    }

    Serial.printf("Resource not found: http://%s%s\n", request->host().c_str(), request->url().c_str());
    request->send(404, "text/plain", "Not found");
  });
}
/*
void processCommand(char* msgBuffer) {
    StaticJsonBuffer<BUFF_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(msgBuffer);

    if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
    }

    yield();
    const char* cmd = root["cmd"];
    char msg[1024];
    if (strcmp(cmd, "heartBeat")  == 0) {
        root.printTo(msg, sizeof(msg));
        safeSend(msg);
    } else if (strcmp(cmd, "identify")  == 0) {
        StaticJsonBuffer<1024> jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();

        //SIZE FOR BASE INFO WITHOUT SERVICES MAY BE ~512 so there 512 jsonBuffer for services array: https://github.com/bblanchon/ArduinoJson/wiki/Memory-model
        json["cmd"] = "identify";
        json["chipId"] = ESP.getChipId();
        json["freeSketchSpace"] = ESP.getFreeSketchSpace();

        JsonObject& version = json.createNestedObject("version");
        version["sdk"] = system_get_sdk_version();
        version["major"] = MAJOR_VERSION;
        version["minor"] = MINOR_VERSION;

        rst_info* resetInfo = system_get_rst_info();
        JsonObject& reset = json.createNestedObject("reset");
        reset["reason"] = resetInfo->reason;
        reset["exccause"] = resetInfo->exccause;
        reset["epc1"] = resetInfo->epc1;
        reset["epc2"] = resetInfo->epc2;
        reset["epc3"] = resetInfo->epc3;
        reset["excvaddr"] = resetInfo->excvaddr;
        reset["depc"] = resetInfo->depc;

        JsonObject& flash = json.createNestedObject("flash");
        flash["id"] = ESP.getFlashChipId();
        flash["size"] = ESP.getFlashChipSize();
        flash["realSize"] = ESP.getFlashChipRealSize();
        flash["speed"] = ESP.getFlashChipSpeed();
        FlashMode_t ideMode = ESP.getFlashChipMode();
        flash["mode"] = (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");

        const char* phyModes[] = { "", "B", "G", "N" };
        JsonObject& wifi = json.createNestedObject("wifi");
        wifi["channel"] = wifi_get_channel();
        wifi["reconnect"] = wifi_station_get_auto_connect();
        wifi["phyMode"] = phyModes[wifi_get_phy_mode()];
        wifi["ssid"] = WiFi.SSID();
        wifi["bssid"] = WiFi.BSSIDstr();
        wifi["mac"] = WiFi.macAddress();
        wifi["ip"] = IP2Str(WiFi.localIP());
        wifi["mask"] = IP2Str(WiFi.subnetMask());
        wifi["gateway"] = IP2Str(WiFi.gatewayIP());
        wifi["dns"] = IP2Str(WiFi.dnsIP());
        wifi["hostname"] = WiFi.hostname();

        JsonArray& services = json.createNestedArray("services");
        for (unsigned int i = 0; i < 10; i++) {
            JsonObject& service = services.createNestedObject();
            service["keyA"] = "AAA";
            service["keyB"] = "BBB";
            service["keyC"] = 111;
            yield();
        }

        json.printTo(msg, sizeof(msg));
        safeSend(msg);
    } else if(strcmp(cmd, "restart")  == 0) {
        ESP.restart();
    } else if(strcmp(cmd, "perform")  == 0) {

    }
}
*/
