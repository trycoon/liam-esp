
#ifndef _liam_wifi_h
#define _liam_wifi_h

#include <string>
#include <queue>
#include <vector>
#include <Ticker.h>
#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <functional>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "configuration.h"
#include "processable.h"

struct scannedNetwork {
  String ssid;
  int32_t channel;
  int32_t rssi;
  bool isSecure;
};

class Wlan : public Processable {
  public:
    Wlan();
    void start();
    AsyncWebServer& getWebServer();  // code-smell, we should think of a better way than to expose this inner reference when we need to register routes!
    AsyncWebSocket& getWebSocketServer();  // code-smell, we should think of a better way than to expose this inner reference when we need to register routes!
    size_t getConnectedWebsocketClientsCount();
    bool isAuthenticated(AsyncWebServerRequest *request);
    bool isAuthenticatedSession(AsyncWebServerRequest *request);
    String authenticateSession(String username, String password);
    void removeAuthenticatedSession(AsyncWebServerRequest *request);
    void sendDataWebSocket(String msgType, JsonObject& json, AsyncWebSocketClient* client = nullptr);
    
    /* Internal use only! */
    void process();
    
  private:
    static Wlan* Instance;
    static String renderPlaceholder(const String& placeholder);
    Ticker wifiReconnectTimer;

    Ticker flushQueueTimer;
    byte mac[6];
    std::vector<scannedNetwork> availableSSID;

    AsyncWebServer web_server;
    AsyncWebSocket ws;
    DNSServer dnsServer;
    std::vector<String> authenticatedSessions;
    bool capturePortalRunning = false;
    bool wifiStartedOnce = false;   // set to true if WiFi.begin() has been run atleast once.

    static void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
    void connect();
    void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info);
    void onWifiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info);
    String parseSessionFromRequest(AsyncWebServerRequest *request);
    void loadAuthenticatedSessions();
    void saveAuthenticatedSessions();
    void scanForSSID();
    void setupOTA();
    void setupWebServer();
};

#endif