#include "controller.h"

Controller::Controller(Resources& resources) :
            resources(resources),
            stateController(Definitions::MOWER_STATES::DOCKED, resources) {  // When started we assume we are docked!

}

void Controller::run() {
  stateController.getStateInstance()->run();
}
