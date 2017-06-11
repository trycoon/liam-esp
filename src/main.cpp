#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <AsyncMqttClient.h>
#include <settings.h>

/*
 * Application to control a LIAM robot mower using a NodeMCU/ESP-12E (or similar ESP8266) microcontroller.
 */

void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttPublish(uint16_t packetId);
void publish_message(char* msg);
void setup_WiFi();
void setup_OTA();
void setup_MQTT();

AsyncMqttClient mqttClient;

void setup() {
  Serial.begin(115200);
  Serial.println(APP_NAME);

  setup_WiFi();
  setup_OTA();
  setup_MQTT();
}

void setup_WiFi() {
  WiFi.hostname(APP_NAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

void setup_OTA() {
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  ArduinoOTA.setHostname(APP_NAME);

  // authentication string
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
    publish_message("START UPDATING FIRMWARE");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    publish_message("DONE UPDATING FIRMWARE");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_MQTT() {
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  //mqttClient.setCredentials("MQTT_USERNAME", "MQTT_PASSWORD");
  mqttClient.setKeepAlive(15); // seconds
  mqttClient.setClientId(APP_NAME);
  mqttClient.setWill(MQTT_TOPIC, 2, true, "DISCONNECTED");
  Serial.println("Connecting to MQTT broker...");
  mqttClient.connect();
}

void loop() {
  ArduinoOTA.handle();

}

void onMqttConnect(bool sessionPresent) {
  mqttClient.publish(MQTT_TOPIC, 1, true, "CONNECTED");
  Serial.println("Connected to the MQTT broker.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void publish_message(char* msg) {
  if (mqttClient.connected()) {
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_TOPIC, 1, true, msg);
    Serial.println(packetIdPub1);
  }
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print("Disconnected from the MQTT broker! reason: ");
  Serial.println(static_cast<uint8_t>(reason));
  Serial.println("Reconnecting to MQTT...");
  mqttClient.connect();
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("MQTT Publish acknowledged");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
