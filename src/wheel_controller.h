#ifndef _wheel_controller_h
#define _wheel_controller_h

#include <functional>
#include "wheel.h"
#include "io_accelerometer.h"
#include "processable.h"

class WheelController : public Processable {
  public:
    WheelController(Wheel& leftWheel, Wheel& rightWheel, IO_Accelerometer& accelerometer);
    ~WheelController();
    /**
     * Drives mower forward at specified speed and turning at specified speed.
     * @param turnrate speed of turning (-127-0 left, 0-128 right). 0 = don't turn.
     * @param speed forward speed (0-100%)
     * @param smooth smoothly take us to target speed.
     */ 
    void forward(int8_t turnrate, uint8_t speed, bool smooth = false);
    /**
     * Drives mower backward at specified speed and turning at specified speed.
     * @param turnrate speed of turning (-127-0 left, 0-128 right). 0 = don't turn.
     * @param speed backward speed (0-100%)
     * @param smooth smoothly take us to target speed.
     */ 
    void backward(int8_t turnrate, uint8_t speed, bool smooth = false);
    /**
     * Turns mower on the spot.
     * @param direction turns mower to the specified direction. Direction is 0-360 degrees relative current heading.
     * @param function optional callback that will be executed once mower is facing desired direction.
     */ 
    void turn(uint16_t direction, std::function<void(void)> fn = nullptr);
    /**
     * Stop mowers movement.
     * @param smooth smoothly take us to halt.
     */     
    void stop(bool smooth = false);
    /* Internal use only! */
    void process();

  private:
    Wheel& leftWheel;
    Wheel& rightWheel;
    IO_Accelerometer& accelerometer;
};

#endif
