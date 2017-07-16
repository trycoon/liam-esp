#include "controller.h"

Controller::Controller(Resources& resources, IO_Accelerometer& io_accelerometer) :
            resources(resources),
            io_accelerometer(io_accelerometer),
            stateController(Definitions::MOWER_STATES::DOCKED, resources) {  // When started we assume we are docked!

}

void Controller::run() {
  stateController.getStateInstance()->run();
}
