
#ifndef mqtt_h
#define mqtt_h

#include <Arduino.h>
#include <AsyncMqttClient.h>

class MQTT_Client {
  public:
    MQTT_Client();
    void connect();
    void publish_message(char* msg);
  private:
    AsyncMqttClient mqttClient;
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttPublish(uint16_t packetId);
};

#endif
