
#ifndef _liam_wifi_h
#define _liam_wifi_h

#include <string>
#include <queue>
#include <vector>
#include <Ticker.h>
#include <Arduino.h>
#include <FS.h>
#include <functional>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include "configuration.h"
#include "processable.h"

struct MQTT_Message {
  std::string message;
  std::string topic;
};

struct scannedNetwork {
  String ssid;
  int32_t channel;
  int32_t rssi;
  bool isSecure;
};

using cb_mqttMessage = std::function<void(char* topic, char* payload, size_t length)>;

class WiFi_Client : public Processable {
  public:
    WiFi_Client();
    void start();
    void publish_mqtt(std::string message, std::string subtopic = "");
    AsyncWebServer& getWebServer();  // code-smell, we should think of a better way than to expose this inner reference when we need to register routes!
    AsyncWebSocket& getWebSocketServer();  // code-smell, we should think of a better way than to expose this inner reference when we need to register routes!
    size_t getConnectedWebsocketClientsCount();
    bool isAuthenticated(AsyncWebServerRequest *request);
    bool isAuthenticatedSession(AsyncWebServerRequest *request);
    String authenticateSession(String username, String password);
    void removeAuthenticatedSession(AsyncWebServerRequest *request);
    void sendDataWebSocket(String msgType, JsonObject& json, AsyncWebSocketClient* client = nullptr);
    void registerMqttMessageCallback(const cb_mqttMessage &cb);
    
    /* Internal use only! */
    void process();
    
  private:
    static WiFi_Client* Instance;
    static String renderPlaceholder(const String& placeholder);
    Ticker wifiReconnectTimer;

    AsyncMqttClient mqttClient;
    Ticker mqttReconnectTimer;
    Ticker flushQueueTimer;
    std::queue<MQTT_Message> msgQueue;
    byte mac[6];
    std::vector<scannedNetwork> availableSSID;

    AsyncWebServer web_server;
    AsyncWebSocket ws;
    DNSServer dnsServer;
    std::vector<cb_mqttMessage> onMqttMessageCallbacks;
    std::vector<String> authenticatedSessions;
    bool capturePortalRunning = false;
    bool wifiStartedOnce = false;   // set to true if WiFi.begin() has been run atleast once.

    static void WiFiEvent(system_event_id_t event, system_event_info_t info);
    bool isMQTT_enabled();
    void flushMqttQueue();
    void connect();
    void onWifiConnect(system_event_id_t event, system_event_info_t info);
    void onWifiDisconnect(system_event_id_t event, system_event_info_t info);
    void connectToMqtt();
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttPublish(uint16_t packetId);
    String parseSessionFromRequest(AsyncWebServerRequest *request);
    void loadAuthenticatedSessions();
    void saveAuthenticatedSessions();
    void scanForSSID();
    void setupOTA();
    void setupMQTT();
    void setupWebServer();
};

#endif
