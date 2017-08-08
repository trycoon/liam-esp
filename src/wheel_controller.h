#ifndef _wheel_controller_h
#define _wheel_controller_h

#include <functional>
#include "wheel.h"
#include "processable.h"

class WheelController : public Processable {
  public:
    WheelController(Wheel& leftWheel, Wheel& rightWheel);
    ~WheelController();
    void forward(bool smooth = false);
    void backward(bool smooth = false);
    void turnLeft(bool smooth = false, uint16_t milliseconds = 500, std::function<void(void)> fn = nullptr);
    void turnRight(bool smooth = false, uint16_t milliseconds = 500, std::function<void(void)> fn = nullptr);
    void stop(bool smooth = false);
    void process();

  private:
    Wheel& leftWheel;
    Wheel& rightWheel;
};

#endif
