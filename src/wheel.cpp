#include <Arduino.h>
#include "wheel.h"

Wheel::Wheel(uint8_t motor_pin, uint8_t motor_dir_pin, bool wheel_invert) : target_speed(0), motor_pin(motor_pin), motor_dir_pin(motor_dir_pin), wheel_invert(wheel_invert) {
  pinMode(motor_pin, OUTPUT);
  pinMode(motor_dir_pin, OUTPUT);
  analogWrite(motor_pin, 0);
}

Wheel::~Wheel() {
  analogWrite(motor_pin, 0);
}

void Wheel::speed(int16_t speed) {

}

void Wheel::stop() {
  analogWrite(motor_pin, 0);
  target_speed = 0;
}

int16_t Wheel::getSpeed() {
  return target_speed;
}
