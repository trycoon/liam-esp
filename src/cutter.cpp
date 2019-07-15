#include <ArduinoLog.h>
#include "cutter.h"
#include "definitions.h"

Cutter::Cutter(IO_Analog& io_analog) : cutter_id(3), io_analog(io_analog), cutterSpeed(0) {
  pinMode(Definitions::CUTTER_MOTOR_PIN, OUTPUT);
  pinMode(Definitions::CUTTER_BRAKE_PIN, OUTPUT);
  digitalWrite(Definitions::CUTTER_BRAKE_PIN, LOW);
  
  ledcSetup(cutter_id, Definitions::MOTOR_BASE_FREQ, Definitions::MOTOR_TIMER_13_BIT);
  ledcAttachPin(Definitions::CUTTER_MOTOR_PIN, cutter_id);

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

    digitalWrite(Definitions::CUTTER_BRAKE_PIN, LOW);

    delay(10);

    cutterSpeed = Definitions::CUTTER_MAX_SPEED;
    // calculate duty, 8191 from 2 ^ 13 - 1
    uint32_t duty = ((pow(2, Definitions::MOTOR_TIMER_13_BIT) - 1) / 100) * abs(cutterSpeed);
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
      digitalWrite(Definitions::CUTTER_BRAKE_PIN, HIGH);
    } else {
      digitalWrite(Definitions::CUTTER_BRAKE_PIN, LOW);
    }

    cutterLoadReadingTicker.detach();

    Log.trace(F("Cutter-stop, brake: %d" CR), brake);
  }
}

bool Cutter::isCutting() {
    return cutterSpeed > 0;
}

void Cutter::senseLoad() {
  auto adc_reading = io_analog.getVoltage(Definitions::CUTTER_LOAD_PIN);

  load = round((adc_reading * Definitions::CUTTER_LOAD_RESISTOR_MULTIPLIER - Definitions::CUTTER_NOLOAD_CURRENT) / (Definitions::CUTTER_MAX_CURRENT - Definitions::CUTTER_NOLOAD_CURRENT) * 100);

  // make sure we stay within percentage boundaries.
  if (load < 0) {
    load = 0;
  } else if (load > 100) {
    load = 100;
  }
}

uint8_t Cutter::getLoad() {
  return load;
}