#include "wifi.h"
#include <WiFi.h>
#include <string>
#include <time.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include "definitions.h"
#include "configuration.h"
#include "settings.h"
#include <cassert>

static const char* MQTT_TOPIC = "home/liam-esp";
static const char* DEFAULT_NTP = "pool.ntp.org";
// TODO: AVOID "%" in CSS, https://github.com/me-no-dev/ESPAsyncWebServer/pull/366
static const char SETUP_HTML[] PROGMEM = "<html><head><meta name=\"viewport\" content=\"width=device-width\"><title>Liam-ESP</title><style>fieldset{padding:1em;font:0.8em/1 sans-serif;margin:1em}legend{padding:0.2em 0.5em;border:1px solid black;font-size:0.9em}label{float:left;margin-right:0.5em;padding-top:0.2em;text-align:right;font-weight:bold;width:6em}input{margin-bottom:0.4em;padding-left: 0.2em;}.center{display:block;margin-right:auto;margin-left:auto;text-align:center}a:link{text-decoration:none;}</style></head><body><h1 class=\"center\">Setup</h1><form action=\"/setup\" method=\"post\"><fieldset><legend>WiFi</legend><label>SSID: </label><input type=\"text\" name=\"SSID\" length=32 value=\"%SSID%\"><br><label>Password: </label><input type=\"password\" name=\"WIFI_PASSWORD\" length=64 value=\"%WIFI_PASSWORD%\"></fieldset><fieldset><legend>Administrator</legend><label>Username: </label><input type=\"text\" name=\"USERNAME\" length=20 value=\"%USERNAME%\"><br><label>Password: </label><input type=\"password\" name=\"PASSWORD\" length=20 value=\"%PASSWORD%\"></fieldset><fieldset><legend>Time</legend><label>NTP-server: </label><input type=\"text\" name=\"NTP_SERVER\" length=40 value=\"%NTP_SERVER%\"><br><label>Time zone: </label><input type=\"number\" name=\"GMT\"  min=\"-12\" max=\"12\" value=\"%GMT%\">&nbsp;<a href=\"https://upload.wikimedia.org/wikipedia/commons/e/e8/Standard_World_Time_Zones.png\" target=\"_blank\">&nbsp;i&nbsp;</a></fieldset><fieldset><legend>MQTT</legend><label>Server IP: </label><input type=\"text\" name=\"MQTT_SERVER\" placeholder=\"leave blank if not used\" length=64 value=\"%MQTT_SERVER%\"><br><label>Port: </label><input type=\"number\" name=\"MQTT_PORT\" placeholder=\"leave blank if not used\" min=\"1024\" max=\"65535\" value=\"%MQTT_PORT%\"><br> <label>Topic: </label> <input type=\"text\" name=\"MQTT_TOPIC\" placeholder=\"leave blank if not used\" length=200 value=\"%MQTT_TOPIC%\"></fieldset><input type=\"submit\" value=\"Save\" class=\"center\"></form></body></html>";
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
    return Configuration::getString("USERNAME", "admin");
  } else if (placeholder == "PASSWORD") {
    return Configuration::getString("PASSWORD");
  } else if (placeholder == "MQTT_SERVER") {
    return Configuration::getString("MQTT_SERVER");
  } else if (placeholder == "MQTT_PORT") {
    return Configuration::getString("MQTT_PORT", "1883");
  } else if (placeholder == "MQTT_TOPIC") {
    return Configuration::getString("MQTT_TOPIC", MQTT_TOPIC);
  } else if (placeholder == "NTP_SERVER") {
    return Configuration::getString("NTP_SERVER", DEFAULT_NTP);
  } else if (placeholder == "GMT") {
    return Configuration::getString("GMT", "0");
  } else if (placeholder == "WIFI_PASSWORD") {
    return Configuration::getString("WIFI_PASSWORD");
  } else if (placeholder == "SSID") {
    return Configuration::getString("SSID");
  } else {
    return String();
  }
}

void printLocalTime() {
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println(&timeinfo, "Time: %A, %d %B %Y, %H:%M:%S");
}

