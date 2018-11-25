#include "wifi.h"
#include <WiFi.h>
#include <string>
#include <time.h>
#include <SPIFFS.h>
#include <cassert>
#include <ESPmDNS.h>
#include <ArduinoLog.h>
#include <Update.h>
#include "definitions.h"
#include "configuration.h"
#include "log_store.h"

// TODO: AVOID "%" in CSS, https://github.com/me-no-dev/ESPAsyncWebServer/pull/366
static const char SETUP_HTML[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"><title>Liam-ESP</title><style>fieldset{padding:1em;font:0.8em/1 sans-serif;margin:1em}legend{padding:0.2em 0.5em;border:1px solid black;font-size:0.9em}label{float:left;margin-right:0.5em;padding-top:0.2em;text-align:right;font-weight:bold;width:6em}input{margin-bottom:0.4em;padding-left: 0.2em;}.center{display:block;margin-right:auto;margin-left:auto;text-align:center}a:link{text-decoration:none;}button,.button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2rem;font-size:1.1rem;width:6em;}</style></head><body><h1 class=\"center\">Setup</h1><form action=\"/setup\" method=\"post\"><fieldset><legend>WiFi</legend><label>SSID: </label><input type=\"text\" name=\"SSID\" length=32 value=\"%SSID%\" required><br><label>Password: </label><input type=\"password\" name=\"WIFI_PASSWORD\" length=64 value=\"%WIFI_PASSWORD%\"></fieldset><fieldset><legend>Administrator</legend><label>Username: </label><input type=\"text\" name=\"USERNAME\" length=20 value=\"%USERNAME%\" required><br><label>Password: </label><input type=\"password\" name=\"PASSWORD\" length=20 value=\"%PASSWORD%\" required></fieldset><fieldset><legend>Time</legend><label>NTP-server: </label><input type=\"text\" name=\"NTP_SERVER\" length=40 value=\"%NTP_SERVER%\"><br><label>Time zone: </label><input type=\"number\" name=\"GMT\"  min=\"-12\" max=\"12\" value=\"%GMT%\">&nbsp;<a href=\"https://upload.wikimedia.org/wikipedia/commons/e/e8/Standard_World_Time_Zones.png\" target=\"_blank\">&nbsp;i&nbsp;</a></fieldset><fieldset><legend>MQTT</legend><label>Server IP: </label><input type=\"text\" name=\"MQTT_SERVER\" placeholder=\"leave blank if not used\" length=64 value=\"%MQTT_SERVER%\"><br><label>Port: </label><input type=\"number\" name=\"MQTT_PORT\" placeholder=\"leave blank if not used\" min=\"1024\" max=\"65535\" value=\"%MQTT_PORT%\"><br><label>Subscribe: </label><input type=\"text\" name=\"MQTT_TOPIC\" placeholder=\"leave blank if not used\" length=200 value=\"%MQTT_TOPIC%\"><br><label>Command: </label><input type=\"text\" name=\"MQTT_TOPIC_COMMAND\" placeholder=\"leave blank if not used\" length=200 value=\"%MQTT_TOPIC_COMMAND%\"></fieldset><input type=\"submit\" value=\"Save\" class=\"center button\"></form></body></html>";
static const char NO_WEB_UI[] PROGMEM = "Web interface is not available. See README.md for instructions about how to flash interface into mower.";
WiFi_Client* WiFi_Client::Instance = nullptr;

// Constructor
WiFi_Client::WiFi_Client() : web_server(80), ws("/ws") {  
  assert(!Instance);
  Instance = this;
}

// Method that get called upon when the template engine renders the SETUP_HTML-page and finds a "%<placeholder>%"-element.
// We populate the HTML with settings saved from Flash memory (or default values).
String WiFi_Client::renderPlaceholder(const String& placeholder) {
  if (placeholder == "USERNAME") {
    return Configuration::config.username;
  } else if (placeholder == "PASSWORD") {
    return Configuration::config.password;
  } else if (placeholder == "MQTT_SERVER") {
    return Configuration::config.mqttServer;
  } else if (placeholder == "MQTT_PORT") {
    return Configuration::config.mqttPort;
  } else if (placeholder == "MQTT_TOPIC") {
    return Configuration::config.mqttTopic;
  } else if (placeholder == "MQTT_TOPIC_COMMAND") {
    return Configuration::config.mqttTopicCommand;
  } else if (placeholder == "NTP_SERVER") {
    return Configuration::config.ntpServer;
  } else if (placeholder == "GMT") {
    return Configuration::config.gmt;
  } else if (placeholder == "WIFI_PASSWORD") {
    return Configuration::config.wifiPassword;
  } else if (placeholder == "SSID") {
    return Configuration::config.ssid;
  } else {
    return String();
  }
}

String WiFi_Client::getTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo, 5000)) { // tries for 5000 ms
    return F("Failed to obtain time");
  }

  char outstr[80];
  strftime(outstr, sizeof(outstr), "%d %b %Y, %H:%M:%S%z", &timeinfo); // ISO 8601 time

  return String(outstr);
}

