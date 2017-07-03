#include <string>
#include "mqtt.h"
#include "settings.h"

MQTT_Client::MQTT_Client() {
  mqttClient.onConnect([this](bool sessionPresent) { onMqttConnect(sessionPresent); });
  mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) { onMqttDisconnect(reason); });
  mqttClient.onPublish([this](uint16_t packetId) { onMqttPublish(packetId); });
  mqttClient.setServer(Settings::MQTT_SERVER, Settings::MQTT_PORT);
  //mqttClient.setCredentials("MQTT_USERNAME", "MQTT_PASSWORD");
  mqttClient.setKeepAlive(15); // seconds
  mqttClient.setClientId(Definitions::APP_NAME);
  mqttClient.setWill(Settings::MQTT_TOPIC.c_str(), 2, true, "DISCONNECTED");
}

void MQTT_Client::connect() {
  Serial.println("Connecting to MQTT broker...");
  mqttClient.connect();
}

void MQTT_Client::onMqttConnect(bool sessionPresent) {
  mqttClient.publish(Settings::MQTT_TOPIC.c_str(), 1, true, "CONNECTED");
  Serial.println("Connected to the MQTT broker.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void MQTT_Client::publish_message(std::string msg, std::string subtopic) {
  if (mqttClient.connected()) {

    std::string topic = Settings::MQTT_TOPIC;

    if (subtopic.length() > 0) {
      // make sure subtopics always begins with an slash ("/")
      if (subtopic.find("/") > 0) {
        subtopic = "/" + subtopic;
      }

      topic = topic + subtopic;
    }

    uint16_t packetIdPub1 = mqttClient.publish(topic.c_str(), 1, true, msg.c_str());
    Serial.println(packetIdPub1);
  }
}

void MQTT_Client::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print("Disconnected from the MQTT broker! reason: ");
  Serial.println(static_cast<uint8_t>(reason));
  Serial.println("Reconnecting to MQTT...");
  mqttClient.connect();
}

void MQTT_Client::onMqttPublish(uint16_t packetId) {
  Serial.println("MQTT Publish acknowledged");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
