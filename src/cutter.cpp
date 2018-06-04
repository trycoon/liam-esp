#include "cutter.h"
#include "settings.h"

Cutter::Cutter(IO_Analog& io_analog) : io_analog(io_analog), cutting(false), load(0) {
  pinMode(Settings::CUTTER_MOTOR_PIN, OUTPUT);

  sigmaDeltaSetup(Cutter::CHANNEL, 312500);
  //attach pin to channel
  sigmaDeltaAttachPin(Settings::CUTTER_MOTOR_PIN, Cutter::CHANNEL);
  stop(false);
}

Cutter::~Cutter() {
  stop(true);
}

void Cutter::start() {
  senseLoadTimerId = senseLoadTimer.schedule([this]() {
    senseLoad();
  }, 100, true);

  cutting = true;

  digitalWrite(Settings::CUTTER_BRAKE_PIN, LOW);
  delay(10);
  sigmaDeltaWrite(Cutter::CHANNEL, map(constrain(Settings::CUTTER_MAX_SPEED, 0, 100), 0, 100, 0, 255));
}

void Cutter::stop(bool brake) {
  sigmaDeltaWrite(Cutter::CHANNEL, 0);

  cutting = false;

  if (brake) {
    delay(10);
    digitalWrite(Settings::CUTTER_BRAKE_PIN, HIGH);
  } else {
    digitalWrite(Settings::CUTTER_BRAKE_PIN, LOW);
  }

  senseLoadTimer.unschedule(senseLoadTimerId);
}

bool Cutter::isCutting() {
    return cutting;
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