// WiFi status eventhandler, called upon when WiFi connection change status.
void WiFi_Client::WiFiEvent(WiFiEvent_t event, system_event_info_t info) {
    switch(event) {
      case SYSTEM_EVENT_AP_START: {
        // When we run as an accesspoint
        WiFi.softAPsetHostname(Definitions::APP_NAME);
        //WiFi.softAPenableIpV6();
        break;
      case SYSTEM_EVENT_STA_START:
        // When we run in Station-mode (are connected to an accesspoint as client)
        WiFi.setHostname(Definitions::APP_NAME);
        break;
      case SYSTEM_EVENT_STA_CONNECTED:
        //WiFi.enableIpV6();
        break;
      /*case SYSTEM_EVENT_AP_STA_GOT_IP6:
        //both interfaces get the same event
        Log.trace("STA IPv6: %s, AP IPv6: %s" CR, WiFi.localIPv6().toString().c_str(), WiFi.softAPIPv6().toString().c_str());
        break;*/
      case SYSTEM_EVENT_STA_GOT_IP:
        Instance->onWifiConnect(event, info);
        break;
      case SYSTEM_EVENT_STA_LOST_IP:
        Instance->onWifiDisconnect(event, info);
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        Instance->onWifiDisconnect(event, info);
        break;
      default:
        break; // just to prevent us from getting compile warnings.
    }
  }
}

// Setup and connect WiFi, should only be called upon once.
void WiFi_Client::start() {

  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(Definitions::APP_NAME);
  Log.notice(F("AP Started, AP SSID: \"%s\", AP IPv4: %s" CR), Definitions::APP_NAME, WiFi.softAPIP().toString().c_str());
  
  // Annonce us on WiFi network using Multicast DNS. This should be run BEFORE WiFi connect.
  if (MDNS.begin(Definitions::APP_NAME)) {
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 80);
  } else {
    Log.warning("Failed setup mDNS" CR);
  }

  connect();

  setupWebServer();

  if (isMQTT_enabled()) {
    mqttClient.onConnect([this](bool sessionPresent) {
      onMqttConnect(sessionPresent);
    });

    mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
      onMqttDisconnect(reason);
    });

    mqttClient.onPublish([this](uint16_t packetId) {
      onMqttPublish(packetId);
    });

    mqttClient.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
      // iterate through callback list and call each one
      for (const auto &cb : onMqttMessageCallbacks) {
        cb(topic, payload, len);
      }
    });

    uint8_t ip[4];
    sscanf(Configuration::config.mqttServer.c_str(), "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    auto mqtt_ip = IPAddress(ip[0], ip[1], ip[2], ip[3]);
    auto mqtt_port = Configuration::config.mqttPort;
    
    Log.notice(F("using MQTT server IP: %s, and port: %s" CR), mqtt_ip.toString().c_str(), mqtt_port.c_str());

    mqttClient.setServer(mqtt_ip, mqtt_port.toInt());
    //mqttClient.setCredentials("MQTT_USERNAME", "MQTT_PASSWORD");
    mqttClient.setKeepAlive(15); // seconds
    mqttClient.setClientId(Definitions::APP_NAME);
    mqttClient.setWill(Configuration::config.mqttTopic.c_str(), 2, true, "DISCONNECTED");
  }
}

