#include <ArduinoOTA.h>
#include <ArduinoLog.h>
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
    Log.notice(F("Start updating %s" CR), type);
    mqtt.publish_message("START UPDATING FIRMWARE");
  });

  ArduinoOTA.onEnd([this]() {
    Log.notice(F(CR "Done updating" CR));
    mqtt.publish_message("DONE UPDATING FIRMWARE");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Log.notice("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    if (error == OTA_AUTH_ERROR) {
      Log.error("Error[%u]: Auth Failed" CR, error);
    } else if (error == OTA_BEGIN_ERROR) {
      Log.error("Error[%u]: Begin Failed" CR, error);
    } else if (error == OTA_CONNECT_ERROR) {
      Log.error("Error[%u]: Connect Failed" CR, error);
    } else if (error == OTA_RECEIVE_ERROR) {
      Log.error("Error[%u]: Receive Failed" CR, error);
    } else if (error == OTA_END_ERROR) {
      Log.error("Error[%u]: End Failed" CR, error);
    }
  });
}

void OTA::start() {

  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  ArduinoOTA.setHostname(Definitions::APP_NAME);

  // authentication string
  ArduinoOTA.setPassword(Configuration::getString("PASSWORD", "liam").c_str());

  ArduinoOTA.begin();
  Log.notice(F("OTA available." CR));
}

void OTA::handle() {
  ArduinoOTA.handle();
}
