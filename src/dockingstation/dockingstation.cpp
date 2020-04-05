#include <ArduinoLog.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "dockingstation.h"
#include "esp_log.h"
#include "definitions.h"
#include "utils.h"
#include "io_accelerometer/io_accelerometer.h"

// Useful LoRa information: https://www.youtube.com/channel/UCG5_CT_KjexxjbgNE4lVGkg/videos

/**
* Class used for all communication with the docking station, over a low bandwidth, long range LoRa-connection.
*/
Dockingstation::Dockingstation(StateController& stateController, Resources& resources) :
  stateController(stateController),
  resources(resources), lora(new LoRa(SS, Definitions::LORA_DIO0_PIN, Definitions::LORA_DIO1_PIN)) {
  
  }

/**
 * Collect status information from subsystems and push it to clients, if information has changed.
 */
void Dockingstation::collectAndPushNewStatus() {
  /*bool statusChanged = false;

  auto orient = resources.accelerometer.getOrientation();
  auto wheelStats = resources.wheelController.getStatus();
  auto obstacleDistances = resources.sonar.getObstacleDistances();

  currentStatus.state = stateController.getStateInstance()->getStateName();
  currentStatus.batteryVoltage = resources.battery.getBatteryVoltage();
  currentStatus.batteryLevel = resources.battery.getBatteryStatus();
  currentStatus.batteryChargeCurrent = resources.battery.getChargeCurrent();
  currentStatus.isCharging = resources.battery.isCharging();
  currentStatus.lastFullyChargeTime = resources.battery.getLastFullyChargeTime();
  currentStatus.lastChargeDuration = resources.battery.getLastChargeDuration();
  currentStatus.cutterLoad = resources.cutter.getLoad();
  currentStatus.cutterRotating = resources.cutter.isCutting();
  currentStatus.leftWheelSpd = wheelStats.leftWheelSpeed;
  currentStatus.rightWheelSpd = wheelStats.rightWheelSpeed;
  currentStatus.obstacleLeftDistance = obstacleDistances.leftDistance;
  currentStatus.obstacleFrontDistance = obstacleDistances.frontDistance;
  currentStatus.obstacleRightDistance = obstacleDistances.rightDistance;
  currentStatus.pitch = orient.pitch;
  currentStatus.roll = orient.roll;
  currentStatus.heading = orient.heading;	

  if (currentStatus.state != stateController.getStateInstance()->getStateName() ||
      currentStatus.batteryVoltage != resources.battery.getBatteryVoltage() ||
      currentStatus.batteryLevel != resources.battery.getBatteryStatus() ||
      currentStatus.batteryChargeCurrent != resources.battery.getChargeCurrent() ||
      currentStatus.isCharging != resources.battery.isCharging() ||
      currentStatus.lastFullyChargeTime != resources.battery.getLastFullyChargeTime() ||
      currentStatus.lastChargeDuration != resources.battery.getLastChargeDuration() ||
      currentStatus.cutterLoad != resources.cutter.getLoad() ||
      currentStatus.cutterRotating != resources.cutter.isCutting() ||
      currentStatus.leftWheelSpd != wheelStats.leftWheelSpeed ||
      currentStatus.rightWheelSpd != wheelStats.rightWheelSpeed ||
      currentStatus.obstacleLeftDistance != obstacleDistances.leftDistance ||
      currentStatus.obstacleFrontDistance != obstacleDistances.frontDistance ||
      currentStatus.obstacleRightDistance != obstacleDistances.rightDistance ||
      currentStatus.pitch != orient.pitch ||
      currentStatus.roll != orient.roll ||
      currentStatus.heading != orient.heading
    ) {
    statusChanged = true;
  }

  // we have to check that we are connected before we try to get WiFi signal, otherwise it will freeze up.
  if (WiFi.status() == WL_CONNECTED) {
    auto wifiSignal = WiFi.RSSI();
    
    if (currentStatus.wifiSignal != wifiSignal) {
      currentStatus.wifiSignal = wifiSignal;
      statusChanged = true;
    }
  }
  
  // These change so often that we don't set statusChanged for these, otherwise we would push everytime.
  currentStatus.uptime = (uint32_t)(esp_timer_get_time() / 1000000); // uptime in microseconds so we divide to seconds.

  if (statusChanged) {
    DynamicJsonBuffer jsonBuffer(380);
    JsonObject& root = jsonBuffer.createObject();
    statusToJson(currentStatus, root);

    resources.wlan.sendDataWebSocket("status", root);

    // MQTT updates don't have to be "realtime", we can settle with an update every 10 sec to not spam server.
    if (lastMQTT_push < currentStatus.uptime - 10) {
      String jsonStr;
      root.printTo(jsonStr);
      resources.wlan.publish_mqtt(jsonStr.c_str(), "/status");
      lastMQTT_push = currentStatus.uptime;
    }
  }*/
}

void Dockingstation::start() {
  // carrier frequency:           <Definitions::LORA_FREQ> MHz
  // bandwidth:                   250.0 kHz
  // spreading factor:            7
  // coding rate:                 4
  // sync word:                   0x12
  // output power:                2 dBm
  // current limit:               50 mA
  // preamble length:             8 symbols
  // amplifier gain:              1 (maximum gain)
  int state = lora.begin(Definitions::LORA_FREQ, 250, 7, 4, 0x12, 2, 50, 8, 1);
  if (state == ERR_NONE) {
    Log.notice(F("LoRa success!" CR));
  } else {
    Log.notice(F("LoRa failed, code %d" CR), state);
  }


  // start scanning current channel
  int scanresult = lora.scanChannel();

  if(scanresult == PREAMBLE_DETECTED) {
    // LoRa preamble was detected
    Log.notice(F(" detected preamble!" CR));

  } else if(scanresult == CHANNEL_FREE) {
    // no preamble was detected, channel is free
    Log.notice(F(" channel is free!" CR));

  }

//https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplex/LoRaDuplex.ino
//https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplexCallback/LoRaDuplexCallback.ino
}

