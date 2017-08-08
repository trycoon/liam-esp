#include "cutter.h"
#include "settings.h"

Cutter::Cutter(IO_Analog& io_analog, IO_Digital& io_digital) : io_analog(io_analog), io_digital(io_digital), cutting(false), load(0) {
  pinMode(Settings::CUTTER_MOTOR_PIN, OUTPUT);
  analogWrite(Settings::CUTTER_MOTOR_PIN, 0);
}

Cutter::~Cutter() {
  stop(true);
}

void Cutter::start() {
  senseLoadTimerId = senseLoadTimer.schedule([this]() {
    senseLoad();
  }, 100, true);

  cutting = true;

  io_digital.digitalWrite(Settings::CUTTER_BRAKE_PIN, false);
  delay(10);
  analogWrite(Settings::CUTTER_MOTOR_PIN, map(constrain(Settings::CUTTER_MAX_SPEED, 0, 100), 0, 100, 0, PWMRANGE));
}

void Cutter::stop(bool brake) {
  analogWrite(Settings::CUTTER_MOTOR_PIN, 0);

  cutting = false;

  if (brake) {
    delay(10);
    io_digital.digitalWrite(Settings::CUTTER_BRAKE_PIN, true);
  }

  senseLoadTimer.unschedule(senseLoadTimerId);
}

void Cutter::senseLoad() {
  //TODO: convert to percent.
  load = io_analog.getChannelVoltage(Settings::CUTTER_LOAD_CHANNEL);
}

uint8_t Cutter::getLoad() {
  return load;
}

void Cutter::process() {
  senseLoadTimer.process();
}
