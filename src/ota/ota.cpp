#include <ArduinoOTA.h>
#include "definitions.h"
#include "configuration.h"
#include "settings.h"
#include "ota.h"

OTA::OTA(WiFi_Client& wifiClient) : mqtt(wifiClient) {

  ArduinoOTA.onStart([this]() {
    String type;

    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
    mqtt.publish_message("START UPDATING FIRMWARE");
  });

  ArduinoOTA.onEnd([this]() {
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
}

void OTA::start() {

  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  ArduinoOTA.setHostname(Definitions::APP_NAME);

  // authentication string
  ArduinoOTA.setPassword(Configuration::getString("PASSWORD", "liam").c_str());

  ArduinoOTA.begin();
  Serial.println("OTA available.");
}

void OTA::handle() {
  ArduinoOTA.handle();
}
