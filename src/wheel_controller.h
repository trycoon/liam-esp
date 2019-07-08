#ifndef _wheel_controller_h
#define _wheel_controller_h

#include <functional>
#include <Arduino.h>
#include "wheel.h"
#include "definitions.h"
#include "processable.h"

struct status {
  int16_t leftWheelSpeed;
  int16_t rightWheelSpeed;
};

class WheelController : public Processable {
  public:
    typedef std::function<void(void)> TargetReachedCallback;

    WheelController(Wheel& leftWheel, Wheel& rightWheel);
    ~WheelController();
    /**
     * Drives mower forward at specified speed and turning at specified speed.
     * @param turnrate speed of turning (-1 to -100 left, 1 to 100 right). 0 = don't turn.
     * @param speed forward speed (0-100%)
     * @param smooth smoothly take us to target speed.
     * @param distance [optional] distance we want mower to move (in centimeters).
     * @param fn [optional] callback that will be executed once mower has moved desired distance.
     */ 
    void forward(int8_t turnrate, uint8_t speed, bool smooth = false, uint32_t distance = 0, const TargetReachedCallback& fn = nullptr);
    /**
     * Drives mower backward at specified speed and turning at specified speed.
     * @param turnrate speed of turning (-1 to -100 left, 1 to 100 right). 0 = don't turn.
     * @param speed backward speed (0-100%)
     * @param smooth smoothly take us to target speed.
     * @param distance [optional] distance we want mower to move (in centimeters).
     * @param fn [optional] callback that will be executed once mower has moved desired distance.
     */ 
    void backward(int8_t turnrate, uint8_t speed, bool smooth = false, uint32_t distance = 0, const TargetReachedCallback& fn = nullptr);
    /**
     * Turns mower on the spot.
     * @param direction turns mower to the specified direction. Direction is -360 -> 360 degrees relative current heading.
     * @param fn [optional] callback that will be executed once mower is facing desired direction.
     */ 
    void turn(int16_t direction, const TargetReachedCallback& fn = nullptr);
    /**
     * Stop mowers movement.
     * @param smooth smoothly take us to halt.
     */     
    void stop(bool smooth = false);

    status getStatus();

    /* Internal use only! */
    void process();

  private:
    Wheel& leftWheel;
    Wheel& rightWheel;
    uint32_t targetOdometer;
    int8_t targetSpeed;
    int8_t lastSpeed;
    TargetReachedCallback reachedTargetCallback;
};

#endif
