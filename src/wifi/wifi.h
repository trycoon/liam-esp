
#ifndef _liam_wifi_h
#define _liam_wifi_h

#include <string>
#include <vector>  // NOTE: needed to fix the following error: "Arduino.h:253:18: error: expected unqualified-id before '(' token".
#include <queue>
#include <Ticker.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

struct MQTT_Message {
  std::string message;
  std::string topic;
};

class WiFi_Client {
  public:
    WiFi_Client();
    void connect();
    void publish_message(std::string message, std::string subtopic = "");

  private:
    WiFiEventHandler wifiConnectHandler;
    WiFiEventHandler wifiDisconnectHandler;
    Ticker wifiReconnectTimer;
    
    AsyncMqttClient mqttClient;
    Ticker mqttReconnectTimer;
    Ticker flushQueueTimer;
    std::queue<MQTT_Message> msgQueue;

    bool isMQTT_enabled();
    void flushQueue();
    void onWifiConnect(const WiFiEventStationModeGotIP& event);
    void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
    void connectToMqtt();
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttPublish(uint16_t packetId);
};

#endif
