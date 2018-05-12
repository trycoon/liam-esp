
#ifndef _liam_wifi_h
#define _liam_wifi_h

#include <string>
#include <vector>  // NOTE: needed to fix the following error: "Arduino.h:253:18: error: expected unqualified-id before '(' token".
#include <queue>
#include <ESP32Ticker.h>
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


struct MQTT_Message {
  std::string message;
  std::string topic;
};

class WiFi_Client {
  public:
    WiFi_Client();
    void connect();
    void publish_message(std::string message, std::string subtopic = "");
    AsyncWebServer& getWebServer();  // code-smell, we should think of a better way than to expose this inner reference when we need to register routes!

  private:
    static WiFi_Client* Instance;
    Ticker wifiReconnectTimer;

    AsyncMqttClient mqttClient;
    Ticker mqttReconnectTimer;
    Ticker flushQueueTimer;
    std::queue<MQTT_Message> msgQueue;

    AsyncWebServer web_server;
    static void WiFiEvent(system_event_id_t event, system_event_info_t info);
    bool isMQTT_enabled();
    void flushQueue();
    void onWifiConnect(system_event_id_t event, system_event_info_t info);
    void onWifiDisconnect(system_event_id_t event, system_event_info_t info);
    void connectToMqtt();
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttPublish(uint16_t packetId);
    void checkWifiSettings();
    void setupWebServer();
};

#endif
