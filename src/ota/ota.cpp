#include <vector> // NOTE: needed to fix the following error: "Arduino.h:253:18: error: expected unqualified-id before '(' token".
#include <ArduinoOTA.h>
#include "definitions.h"
#include "settings.h"
#include "ota.h"

// TODO: Replace with webbased OTA? https://github.com/esp8266/Arduino/tree/master/doc/ota_updates

OTA::OTA(WiFi_Client& wifiClient) : wifi(wifiClient) {

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  ArduinoOTA.setHostname(Definitions::APP_NAME);

  // authentication string
  ArduinoOTA.setPassword(Settings::OTA_PASSWORD);

  ArduinoOTA.onStart([this]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
    wifi.publish_message("START UPDATING FIRMWARE");
  });

  ArduinoOTA.onEnd([this]() {
    Serial.println("\nEnd");
    wifi.publish_message("DONE UPDATING FIRMWARE");
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
}

void OTA::start() {
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void OTA::handle() {
  ArduinoOTA.handle();
}
