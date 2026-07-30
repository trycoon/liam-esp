#include "dockingstation.h"

#include <ArduinoLog.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "definitions.h"
#include "esp_log.h"
#include "io_accelerometer/io_accelerometer.h"
#include "utils.h"
#include <PJONThroughLora.h>


// Useful LoRa information:
// https://www.youtube.com/channel/UCG5_CT_KjexxjbgNE4lVGkg/videos

// Use PJON, https://github.com/gioblu/PJON, as LoRa communiccation protocol!
// https://github.com/gioblu/PJON/tree/master/src/strategies/ThroughLoRahttps://github.com/gioblu/PJON/tree/master/src/strategies/ThroughLoRa

/**
 * Class used for all communication with the docking station, over a low
 * bandwidth, long range LoRa-connection.
 */
Dockingstation::Dockingstation(StateController& stateController,
                               Resources& resources)
    : stateController(stateController),
      resources(resources),
      lora(new Module(Definitions::LORA_NSS_PIN, Definitions::LORA_DIO1_PIN,
                      Definitions::LORA_NRST_PIN, Definitions::LORA_BUSY_PIN)) {

}

/*
  This function is called when a complete packet is received by the module.
*/
void IRAM_ATTR Dockingstation::setReceivedFlag() {
  // check if the interrupt is enabled
  if (!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void Dockingstation::start() {
  // carrier frequency:           2400.0 MHz
  // bandwidth:                   1625 kHz
  // spreading factor:            6
  // coding rate:                 5
  // output power:                10 dBm
  // preamble length:             12 symbols
  int16_t state = lora.begin(2400.0, 1625, 6, 5, 10, 12);
  if (state == RADIOLIB_ERR_NONE) {
    Log.notice(F("LoRa initialized." CR));
  } else {
    Log.error(F("LoRa failed to initialize, code %d" CR), state);
    delay(10000);
    ESP.restart();
  }

  // set the function that will be called
  // when new packets is received
  lora.setDio1Action([]() { &Dockingstation::setReceivedFlag; });

  startReceive();
}

void Dockingstation::startReceive() {
  int16_t state = lora.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    Log.error(F("LoRa failed to start receive, code %d" CR), state);
    delay(2000);
    startReceive();
  }
}

bool Dockingstation::transmit(uint8_t* buffer, size_t length) {
  if (length > 253) {
    Log.warning(F("LoRa transmission fault, buffer size (%d) excede allowed "
                  "size of 253 bytes! Ignoring transmission." CR),
                length);
    return false;
  }

  // start scanning current channel for ongoing transmissions
  int state = lora.scanChannel();

  if (state == RADIOLIB_LORA_DETECTED) {
    Log.trace(
        F("Ongoing LoRa transmission detected, waiting a moment and trying "
          "again." CR));
    delay(random(20, 25));
    return transmit(buffer, length);
  } else if (state != RADIOLIB_CHANNEL_FREE) {
    Log.warning(F("LoRa unknown fault during channel scanning: %d. Trying to "
                  "send anyway." CR),
                state);
  }

  state = lora.transmit(buffer, length);
  if (state != RADIOLIB_ERR_NONE) {
    Log.warning(
        F("LoRa unknown transmission fault: %d. Ignoring transmission." CR),
        state);
    return false;
  }

  startReceive();

  return true;
}

/**
 * Collect status information from subsystems and push it to clients, if
 * information has changed.
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
  currentStatus.lastFullyChargeTime =
  resources.battery.getLastFullyChargeTime(); currentStatus.lastChargeDuration =
  resources.battery.getLastChargeDuration(); currentStatus.cutterLoad =
  resources.cutter.getLoad(); currentStatus.cutterRotating =
  resources.cutter.isCutting(); currentStatus.leftWheelSpd =
  wheelStats.leftWheelSpeed; currentStatus.rightWheelSpd =
  wheelStats.rightWheelSpeed; currentStatus.obstacleLeftDistance =
  obstacleDistances.leftDistance; currentStatus.obstacleFrontDistance =
  obstacleDistances.frontDistance; currentStatus.obstacleRightDistance =
  obstacleDistances.rightDistance; currentStatus.pitch = orient.pitch;
  currentStatus.roll = orient.roll;
  currentStatus.heading = orient.heading;

  if (currentStatus.state != stateController.getStateInstance()->getStateName()
  || currentStatus.batteryVoltage != resources.battery.getBatteryVoltage() ||
      currentStatus.batteryLevel != resources.battery.getBatteryStatus() ||
      currentStatus.batteryChargeCurrent != resources.battery.getChargeCurrent()
  || currentStatus.isCharging != resources.battery.isCharging() ||
      currentStatus.lastFullyChargeTime !=
  resources.battery.getLastFullyChargeTime() || currentStatus.lastChargeDuration
  != resources.battery.getLastChargeDuration() || currentStatus.cutterLoad !=
  resources.cutter.getLoad() || currentStatus.cutterRotating !=
  resources.cutter.isCutting() || currentStatus.leftWheelSpd !=
  wheelStats.leftWheelSpeed || currentStatus.rightWheelSpd !=
  wheelStats.rightWheelSpeed || currentStatus.obstacleLeftDistance !=
  obstacleDistances.leftDistance || currentStatus.obstacleFrontDistance !=
  obstacleDistances.frontDistance || currentStatus.obstacleRightDistance !=
  obstacleDistances.rightDistance || currentStatus.pitch != orient.pitch ||
      currentStatus.roll != orient.roll ||
      currentStatus.heading != orient.heading
    ) {
    statusChanged = true;
  }

  // we have to check that we are connected before we try to get WiFi signal,
  otherwise it will freeze up. if (WiFi.status() == WL_CONNECTED) { auto
  wifiSignal = WiFi.RSSI();

    if (currentStatus.wifiSignal != wifiSignal) {
      currentStatus.wifiSignal = wifiSignal;
      statusChanged = true;
    }
  }

  // These change so often that we don't set statusChanged for these, otherwise
  we would push everytime. currentStatus.uptime =
  (uint32_t)(esp_timer_get_time() / 1000000); // uptime in microseconds so we
  divide to seconds.

  if (statusChanged) {
    DynamicJsonBuffer jsonBuffer(380);
    JsonObject& root = jsonBuffer.createObject();
    statusToJson(currentStatus, root);

    resources.wlan.sendDataWebSocket("status", root);

    // MQTT updates don't have to be "realtime", we can settle with an update
  every 10 sec to not spam server. if (lastMQTT_push < currentStatus.uptime -
  10) { String jsonStr; root.printTo(jsonStr);
      resources.wlan.publish_mqtt(jsonStr.c_str(), "/status");
      lastMQTT_push = currentStatus.uptime;
    }
  }*/
}