void WiFi_Client::setupWebServer() {
    // Add CORS support, to fix error on iOS. https://github.com/me-no-dev/ESPAsyncWebServer/blob/d7399a7664bce76e9939837fd6cf51bd272ef588/README.md#adding-default-headers
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    // remove all previous rewrites, handlers and onNotFound/onFileUpload/onRequestBody callbacks (unsure if web_server.begin() does that for us)
    web_server.reset();

    web_server.addHandler(&ws);

    //HTTP Authenticate before switch to Websocket protocol
    ws.setAuthentication(Configuration::config.username.c_str(), Configuration::config.password.c_str());

    web_server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request) {
      // TODO: list available accesspoints to choose between. https://github.com/switchdoclabs/SDL_ESP32_BC24_GETIP/blob/master/WiFiManager.cpp#L436
      request->send_P(200, "text/html", SETUP_HTML, renderPlaceholder);
    });
    // Handle Post-back form from setup page.
    web_server.on("/setup", HTTP_POST, [](AsyncWebServerRequest *request) {
      if ((request->hasParam("apiKey") &&
        request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
      !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
      {
        return request->requestAuthentication();
      }

      if(request->hasParam("USERNAME", true)){
        Configuration::config.username = request->getParam("USERNAME", true)->value();
      }
      if(request->hasParam("PASSWORD", true)){
        Configuration::config.password = request->getParam("PASSWORD", true)->value();
      }
      if(request->hasParam("MQTT_SERVER", true)){
        Configuration::config.mqttServer = request->getParam("MQTT_SERVER", true)->value();
      }
      if(request->hasParam("MQTT_PORT", true)){
        Configuration::config.mqttPort = request->getParam("MQTT_PORT", true)->value();
      }
      if(request->hasParam("MQTT_TOPIC", true)){
        Configuration::config.mqttTopic = request->getParam("MQTT_TOPIC", true)->value();
      }
      if(request->hasParam("MQTT_TOPIC_COMMAND", true)){
        Configuration::config.mqttTopicCommand = request->getParam("MQTT_TOPIC_COMMAND", true)->value();
      }
      if(request->hasParam("NTP_SERVER", true)){
        Configuration::config.ntpServer = request->getParam("NTP_SERVER", true)->value();
      }
      if(request->hasParam("GMT", true)){
        Configuration::config.gmt = request->getParam("GMT", true)->value();
      }
      if(request->hasParam("WIFI_PASSWORD", true)){
        Configuration::config.wifiPassword = request->getParam("WIFI_PASSWORD", true)->value();
      }
      if(request->hasParam("SSID", true)){
        Configuration::config.ssid = request->getParam("SSID", true)->value();
      }
      // Save that the setup wizard has been run
      Configuration::config.setupDone = true;
      
      Configuration::save();
      Log.notice(F("Saved configuration." CR));

      // TODO:
      // Run WiFi begin with new SSID settings to verify that they are working before we reboot.
      // If not working then notify user that credentials or something is wrong.
      // If working, then set location below to optained IP from accesspoint.
      
      // Redirect client back to startpage
      auto *response = request->beginResponse(303);
      response->addHeader("Location", "/");
      request->send(response);

      sleep(50);
      ESP.restart();
    });

    // Handle Post-back form firmware update.
    web_server.on("/updatefirmware", HTTP_POST, [](AsyncWebServerRequest *request) {
      
      // Redirect client back to startpage
      auto *response = request->beginResponse(303, "text/plain", Update.hasError() ? "FAILED" : "SUCCESS");
      response->addHeader("Location", "/");
      request->send(response);

      sleep(50);
      ESP.restart();
    }, [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if ((request->hasParam("apiKey") &&
        request->getParam("apiKey")->value() != Configuration::config.apiKey) && 
      !request->authenticate(Configuration::config.username.c_str(), Configuration::config.password.c_str()))
      {
        return request->requestAuthentication();
      }
      
      String firmwareUpdateType = "SPIFFS"; // TODO: get parameter from request.

      if (!index) { // if index == 0 then this is the first frame of data
        Log.notice(F("Updating firmware from file: %s of type %s" CR), filename.c_str(), firmwareUpdateType);
        publish_mqtt("START UPDATING FIRMWARE");
        
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, firmwareUpdateType == "SPIFFS" ? U_SPIFFS : U_FLASH)) {  // TODO: support update of SPIFFS using "U_SPIFFS"
          Update.printError(Serial);
        }
      }

      //Write chunked data to the free sketch space
      if (Update.write(data, len) != len) {
        Update.printError(Serial);
      }
      
      if (final) { // if the final flag is set then this is the last frame of data
        if (Update.end(true)) { //true to set the size to the current progress
          Log.notice(F(CR "Firmware update complete, wrote %i bytes. Rebooting..." CR), index + len);
          publish_mqtt("DONE UPDATING FIRMWARE");
        } else {
          Update.printError(Serial);
        }
      }
    });

    // When no SSID set, redirect first page to setup page.
    if (Configuration::config.ssid.length() == 0) {
      web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/setup");
      });
    }

    web_server.onNotFound([](AsyncWebServerRequest *request){
      if (request->method() == HTTP_OPTIONS) {
		    request->send(200); // CORS-support
	    } else if (Configuration::config.ssid.length() == 0) {
        request->redirect("/setup");
      } else {
		    request->send(404);
	    }
    });

    // mount SPIFFS filesystem, the files stored in the "data"-directory in the root of this project. Contains the web application files.
    if (!SPIFFS.begin()) {
      Log.error(F("Failed to mount SPIFFS filesystem." CR));
      // If we have WiFi settings but web interface is not available, notify user so that they don't think anything is broken.
      if (Configuration::config.ssid.length() > 0) {
        web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
          request->send_P(200, "text/plain", NO_WEB_UI);
        }).setFilter(ON_STA_FILTER);
      }
      return;
    }

    if (Configuration::config.ssid.length() > 0) {
      web_server
      .serveStatic("/", SPIFFS, "/")
      .setDefaultFile("index.html")
      .setCacheControl("max-age=2592000") // 30 days.
      .setFilter(ON_STA_FILTER)
      .setAuthentication(Configuration::config.username.c_str(), Configuration::config.password.c_str());
    }

    // Start web server.
    web_server.begin();
    Log.notice(F("Web server initialized" CR));
}

