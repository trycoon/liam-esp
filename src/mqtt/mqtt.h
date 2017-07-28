
#ifndef _mqtt_h
#define _mqtt_h

#include <string>
#include <vector>  // NOTE: needed to fix the following error: "Arduino.h:253:18: error: expected unqualified-id before '(' token".
#include <queue>
#include <Arduino.h>
#include <AsyncMqttClient.h>

struct MQTT_Message {
  std::string message;
  std::string topic;
};

class MQTT_Client {
  public:
    MQTT_Client();
    void connect();
    void publish_message(std::string message, std::string subtopic = "");

  private:
    AsyncMqttClient mqttClient;
    std::queue<MQTT_Message> msgQueue;
    void flushQueue();
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttPublish(uint16_t packetId);
};

#endif
