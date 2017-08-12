#include "wifi.h"
#include "definitions.h"
#include "settings.h"
#include <ESP8266mDNS.h>


WiFi_Client::WiFi_Client() : web_server(80) {
  WiFi.hostname(Definitions::APP_NAME);
  WiFi.mode(WIFI_STA);

  wifiConnectHandler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP& event) {
    onWifiConnect(event);
  });

  wifiDisconnectHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected& event) {
    onWifiDisconnect(event);
  });

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
    if (!SPIFFS.begin()) {
      Serial.println("Failed to mount SPIFFS filesystem! Rebooting.");
      delay(2000);
      ESP.restart();
    }

    web_server
    .serveStatic("/", SPIFFS, "/")
    .setDefaultFile("index.html")
    .setCacheControl("max-age=2592000"); // 30 days.
    //.setAuthentication("user", "pass");
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

void WiFi_Client::onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.print("Connected to Wi-Fi using IP-address: "); Serial.println(event.ip);

  // Annonce us on Wi-Fi network using mDNS.
  MDNS.begin(Definitions::APP_NAME);
  MDNS.addService("http","tcp",80);
  // Start web server.
  web_server.begin();

  if (isMQTT_enabled()) {
    connectToMqtt();
  }
}

void WiFi_Client::onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");

  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi

  wifiReconnectTimer.once<WiFi_Client*>(2, [](WiFi_Client* instance) {
    instance->connect();
  }, this);
  //wifiReconnectTimer.once(2, []() { Serial.print("");});
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

AsyncWebServer& WiFi_Client::getWebServer() {
  return web_server;
}