// WiFi status eventhandler, called upon when WiFi connection change status.
void WiFi_Client::WiFiEvent(WiFiEvent_t event, system_event_info_t info) {
    switch(event) {
      case SYSTEM_EVENT_AP_START: {
        // When we run as an accesspoint
        WiFi.softAPsetHostname(Definitions::APP_NAME);
        WiFi.softAPenableIpV6();
        break;
      case SYSTEM_EVENT_STA_START:
        // When we run in Station-mode (are connected to an accesspoint as client)
        WiFi.setHostname(Definitions::APP_NAME);
        break;
      case SYSTEM_EVENT_STA_CONNECTED:
        WiFi.enableIpV6();
        break;
      case SYSTEM_EVENT_AP_STA_GOT_IP6:
        //both interfaces get the same event
        Serial.print("STA IPv6: ");
        Serial.println(WiFi.localIPv6());
        Serial.print("AP IPv6: ");
        Serial.println(WiFi.softAPIPv6());
        break;
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
  Serial.println("AP Started");
  Serial.print("AP SSID: ");
  Serial.println(Definitions::APP_NAME);
  Serial.print("AP IPv4: ");
  Serial.println(WiFi.softAPIP());

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

    uint8_t ip[4];
    sscanf(Configuration::getString("MQTT_SERVER").c_str(), "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);
    auto mqtt_ip = IPAddress(ip[0], ip[1], ip[2], ip[3]);
    auto mqtt_port = Configuration::getInt("MQTT_PORT", 1883);
    
    Serial.print("using MQTT server IP: ");
    Serial.print(mqtt_ip.toString());
    Serial.print(", and port: ");
    Serial.println(mqtt_port);

    mqttClient.setServer(mqtt_ip, mqtt_port);
    //mqttClient.setCredentials("MQTT_USERNAME", "MQTT_PASSWORD");
    mqttClient.setKeepAlive(15); // seconds
    mqttClient.setClientId(Definitions::APP_NAME);
    mqttClient.setWill(Configuration::getString("MQTT_TOPIC", MQTT_TOPIC).c_str(), 2, true, "DISCONNECTED");
  }
}

void WiFi_Client::setupWebServer() {
    // Add CORS support, to fix error on iOS. https://github.com/me-no-dev/ESPAsyncWebServer/blob/d7399a7664bce76e9939837fd6cf51bd272ef588/README.md#adding-default-headers
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    // remove all previous rewrites, handlers and onNotFound/onFileUpload/onRequestBody callbacks (unsure if web_server.begin() does that for us)
    web_server.reset();

    web_server.addHandler(&ws);

    web_server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", SETUP_HTML, renderPlaceholder);
    });
    // Handle Post-back form from setup page.
    web_server.on("/setup", HTTP_POST, [](AsyncWebServerRequest *request) {
      if(request->hasParam("USERNAME", true)){
        Configuration::set("USERNAME", request->getParam("USERNAME", true)->value());
      }
      if(request->hasParam("PASSWORD", true)){
        Configuration::set("PASSWORD", request->getParam("PASSWORD", true)->value());
      }
      if(request->hasParam("MQTT_SERVER", true)){
        Configuration::set("MQTT_SERVER", request->getParam("MQTT_SERVER", true)->value());
      }
      if(request->hasParam("MQTT_PORT", true)){
        Configuration::set("MQTT_PORT", request->getParam("MQTT_PORT", true)->value());
      }
      if(request->hasParam("MQTT_TOPIC", true)){
        Configuration::set("MQTT_TOPIC", request->getParam("MQTT_TOPIC", true)->value());
      }
      if(request->hasParam("NTP_SERVER", true)){
        Configuration::set("NTP_SERVER", request->getParam("NTP_SERVER", true)->value());
      }
      if(request->hasParam("GMT", true)){
        Configuration::set("GMT", request->getParam("GMT", true)->value());
      }
      if(request->hasParam("WIFI_PASSWORD", true)){
        Configuration::set("WIFI_PASSWORD", request->getParam("WIFI_PASSWORD", true)->value());
      }
      if(request->hasParam("SSID", true)){
        Configuration::set("SSID", request->getParam("SSID", true)->value());
      }

      Serial.println("Saved configuration.");
      request->redirect("/");
      sleep(100);
      ESP.restart();
    });

    // When no SSID set, redirect first page to setup page.
    if (Configuration::getString("SSID", "").length() == 0) {
      web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/setup");
      });
    }

    web_server.onNotFound([](AsyncWebServerRequest *request){
      if (request->method() == HTTP_OPTIONS) {
		    request->send(200); // CORS-support
	    } else {
		    request->send(404);
	    }
    });

    // mount SPIFFS filesystem, the files stored in the "data"-directory in the root of this project. Contains the web application files.
    if (!SPIFFS.begin()) {
      Serial.println("Failed to mount SPIFFS filesystem.");
      // If we have WiFi settings but web interface is not available, notify user so that they don't think anything is broken.
      if (Configuration::getString("SSID", "").length() > 0) {
        web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
          request->send_P(200, "text/plain", NO_WEB_UI);
        }).setFilter(ON_STA_FILTER);
      }
      return;
    }

    if (Configuration::getString("SSID", "").length() > 0) {
      web_server
      .serveStatic("/", SPIFFS, "/")
      .setDefaultFile("index.html")
      .setCacheControl("max-age=2592000") // 30 days.*/
      .setFilter(ON_STA_FILTER)
      .setAuthentication(Configuration::getString("USERNAME").c_str(), Configuration::getString("PASSWORD").c_str());
    }

    // Start web server.
    web_server.begin();
    Serial.println("Web server initialized");
}

bool WiFi_Client::isMQTT_enabled() {
  if (Configuration::getString("MQTT_SERVER", "").length() == 0 || Configuration::getString("MQTT_TOPIC", "").length() == 0) {
    return false;
  } else {
    return true;
  }
}

