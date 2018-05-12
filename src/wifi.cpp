#include "wifi.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include "definitions.h"
#include "settings.h"

#include <cassert>

WiFi_Client* WiFi_Client::Instance = nullptr;

void WiFi_Client::WiFiEvent(system_event_id_t event, system_event_info_t info)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Instance->onWifiConnect(event, info);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        break;
    }
}

//TODO: Setup time when we get IP: https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Time/SimpleTime/SimpleTime.ino
// TODO: Smart Config: http://www.iotsharing.com/2017/05/how-to-use-smartconfig-on-esp32.html
WiFi_Client::WiFi_Client() : web_server(80) {
  assert(!Instance);
  Instance = this;
  WiFi.setHostname(Definitions::APP_NAME);
  WiFi.mode(WIFI_STA);

  WiFi.onEvent(WiFiEvent);
  /*WiFi.onEvent([this](WiFiEventCb event) {
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
        //onWifiConnect();
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        //onWifiDisconnect();
        break;
    }
  });*/

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

    mqttClient.setServer(Settings::MQTT_SERVER, Settings::MQTT_PORT);
    //mqttClient.setCredentials("MQTT_USERNAME", "MQTT_PASSWORD");
    mqttClient.setKeepAlive(15); // seconds
    mqttClient.setClientId(Definitions::APP_NAME);
    mqttClient.setWill(Settings::MQTT_TOPIC.c_str(), 2, true, "DISCONNECTED");
  }
}

void WiFi_Client::setupWebServer() {

    // mount SPIFFS filesystem, the files stored in the "data"-directory in the root of this project. Contains the static webpage files.
/*    if (!SPIFFS.begin()) {
      Serial.println("Failed to mount SPIFFS filesystem! Rebooting.");
      delay(2000);
      ESP.restart();
    }

    web_server
    .serveStatic("/", SPIFFS, "/")
    .setDefaultFile("index.html")
    .setCacheControl("max-age=2592000"); // 30 days.*/

    Serial.println("web_server initialized");
}

bool WiFi_Client::isMQTT_enabled() {
  if (strcmp(Settings::MQTT_SERVER, "") || strcmp(Settings::MQTT_SERVER, "<your MQTT servers IP-address>")) {
    return false;
  } else {
    return true;
  }
}

void WiFi_Client::connect() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(Settings::SSID, Settings::WIFI_PASSWORD);
}

void WiFi_Client::onWifiConnect(system_event_id_t event, system_event_info_t info) {
  Serial.print("Connected to Wi-Fi using IP-address: ");
  Serial.print(WiFi.localIP()); //String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3])
  Serial.print(", MAC-address: ");
  uint8_t MAC_array[6];
  char MAC_char[18];
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){
    sprintf(MAC_char,"%s%02x:", MAC_char, MAC_array[i]);
  }
  Serial.println(MAC_char);

  // Annonce us on Wi-Fi network using mDNS.
  MDNS.begin(Definitions::APP_NAME);
  MDNS.addService("http","tcp",80);
  // Start web server.
  web_server.begin();

  if (isMQTT_enabled()) {
    connectToMqtt();
  }
}

void WiFi_Client::onWifiDisconnect(system_event_id_t event, system_event_info_t info) {
  Serial.println("Disconnected from Wi-Fi.");

  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi

  wifiReconnectTimer.once<WiFi_Client*>(2, [](WiFi_Client* instance) {
    instance->connect();
  }, this);
}

void WiFi_Client::connectToMqtt() {
  Serial.println("Connecting to MQTT broker...");
  mqttClient.connect();
}

void WiFi_Client::onMqttConnect(bool sessionPresent) {
  mqttClient.publish(Settings::MQTT_TOPIC.c_str(), 1, true, "CONNECTED");
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
    mqttReconnectTimer.once<WiFi_Client*>(2, [](WiFi_Client* instance) {
      instance->connectToMqtt();
    }, this);
  }
}

void WiFi_Client::flushQueue() {
  if (mqttClient.connected()) {
    while(!msgQueue.empty()) {
      MQTT_Message message = msgQueue.front();  // oldest message in queue.

      uint16_t packetIdPub1 = mqttClient.publish(message.topic.c_str(), 1, true, message.message.c_str());
      // if packet id is greater than 0 then we have successfully sent package.
      if (packetIdPub1 > 0) {
        Serial.print("Publish MQTT-message: id="); Serial.print(packetIdPub1);
        Serial.print(", topic=\""); Serial.print(message.topic.c_str());
        Serial.print("\", message=\""); Serial.print(message.message.c_str());
        Serial.println("\"");

        msgQueue.pop();
      }
    }
  }
}

void WiFi_Client::publish_message(std::string message, std::string subtopic) {
  if (isMQTT_enabled()) {
    std::string topic = Settings::MQTT_TOPIC;

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

/**
* Check if we have necessary WiFi settings, if not then enter "ESP SmartConfig"-mode to bind to an existing accesspoint.
*/
void WiFi_Client::checkWifiSettings() {
  if (sizeof(Settings::SSID) == 0) {
    Serial.println("Missing WiFi settings, starting up own accesspoint for SmartConfig.");
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    //Wait for SmartConfig packet from mobile
    Serial.println("Waiting for SmartConfig.");
    while (!WiFi.smartConfigDone()) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("SmartConfig received.");

    //Wait for WiFi to connect to AP
    Serial.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi Connected.");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // TODO: save settings to persistant store.
    Serial.println("Restarting software.");
    delay(2000);
    ESP.restart();
  }
}

AsyncWebServer& WiFi_Client::getWebServer() {
  return web_server;
}
