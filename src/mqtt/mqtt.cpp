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
  // TODO: anropa denna med en timer eftersom vi inte skall köra tunga saker i callbacks.
  // https://github.com/marvinroger/async-mqtt-client/blob/master/examples/FullyFeatured/FullyFeatured.ino
  flushQueue();  // Flush any messages waiting in queue now that we are connected to broker.
}

void MQTT_Client::flushQueue() {
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

void MQTT_Client::publish_message(std::string message, std::string subtopic) {
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

void MQTT_Client::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print("Disconnected from the MQTT broker! reason: ");
  Serial.println(static_cast<uint8_t>(reason));
  mqttClient.connect();
}

void MQTT_Client::onMqttPublish(uint16_t packetId) {
  Serial.print("MQTT Publish acknowledged, packet id: ");
  Serial.println(packetId);
}
