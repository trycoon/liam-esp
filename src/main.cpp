#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "settings.h"
#include "mqtt/mqtt.h"

/*
 * Application to control a LIAM robot mower using a NodeMCU/ESP-12E (or similar ESP8266) microcontroller.
 */
void setup_WiFi();
void setup_OTA();
MQTT_Client mqtt;

void setup() {
  Serial.begin(115200);
  Serial.println(APP_NAME);

  setup_WiFi();
  setup_OTA();
  mqtt = new MQTT_Client();
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
    mqtt.publish_message("START UPDATING FIRMWARE");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    mqtt.publish_message("DONE UPDATING FIRMWARE");
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

void loop() {
  ArduinoOTA.handle();

}
