#include <Arduino.h>
#include "wheel.h"

Wheel::Wheel(uint8_t motor_pin, uint8_t motor_dir_pin, bool wheel_invert, uint8_t wheel_max_speed) : current_speed(0), max_speed(constrain(wheel_max_speed, 0, 100)), motor_pin(motor_pin), motor_dir_pin(motor_dir_pin), wheel_invert(wheel_invert) {
  pinMode(motor_pin, OUTPUT);
  pinMode(motor_dir_pin, OUTPUT);

  setSpeed(0);
}

Wheel::~Wheel() {
  setSpeed(0);
}

void Wheel::setSpeed(int8_t speed) {
  speed = constrain(speed, -max_speed, max_speed);
  current_speed = speed;

  // if speed is negative then reverse motor direction.
  if (speed < 0) {
    digitalWrite(motor_dir_pin, wheel_invert ? 1 : 0);
  } else {
    digitalWrite(motor_dir_pin, wheel_invert ? 0 : 1);
  }
  //https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/AnalogOut/LEDCSoftwareFade/LEDCSoftwareFade.ino
  //analogWrite(motor_pin, map(abs(speed), 0, 100, 0, PWMRANGE));
}

int8_t Wheel::getSpeed() {
  return current_speed;
}
