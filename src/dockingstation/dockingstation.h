#ifndef _liam_dockingstation_h
#define _liam_dockingstation_h

#include <Arduino.h>
#include <RadioLib.h>
#include <Ticker.h>

#include "resources.h"
#include "state_controller.h"

/**
 * Class resposible of maintaining a radio connection with the docking station.
 * The mower periodically send status and metrics messages to the docking
 * station, but also receives command from the docking station relayed from the
 * user. Currently we are relying on a LoRa radio technique that can communcate
 * over a great distance (1000 meter) in noisy environments, but at a slow speed
 * (150-200 Kbps).
 *
 * Using this libary, https://github.com/jgromes/RadioLib, and a Semtech SX1280
 * transceiver. Another useful library with lots of good documentation, however
 * with not as nice API and lacking interrupt support is
 * https://github.com/LoRaTracker/SX12XX-LoRa/blob/master/What%20is%20LoRa.md
 */
class Dockingstation {
 public:
  Dockingstation(StateController& stateController, Resources& resources);
  void start();

 private:
  // flag to indicate that a packet was received
  volatile bool receivedFlag = false;
  // disable interrupt when it's not needed
  volatile bool enableInterrupt = true;
  StateController& stateController;
  Resources& resources;
  Ticker pushNewInfoTicker;
  SX1280 lora;
  void IRAM_ATTR setReceivedFlag();
  void startReceive();
  bool transmit(uint8_t buffer[]);
  void collectAndPushNewStatus();
};

#endif
