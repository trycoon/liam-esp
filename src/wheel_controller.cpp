#include "wheel_controller.h"
#include <ArduinoLog.h>

WheelController::WheelController(Wheel& leftWheel, Wheel& rightWheel)
    : leftWheel(leftWheel)
    , rightWheel(rightWheel)
{
}

WheelController::~WheelController()
{
    stop(false);
}

void WheelController::forward(int8_t turnrate, uint8_t speed, bool smooth, uint32_t distance, const TargetReachedCallback& fn)
{
    turnrate  = constrain(turnrate, -100, 100);
    speed     = constrain(speed, 0, 100);
    lastSpeed = 0;

    if (speed > 0 && speed < Definitions::WHEEL_MOTOR_MIN_SPEED)
    {
        speed = Definitions::WHEEL_MOTOR_MIN_SPEED;
    }

    Log.trace(F("WheelController-forward, speed: %d, turnrate: %d, smooth: %d, distance: %d" CR), speed, turnrate, smooth, distance);

    if (turnrate < 0)
    {
        leftWheel.setSpeed(speed * (100 + turnrate) / 100);
        rightWheel.setSpeed(speed);
    }
    else if (turnrate > 0)
    {
        leftWheel.setSpeed(speed);
        rightWheel.setSpeed(speed * (100 - turnrate) / 100);
    }
    else
    {
        leftWheel.setSpeed(speed);
        rightWheel.setSpeed(speed);
    }
}

void WheelController::backward(int8_t turnrate, uint8_t speed, bool smooth, uint32_t distance, const TargetReachedCallback& fn)
{
    turnrate  = constrain(turnrate, -100, 100);
    speed     = constrain(speed, 0, 100);
    lastSpeed = 0;

    if (speed > 0 && speed < Definitions::WHEEL_MOTOR_MIN_SPEED)
    {
        speed = Definitions::WHEEL_MOTOR_MIN_SPEED;
    }

    Log.trace(F("WheelController-backward, speed: %d, turnrate: %d, smooth: %d, distance: %d" CR), speed, turnrate, smooth, distance);

    if (turnrate < 0)
    {
        leftWheel.setSpeed(-speed * (100 + turnrate) / 100);
        rightWheel.setSpeed(-speed);
    }
    else if (turnrate > 0)
    {
        leftWheel.setSpeed(-speed);
        rightWheel.setSpeed(-speed * (100 + turnrate) / 100);
    }
    else
    {
        leftWheel.setSpeed(-speed);
        rightWheel.setSpeed(-speed);
    }
}

void WheelController::turn(int16_t direction, const TargetReachedCallback& fn)
{
    direction             = constrain(direction, -360, 360);
    reachedTargetCallback = fn;
    lastSpeed             = leftWheel.getSpeed(); // save current speed so that we can return to this after turn.

    Log.trace(F("WheelController-turn, direction: %i" CR), direction);

    if (direction < 0)
    {
        leftWheel.setSpeed(-Definitions::WHEEL_MOTOR_TURN_SPEED);
        rightWheel.setSpeed(Definitions::WHEEL_MOTOR_TURN_SPEED);
    }
    else if (direction > 0)
    {
        leftWheel.setSpeed(Definitions::WHEEL_MOTOR_TURN_SPEED);
        rightWheel.setSpeed(-Definitions::WHEEL_MOTOR_TURN_SPEED);
    }
}

void WheelController::stop(bool smooth)
{
    leftWheel.setSpeed(0);
    rightWheel.setSpeed(0);
    reachedTargetCallback = nullptr;
    lastSpeed             = 0;

    Log.trace(F("WheelController-stop, smooth: %d" CR), smooth);
}

status WheelController::getStatus()
{
    return { leftWheel.getSpeed(), rightWheel.getSpeed() };
}

bool WheelController::increaseForwardSpeed()
{
    auto leftSpeed  = leftWheel.getSpeed();
    auto rightSpeed = rightWheel.getSpeed();

    if (leftSpeed == rightSpeed && leftSpeed > 0 && leftSpeed < 100)
    {
        leftWheel.setSpeed(leftSpeed + 10);
        rightWheel.setSpeed(rightSpeed + 10);

        return true;
    }

    return false;
}

bool WheelController::decreaseForwardSpeed()
{
    auto leftSpeed  = leftWheel.getSpeed();
    auto rightSpeed = rightWheel.getSpeed();

    if (leftSpeed == rightSpeed && leftSpeed >= 20)
    {
        leftWheel.setSpeed(leftSpeed - 10);
        rightWheel.setSpeed(rightSpeed - 10);

        return true;
    }

    return false;
}

void WheelController::process()
{
    // TODO: handle smooth-running.

    // check if we have reached target
    /*if (targetOdometer > 0 && leftWheel.getOdometer() >= targetOdometer) {
      targetOdometer = 0;
      Log.trace(F("WheelController-process, reached target" CR));

      leftWheel.setSpeed(lastSpeed);
      rightWheel.setSpeed(lastSpeed);

      if (reachedTargetCallback != nullptr) {
        reachedTargetCallback();
        reachedTargetCallback = nullptr;
      }
    }*/
}
