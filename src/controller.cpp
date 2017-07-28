#include "controller.h"

Controller::Controller(Resources& resources, IO_Accelerometer& io_accelerometer) :
            resources(resources),
            io_accelerometer(io_accelerometer),
            stateController(Definitions::MOWER_STATES::DOCKED, resources) {  // When started we assume we are docked!

}

void Controller::run() {
  // always check if we are flipped.
  if (io_accelerometer.isFlipped() && stateController.getStateInstance()->getState() != Definitions::MOWER_STATES::FLIPPED) {
    stateController.setState(Definitions::MOWER_STATES::FLIPPED);
  }
io_accelerometer.getOrientation(); //TODO: remove!
  stateController.getStateInstance()->run();
}
