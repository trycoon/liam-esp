#include <ArduinoLog.h>
#include "wheel_controller.h"

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

  Log.trace(F("WheelController-forward, speed: %d, turnrate: %d, smooth: %d" CR), speed, turnrate, smooth);

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

  Log.trace(F("WheelController-backward, speed: %d, turnrate: %d, smooth: %d" CR), speed, turnrate, smooth);

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

    Log.trace(F("WheelController-turn, direction: %d, currentHeading: %d, targetHeading: %d" CR), direction, currentHeading, targetHeading);

    // TODO: if deviation is small then turnspeed should be small to not overshoot target direction. Introduce different constants for this.
    // TODO: save current wheels speed and the callback function to be used in process-method
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

  Log.trace(F("WheelController-stop, smooth: %d" CR), smooth);
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
  // TODO: when we are close to target direction, reduce speed even more to not overshoot target.
  // TODO: when we are within 2-3 degrees range of target direction then we should concider us having hit the target, the risk is that we enter a endless loop constantly overshooting the target if we are trying to exactly match target direction.
  // TODO: call upon callback-function and set wheels speed to original value (saved at turn-method) when hitting target.
  auto currentHeading = accelerometer.getOrientation().heading;
  if (currentHeading == targetHeading) {
    leftWheel.setSpeed(0);
    rightWheel.setSpeed(0);
  }
}