bool WiFi_Client::isMQTT_enabled() {
  return Configuration::config.mqttServer.length() > 0 && Configuration::config.mqttTopic.length() > 0;
}

// Call upon this method to connect/reconnect WiFi
void WiFi_Client::connect() {
  Log.trace(F("Connecting to WiFi..." CR));

  if (WiFi.getMode() == WIFI_MODE_STA || Configuration::config.ssid.length() > 0) {
    WiFi.begin(Configuration::config.ssid.c_str(), Configuration::config.wifiPassword.c_str());
    dnsServer.stop(); // just in case it's running.
  } else {
    // If no SSID is defined, then start up with captive portal and default password "liam".
    WiFi.begin(Definitions::APP_NAME, "liam");
    dnsServer.stop(); // just in case it's running.
    dnsServer.start(53, "*", WiFi.softAPIP());  // capture all requests
  }
}

// Method is called upon when have a WiFi connection with an IP address (note that this method could be called upon several times).
void WiFi_Client::onWifiConnect(WiFiEvent_t event, system_event_info_t info) {

  WiFi.macAddress(mac);
  Log.notice(F("Connected to WiFi accesspoint \"%s\", using IP-address: %s and MAC: %x:%x:%x:%x:%x:%x" CR), WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  //close own AP network
  WiFi.mode(WIFI_MODE_STA);

  // Get time from NTP server.
  configTime(Configuration::config.gmt.toInt() * 3600, 0, Configuration::config.ntpServer.c_str()); // second parameter is daylight offset (3600 = summertime)
  Log.notice("Time: %s" CR, getTime().c_str());

  if (isMQTT_enabled()) {
    connectToMqtt();
  }
}

// Method is called upon when WiFi connection is lost, it will try to reconnect.
void WiFi_Client::onWifiDisconnect(WiFiEvent_t event, system_event_info_t info) {
  Log.notice(F("Disconnected from WiFi." CR));

  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to WiFi

  WiFi.disconnect(true);  // force disconnect WiFi to get new settings.

  // TODO: change "once" to "attach", in case we fail to reconnect then "onWifiDisconnect" will not be called upon again and no more attempts will be made!
  wifiReconnectTimer.once<WiFi_Client*>(10, [](WiFi_Client* instance) {
    instance->connect();
  }, this);
}

void WiFi_Client::connectToMqtt() {
  Log.trace(F("Connecting to MQTT broker..." CR));
  mqttClient.connect();
}

void WiFi_Client::onMqttConnect(bool sessionPresent) {
  mqttClient.publish(Configuration::config.mqttTopic.c_str(), 1, true, "CONNECTED");
  Log.notice(F("Connected to the MQTT broker." CR));

  // Flush any messages waiting in queue now that we are connected to broker.
  flushQueueTimer.once_ms<WiFi_Client*>(1, [](WiFi_Client* instance) {
    instance->flushMqttQueue();
  }, this);

  if (Configuration::config.mqttTopicCommand.length() > 0) {
    mqttClient.subscribe(Configuration::config.mqttTopicCommand.c_str(), 1);
  }
}

void WiFi_Client::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Log.notice(F("Disconnected from the MQTT broker! reason: %d" CR), static_cast<uint8_t>(reason));

  if (WiFi.isConnected()) {
    // wait two seconds then try to reconnect.
    // TODO: change "once" to "attach", in case we fail to reconnect then "onMqttDisconnect" will not be called upon again and no more attempts will be made!
    mqttReconnectTimer.once<WiFi_Client*>(2, [](WiFi_Client* instance) {
      instance->connectToMqtt();
    }, this);
  }
}

