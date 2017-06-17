#ifndef wheel_h
#define wheel_h

#include <Arduino.h>

class Wheel {
  public:
    Wheel(uint8_t motor_pin);
    ~Wheel();
    void Init();
    void Forward(bool smooth);
    void Forward(uint32_t time, bool smooth);
    void Backward(bool smooth);
    void Backward(uint32_t time, bool smooth);
    void Stop(bool smooth);
};

#endif
