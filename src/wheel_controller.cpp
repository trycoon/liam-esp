#include "wheel_controller.h"
#include "settings.h"

WheelController::WheelController(Wheel& leftWheel, Wheel& rightWheel) :
            leftWheel(leftWheel),
            rightWheel(rightWheel) { }

WheelController::~WheelController() {
  stop(false);
}

void WheelController::forward(bool smooth) {
  leftWheel.setSpeed(100);
  rightWheel.setSpeed(100);
}

void WheelController::backward(bool smooth) {
  leftWheel.setSpeed(-100);
  rightWheel.setSpeed(-100);
}

void WheelController::turnLeft(bool smooth, uint16_t milliseconds) {
  leftWheel.setSpeed(-Settings::WHEEL_MOTOR_TURN_SPEED);
  rightWheel.setSpeed(Settings::WHEEL_MOTOR_TURN_SPEED);
}

void WheelController::turnRight(bool smooth, uint16_t milliseconds) {
  leftWheel.setSpeed(Settings::WHEEL_MOTOR_TURN_SPEED);
  rightWheel.setSpeed(-Settings::WHEEL_MOTOR_TURN_SPEED);
}

void WheelController::stop(bool smooth) {
  leftWheel.setSpeed(0);
  rightWheel.setSpeed(0);
}

void WheelController::process() {

}
