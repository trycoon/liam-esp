#include <string>
#include "mqtt.h"
#include "settings.h"

MQTT_Client::MQTT_Client() {
  mqttClient.onConnect([this](bool sessionPresent) { onMqttConnect(sessionPresent); });
  mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) { onMqttDisconnect(reason); });
  mqttClient.onPublish([this](uint16_t packetId) { onMqttPublish(packetId); });
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  //mqttClient.setCredentials("MQTT_USERNAME", "MQTT_PASSWORD");
  mqttClient.setKeepAlive(15); // seconds
  mqttClient.setClientId(APP_NAME);
  mqttClient.setWill(MQTT_TOPIC, 2, true, "DISCONNECTED");
}

void MQTT_Client::connect() {
  Serial.println("Connecting to MQTT broker...");
  mqttClient.connect();
}

void MQTT_Client::onMqttConnect(bool sessionPresent) {
  mqttClient.publish(MQTT_TOPIC, 1, true, "CONNECTED");
  Serial.println("Connected to the MQTT broker.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void MQTT_Client::publish_message(std::string msg) {
  if (mqttClient.connected()) {
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_TOPIC, 1, true, msg.c_str());
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