void WiFi_Client::flushMqttQueue() {
  while(mqttClient.connected() && !msgQueue.empty()) {
    MQTT_Message message = msgQueue.front();  // oldest message in queue.

    uint16_t packetIdPub1 = mqttClient.publish(message.topic.c_str(), 1, true, message.message.c_str());
    // if packet id is greater than 0 then we have successfully sent package.
    if (packetIdPub1 > 0) {
      Log.trace(F("Publish MQTT-message: id=%i, topic=\"%s\", message=\"%s\"" CR), packetIdPub1, message.topic.c_str(), message.message.c_str());
      msgQueue.pop();
    }
  }
}

void WiFi_Client::publish_mqtt(std::string message, std::string subtopic) {
  if (isMQTT_enabled()) {
    std::string topic = Configuration::config.mqttTopic.c_str();

    if (subtopic.length() > 0) {
      // make sure subtopics always begins with an slash ("/")
      if (subtopic.find("/") > 0) {
        subtopic = "/" + subtopic;
      }

      topic = topic + subtopic;
    }

    // removes the oldest message in queue if we reach the maximum allowed size of the message queue. This is to prevent us from leaking memory in case we fail to connect to a MQTT-broker.
    if (msgQueue.size() >= Definitions::MQTT_QUEUE_LENGTH) {
      msgQueue.pop();
    }

    msgQueue.push({ message, topic });
    flushMqttQueue();
  }
}

void WiFi_Client::onMqttPublish(uint16_t packetId) {
  Log.trace(F("MQTT Publish acknowledged, packet id: %d" CR), packetId);
}

AsyncWebServer& WiFi_Client::getWebServer() {
  return web_server;
}

/**
 * Method for sending information over WebSocket.
 * @param msgType type of payload being sent
 * @param json message to be sent
 * @param client [optional] client to send to, if none specified then we broadcast to everybody
 */
void WiFi_Client::sendDataWebSocket(String msgType, JsonObject& json, AsyncWebSocketClient* client) {
  // only push if there are connected clients.
  if (ws.count() > 0) {
    DynamicJsonBuffer jsonBuffer(400);
    JsonObject& root = jsonBuffer.createObject();
    root["type"] = msgType;
    root["payload"] = json;

    auto len = root.measureLength();
    AsyncWebSocketMessageBuffer* buffer = ws.makeBuffer(len); // creates a buffer (len + 1) for you.

    if (buffer) {
        root.printTo((char*)buffer->get(), len + 1);
        //TODO: would like an Log.isTrace() here!
        String jsonStr;
        root.printTo(jsonStr);
        Log.trace("WS pushed: %s" CR, jsonStr.c_str());

        if (client) {
          client->text(buffer);
        } else {
          ws.textAll(buffer);
        }
    }
  }
}

AsyncWebSocket& WiFi_Client::getWebSocketServer() {
  return ws;
}

void WiFi_Client::process() {
  // when running own accesspoint we want to capture all request and send them to the setup wizard-page.
  if (WiFi.getMode() == WIFI_MODE_APSTA) {
    // TODO:
   // dnsServer.processNextRequest();
  }
}

void WiFi_Client::registerMqttMessageCallback(const cb_mqttMessage &cb) {
  // add callback to end of callback list
  onMqttMessageCallbacks.push_back(cb);
}
