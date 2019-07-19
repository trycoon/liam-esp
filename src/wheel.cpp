#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include "wheel.h"
#include "definitions.h"

Wheel::Wheel(uint8_t wheel_id, uint8_t motor_pin, uint8_t motor_dir_pin, uint8_t odometer_pin, bool wheel_invert, uint8_t wheel_max_speed) : wheel_id(wheel_id), motor_pin(motor_pin), motor_dir_pin(motor_dir_pin), odometer_pin(odometer_pin), wheel_invert(wheel_invert), max_speed(constrain(wheel_max_speed, 0, 100)) {
  pinMode(motor_pin, OUTPUT);
  pinMode(motor_dir_pin, OUTPUT);
  pinMode(odometer_pin, INPUT_PULLUP);
  ledcSetup(wheel_id, Definitions::MOTOR_BASE_FREQ, Definitions::MOTOR_TIMER_13_BIT);
  ledcAttachPin(motor_pin, wheel_id);
  attachInterrupt(digitalPinToInterrupt(odometer_pin), std::bind(&Wheel::updateOdometer, this), RISING);

  setSpeed(0);
}

Wheel::~Wheel() {
  setSpeed(0);
  detachInterrupt(digitalPinToInterrupt(odometer_pin));
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
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = ((pow(2, Definitions::MOTOR_TIMER_13_BIT) - 1) / 100) * abs(speed);
  // write duty to motor using wheel_id as channel
  ledcWrite(wheel_id, duty);
}

int8_t Wheel::getSpeed() {
  return current_speed;
}

void IRAM_ATTR Wheel::updateOdometer() {
  portENTER_CRITICAL_ISR(&mux);
  odometer++;
  portEXIT_CRITICAL_ISR(&mux);
}

uint32_t Wheel::getOdometer() {
  return odometer;
}