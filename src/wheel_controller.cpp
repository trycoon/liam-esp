#include "wheel_controller.h"
#include "settings.h"

WheelController::WheelController(Wheel& leftWheel, Wheel& rightWheel, IO_Accelerometer& accelerometer) :
            leftWheel(leftWheel),
            rightWheel(rightWheel),
            accelerometer(accelerometer) { }

WheelController::~WheelController() {
  stop(false);
}

void WheelController::forward(int8_t turnrate, uint8_t speed, bool smooth) {
  turnrate = constrain(turnrate, -100, 100);
  speed = constrain(speed, 0, 100);

  if (turnrate < 0) {
    leftWheel.setSpeed(speed + turnrate);
    rightWheel.setSpeed(speed);
  } else if (turnrate > 0) {
    leftWheel.setSpeed(speed);
    rightWheel.setSpeed(speed - turnrate);
  } else {
    leftWheel.setSpeed(speed);
    rightWheel.setSpeed(speed);
  }  
}

void WheelController::backward(int8_t turnrate, uint8_t speed, bool smooth) {
  turnrate = constrain(turnrate, -100, 100);
  speed = constrain(speed, 0, 100);

  if (turnrate < 0) {
    leftWheel.setSpeed(-speed - turnrate);
    rightWheel.setSpeed(-speed);
  } else if (turnrate > 0) {
    leftWheel.setSpeed(-speed);
    rightWheel.setSpeed(-speed + turnrate);
  } else {
    leftWheel.setSpeed(-speed);
    rightWheel.setSpeed(-speed);
  }
}

void WheelController::turn(int16_t direction, std::function<void(void)> fn) {
  direction = constrain(direction, -360, 360);
  // ignore too small changes since compass and motors may not be that accurate.
  if (abs(direction) > 3) {
    auto currentHeading = accelerometer.getOrientation().heading;
    targetHeading = currentHeading + direction;
    
    // keep within 0-360 degrees
    if (targetHeading < 0) {
      targetHeading += 360;
    } else if (targetHeading > 360) {
      targetHeading -= 360;
    }

    if (direction < 0) {
      leftWheel.setSpeed(-70);
      rightWheel.setSpeed(70);
    } else if (direction > 0) {
      leftWheel.setSpeed(70);
      rightWheel.setSpeed(-70);
    }
  }
}

void WheelController::stop(bool smooth) {
  leftWheel.setSpeed(0);
  rightWheel.setSpeed(0);
}

status WheelController::getStatus() {
  return { 
    leftWheel.getSpeed(), 
    rightWheel.getSpeed(), 
    targetHeading
  };
}

void WheelController::process() {
  // TODO: handle smooth-running.

  auto currentHeading = accelerometer.getOrientation().heading;
  if (currentHeading == targetHeading) {
    leftWheel.setSpeed(0);
    rightWheel.setSpeed(0);
  }
}
