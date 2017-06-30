#include <Arduino.h>
#include "wheel.h"

Wheel::Wheel() {
  //pinMode(motor_pin, OUTPUT);
  speed = 0;
}

Wheel::~Wheel() {

}

void Wheel::init() {

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
