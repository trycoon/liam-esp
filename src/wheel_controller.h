#ifndef _wheel_controller_h
#define _wheel_controller_h

#include "wheel.h"

class WheelController {
  public:
    WheelController(Wheel& leftWheel, Wheel& rightWheel);
    ~WheelController();
    void forward(bool smooth = false);
    void backward(bool smooth = false);
    void turnLeft(bool smooth = false, uint16_t milliseconds = 500);
    void turnRight(bool smooth = false, uint16_t milliseconds = 500);
    void stop(bool smooth = false);
    void process();

  private:
    Wheel& leftWheel;
    Wheel& rightWheel;
};

#endif
