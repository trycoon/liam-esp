//https://github.com/StefanBruens/ESP8266_new_pwm
/*
analogWrite(pin, value) enables software PWM on the given pin.
PWM may be used on pins 0 to 16. Call analogWrite(pin, 0) to disable PWM on the pin.
value may be in range from 0 to PWMRANGE, which is equal to 1023 by default. PWM range may be changed by calling analogWriteRange(new_range).

PWM frequency is 1kHz by default. Call analogWriteFreq(new_frequency) to change the frequency.



*/
#include "cutter.h"

Cutter::Cutter(IO_Analog& io_analog) : io_analog(io_analog) {
  //map - map values between 0-100
  // constrain - constrain value bwteen to values, e.g. 0-100
}

Cutter::~Cutter() {
  stop();
}

void Cutter::setSpeed(uint8_t speed) {

}

void Cutter::stop() {

}

uint16_t Cutter::getLoad() {
  return 45;
}
