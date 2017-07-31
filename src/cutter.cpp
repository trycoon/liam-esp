#include "cutter.h"
#include "settings.h"

Cutter::Cutter(IO_Analog& io_analog) : io_analog(io_analog) {
  pinMode(Settings::CUTTER_MOTOR_PIN, OUTPUT);
  analogWrite(Settings::CUTTER_MOTOR_PIN, 0);
}

Cutter::~Cutter() {
  stop(true);
}

void Cutter::start() {
  //brakeTimer.once_ms(100, brake off!);
  analogWrite(Settings::CUTTER_MOTOR_PIN, map(constrain(Settings::CUTTER_MAX_SPEED, 0, 100), 0, 100, 0, PWMRANGE));
}

void Cutter::stop(bool brake) {
  analogWrite(Settings::CUTTER_MOTOR_PIN, 0);

  //brakeTimer.once_ms(100, brake on);
}

uint16_t Cutter::getLoad() {
  return 45;  //TODO: dummy value.
}
