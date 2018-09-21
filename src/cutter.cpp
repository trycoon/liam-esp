#include <ArduinoLog.h>
#include "cutter.h"
#include "settings.h"

Cutter::Cutter(IO_Analog& io_analog) : cutter_id(3), io_analog(io_analog), cutterSpeed(0) {
  pinMode(Settings::CUTTER_MOTOR_PIN, OUTPUT);
  pinMode(Settings::CUTTER_BRAKE_PIN, OUTPUT);
  digitalWrite(Settings::CUTTER_BRAKE_PIN, LOW);
  
  ledcSetup(cutter_id, Settings::MOTOR_BASE_FREQ, Settings::MOTOR_TIMER_13_BIT);
  ledcAttachPin(Settings::CUTTER_MOTOR_PIN, cutter_id);

  ledcWrite(cutter_id, cutterSpeed);
}

Cutter::~Cutter() {
  stop(true);
}

void Cutter::start() {
  if (cutterSpeed == 0) {
    cutterLoadReadingTicker.attach_ms<Cutter*>(100, [](Cutter* instance) {
      instance->senseLoad();
    }, this);

    digitalWrite(Settings::CUTTER_BRAKE_PIN, LOW);

    delay(10);

    cutterSpeed = Settings::CUTTER_MAX_SPEED;
    // calculate duty, 8191 from 2 ^ 13 - 1
    uint32_t duty = ((pow(2, Settings::MOTOR_TIMER_13_BIT) - 1) / 100) * abs(cutterSpeed);
    ledcWrite(cutter_id, duty);

    Log.trace(F("Cutter-start, speed: %d, dutyCycle: %d" CR), cutterSpeed, duty);
  }
}

void Cutter::stop(bool brake) {
  if (cutterSpeed > 0) {
    cutterSpeed = 0;
    ledcWrite(cutter_id, cutterSpeed);

    if (brake) {
      delay(10);
      digitalWrite(Settings::CUTTER_BRAKE_PIN, HIGH);
    } else {
      digitalWrite(Settings::CUTTER_BRAKE_PIN, LOW);
    }

    cutterLoadReadingTicker.detach();

    Log.trace(F("Cutter-stop, brake: %d" CR), brake);
  }
}

bool Cutter::isCutting() {
    return cutterSpeed > 0;
}

void Cutter::senseLoad() {
  //TODO: convert to percent.
  //load = io_analog.getVoltage(Settings::CUTTER_LOAD_PIN);
  load=10;  // TODO: make to set this to something otherwise we get an IntegerByDivision-error!
}

uint8_t Cutter::getLoad() {
  return load;
}