// Call upon this method to connect/reconnect WiFi
void WiFi_Client::connect() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(Configuration::getString("SSID").c_str(), Configuration::getString("WIFI_PASSWORD").c_str());
}

// Method is called upon when have a WiFi connection with an IP address (note that this method could be called upon several times).
void WiFi_Client::onWifiConnect(WiFiEvent_t event, system_event_info_t info) {

  Serial.print("Connected to WiFi accesspoint: ");
  Serial.print(WiFi.SSID());
  Serial.print(", using IP-address: ");
  Serial.println(WiFi.localIP());

  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[0], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.println(mac[5], HEX);
  
  //close own AP network
  WiFi.mode(WIFI_MODE_STA);

  // Annonce us on WiFi network using Multicast DNS.
  MDNS.begin(Definitions::APP_NAME);
  MDNS.addService("http","tcp",80);
  // Get time from NTP server.
  configTime(atol(Configuration::getString("GMT", "0").c_str()) * 3600, 3600, Configuration::getString("NTP_SERVER", DEFAULT_NTP).c_str()); // sencond parameter is daylight offset (3600 = summertime)
  printLocalTime();

  if (isMQTT_enabled()) {
    connectToMqtt();
  }
}

// Method is called upon when WiFi connection is lost, it will try to reconnect.
void WiFi_Client::onWifiDisconnect(WiFiEvent_t event, system_event_info_t info) {
  Serial.println("Disconnected from WiFi.");

  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to WiFi

  WiFi.disconnect(true);  // force disconnect WiFi to get new settings.

  // TODO: change "once" to "attach", in case we fail to reconnect then "onWifiDisconnect" wount be called upon again and no more attempts will be made!
  wifiReconnectTimer.once<WiFi_Client*>(10, [](WiFi_Client* instance) {
    instance->connect();
  }, this);
}

void WiFi_Client::connectToMqtt() {
  Serial.println("Connecting to MQTT broker...");
  mqttClient.connect();
}

void WiFi_Client::onMqttConnect(bool sessionPresent) {
  mqttClient.publish(Configuration::getString("MQTT_TOPIC", MQTT_TOPIC).c_str(), 1, true, "CONNECTED");
  Serial.println("Connected to the MQTT broker.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  // Flush any messages waiting in queue now that we are connected to broker.
  flushQueueTimer.once_ms<WiFi_Client*>(1, [](WiFi_Client* instance) {
    instance->flushQueue();
  }, this);
}

void WiFi_Client::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print("Disconnected from the MQTT broker! reason: ");
  Serial.println(static_cast<uint8_t>(reason));

  if (WiFi.isConnected()) {
    // wait two seconds then try to reconnect.
    // TODO: change "once" to "attach", in case we fail to reconnect then "onMqttDisconnect" wount be called upon again and no more attempts will be made!
    mqttReconnectTimer.once<WiFi_Client*>(2, [](WiFi_Client* instance) {
      instance->connectToMqtt();
    }, this);
  }
}

void WiFi_Client::flushQueue() {
  while(mqttClient.connected() && !msgQueue.empty()) {
    MQTT_Message message = msgQueue.front();  // oldest message in queue.

    uint16_t packetIdPub1 = mqttClient.publish(message.topic.c_str(), 1, true, message.message.c_str());
    // if packet id is greater than 0 then we have successfully sent package.
    if (packetIdPub1 > 0) {
     /* Serial.print("Publish MQTT-message: id="); Serial.print(packetIdPub1);
      Serial.print(", topic=\""); Serial.print(message.topic.c_str());
      Serial.print("\", message=\""); Serial.print(message.message.c_str());
      Serial.println("\"");*/

      msgQueue.pop();
    }
  }
}

void WiFi_Client::publish_message(std::string message, std::string subtopic) {
  if (isMQTT_enabled()) {
    std::string topic = Configuration::getString("MQTT_TOPIC").c_str();

    if (subtopic.length() > 0) {
      // make sure subtopics always begins with an slash ("/")
      if (subtopic.find("/") > 0) {
        subtopic = "/" + subtopic;
      }

      topic = topic + subtopic;
    }

    // removes the oldest message in queue if we reach the maximum allowed size of the message queue. This is to prevent us from leaking memory in case we fail to connect to a MQTT-broker.
    if (msgQueue.size() >= Settings::MQTT_QUEUE_LENGTH) {
      msgQueue.pop();
    }

    msgQueue.push({ message, topic });
    flushQueue();
  }
}

void WiFi_Client::onMqttPublish(uint16_t packetId) {
  Serial.print("MQTT Publish acknowledged, packet id: ");
  Serial.println(packetId);
}

AsyncWebServer& WiFi_Client::getWebServer() {
  return web_server;
}

AsyncWebSocket& WiFi_Client::getWebSocketServer() {
  return ws;
}
