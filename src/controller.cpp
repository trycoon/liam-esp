#include "controller.h"

Controller::Controller(MQTT_Client& mqttClient,
                       Wheel& leftWheel,
                       Wheel& rightWheel,
                       Cutter& cutter,
                       BWF& bwf,
                       Battery& battery,
                       GPS& gps)
                       : mqttClient(mqttClient),
                         leftWheel(leftWheel),
                         rightWheel(rightWheel),
                         cutter(cutter),
                         bwf(bwf),
                         battery(battery),
                         gps(gps),
                         stateController(Definitions::MOWER_STATES::DOCKED) {

}

void Controller::run() {

}
