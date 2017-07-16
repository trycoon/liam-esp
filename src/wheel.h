#ifndef _wheel_h
#define _wheel_h

#include <Arduino.h>

class Wheel {
  public:
    Wheel(uint8_t motor_pin, uint8_t motor_dir_pin, bool wheel_invert);
    ~Wheel();
    void forward(bool smooth);
    void forward(uint32_t time, bool smooth);
    void backward(bool smooth);
    void backward(uint32_t time, bool smooth);
    void stop(bool smooth);

  private:
    uint16_t speed;
    uint8_t motor_pin;
    uint8_t motor_dir_pin;
    bool wheel_invert;
};

#endif
