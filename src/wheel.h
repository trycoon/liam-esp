#ifndef _wheel_h
#define _wheel_h

#include <Arduino.h>

class Wheel {
  public:
    Wheel();
    ~Wheel();
    void init();
    void forward(bool smooth);
    void forward(uint32_t time, bool smooth);
    void backward(bool smooth);
    void backward(uint32_t time, bool smooth);
    void stop(bool smooth);

  private:
    uint16_t speed;
};

#endif
