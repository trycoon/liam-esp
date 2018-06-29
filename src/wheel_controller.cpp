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

void WheelController::turn(uint16_t direction, std::function<void(void)> fn) {
  leftWheel.setSpeed(-Settings::WHEEL_MOTOR_TURN_SPEED);
  rightWheel.setSpeed(Settings::WHEEL_MOTOR_TURN_SPEED);
}

void WheelController::stop(bool smooth) {
  leftWheel.setSpeed(0);
  rightWheel.setSpeed(0);
}

void WheelController::process() {
  // TODO: handle smooth-running.
}
