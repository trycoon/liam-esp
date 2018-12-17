#include <ArduinoLog.h>
#include "wheel_controller.h"

static const float PULSE_PER_CENTIMETER = Definitions::WHEEL_ODOMETERPULSES_PER_ROTATION / (Definitions::WHEEL_DIAMETER * PI / 10);
static const float PULSE_PER_DEGREE = (((Definitions::WHEEL_PAIR_DISTANCE * PI) / (Definitions::WHEEL_DIAMETER * PI / 10)) * Definitions::WHEEL_ODOMETERPULSES_PER_ROTATION) / 360;

WheelController::WheelController(Wheel& leftWheel, Wheel& rightWheel) :
            leftWheel(leftWheel),
            rightWheel(rightWheel) { }

WheelController::~WheelController() {
  stop(false);
}

void WheelController::forward(int8_t turnrate, uint8_t speed, bool smooth, uint32_t distance, const TargetReachedCallback& fn) {
  turnrate = constrain(turnrate, -100, 100);
  speed = constrain(speed, 0, 100);
  lastSpeed = 0;

  if (distance > 0) {
    auto currentOdometer = leftWheel.getOdometer(); // we only need to count on one wheel, since they always the same distance (but maybe in the opposite direction)
    targetOdometer = currentOdometer + distance * PULSE_PER_CENTIMETER;
    reachedTargetCallback = fn;
  } else {
    targetOdometer = 0;
  }

  Log.trace(F("WheelController-forward, speed: %d, turnrate: %d, smooth: %d, distance: %d" CR), speed, turnrate, smooth, distance);

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

void WheelController::backward(int8_t turnrate, uint8_t speed, bool smooth, uint32_t distance, const TargetReachedCallback& fn) {
  turnrate = constrain(turnrate, -100, 100);
  speed = constrain(speed, 0, 100);
  lastSpeed = 0;

  if (distance > 0) {
    auto currentOdometer = leftWheel.getOdometer(); // we only need to count on one wheel, since they always the same distance (but maybe in the opposite direction)
    targetOdometer = currentOdometer + distance * PULSE_PER_CENTIMETER;
    reachedTargetCallback = fn;
  } else {
    targetOdometer = 0;
  }

  Log.trace(F("WheelController-backward, speed: %d, turnrate: %d, smooth: %d, distance: %d" CR), speed, turnrate, smooth, distance);

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

void WheelController::turn(int16_t direction, const TargetReachedCallback& fn) {
  direction = constrain(direction, -360, 360);
  reachedTargetCallback = fn;
  lastSpeed = leftWheel.getSpeed(); // save current speed so that we can return to this after turn.

  auto currentOdometer = leftWheel.getOdometer(); // we only need to count on one wheel, since they always the same distance (but maybe in the opposite direction)
  targetOdometer = currentOdometer + abs(direction) * PULSE_PER_DEGREE;
  
  Log.trace(F("WheelController-turn, direction: %i, currentOdometer: %i, targetOdometer: %i" CR), direction, currentOdometer, targetOdometer);

  if (direction < 0) {
    leftWheel.setSpeed(-Definitions::WHEEL_MOTOR_TURN_SPEED);
    rightWheel.setSpeed(Definitions::WHEEL_MOTOR_TURN_SPEED);
  } else if (direction > 0) {
    leftWheel.setSpeed(Definitions::WHEEL_MOTOR_TURN_SPEED);
    rightWheel.setSpeed(-Definitions::WHEEL_MOTOR_TURN_SPEED);
  }
}

void WheelController::stop(bool smooth) {
  leftWheel.setSpeed(0);
  rightWheel.setSpeed(0);
  targetOdometer = 0;
  lastSpeed = 0;

  Log.trace(F("WheelController-stop, smooth: %d" CR), smooth);
}

status WheelController::getStatus() {
  return { 
    leftWheel.getSpeed(), 
    rightWheel.getSpeed() 
  };
}

void WheelController::process() {
  // TODO: handle smooth-running.

  // check if we have reached target
  if (targetOdometer > 0 && leftWheel.getOdometer() >= targetOdometer) {
    targetOdometer = 0;
    Log.trace(F("WheelController-process, reached target" CR));

    leftWheel.setSpeed(lastSpeed);
    rightWheel.setSpeed(lastSpeed);

    if (reachedTargetCallback != nullptr) {
      reachedTargetCallback();
    }
  }
}
