#ifndef _wheel_h
#define _wheel_h

#include <Arduino.h>

class Wheel {
  public:
    Wheel(uint8_t motor_pin, uint8_t motor_dir_pin, bool wheel_invert);
    ~Wheel();
    void speed(int16_t speed);
    int16_t getSpeed();
    void stop();

  private:
    int16_t target_speed;
    uint8_t motor_pin;
    uint8_t motor_dir_pin;
    bool wheel_invert;
};

#endif
