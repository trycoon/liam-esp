#ifndef _wheel_h
#define _wheel_h

#include <Arduino.h>

class Wheel {
  public:
    Wheel(uint8_t wheel_id, uint8_t motor_pin, uint8_t motor_dir_pin, bool wheel_invert, uint8_t wheel_max_speed);
    ~Wheel();
    void setSpeed(int8_t speed);
    int8_t getSpeed();

  private:
    uint8_t wheel_id;
    uint8_t motor_pin;
    uint8_t motor_dir_pin;
    bool wheel_invert;
    uint8_t max_speed;    
    int8_t current_speed;
};

#endif
