#include <Arduino.h>
#include "wheel.h"

Wheel::Wheel(uint8_t motor_pin, uint8_t motor_dir_pin, bool wheel_invert) : speed(0), motor_pin(motor_pin), motor_dir_pin(motor_dir_pin), wheel_invert(wheel_invert) {
  pinMode(motor_pin, OUTPUT);
  pinMode(motor_dir_pin, OUTPUT);
  digitalWrite(motor_pin, 0);

}

Wheel::~Wheel() {
  digitalWrite(motor_pin, 0);
}

void Wheel::forward(bool smooth) {

}

void Wheel::forward(uint32_t time, bool smooth) {

}

void Wheel::backward(bool smooth){

}

void Wheel::backward(uint32_t time, bool smooth) {

}

void Wheel::stop(bool smooth) {

}
