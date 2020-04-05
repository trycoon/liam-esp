#include <ArduinoLog.h>
#include "cutter.h"
#include "definitions.h"
#include "utils.h"

Cutter::Cutter(IO_Analog& io_analog) : cutter_id(3), io_analog(io_analog) {
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
    cutterCurrentSpeed = 1;

    setCutterSpeed(cutterCurrentSpeed);

    Log.trace(F("Cutter-start, speed: %d" CR), cutterSpeed);
  }
}

void Cutter::stop(bool brake) {
  if (cutterSpeed > 0) {
    cutterSpeed = 0;
    cutterCurrentSpeed = 0;

    setCutterSpeed(cutterCurrentSpeed);

    if (brake) {
      delay(10);
      digitalWrite(Definitions::CUTTER_BRAKE_PIN, HIGH);
    } else {
      digitalWrite(Definitions::CUTTER_BRAKE_PIN, LOW);
    }

    cutterLoadReadingTicker.detach();
    load = 0;

    Log.trace(F("Cutter-stop, brake: %d" CR), brake);
  }
}

bool Cutter::isCutting() {
    return cutterSpeed > 0;
}

void Cutter::senseLoad() {

  auto current = round(io_analog.getVoltageAdc1(Definitions::CUTTER_LOAD_CHANNEL) / Definitions::CUTTER_LOAD_RESISTOR * 1000); // 1000 for converting ampere to milliampere
  auto newLoad = round((current - Definitions::CUTTER_NOLOAD_CURRENT) / (Definitions::CUTTER_MAX_CURRENT - Definitions::CUTTER_NOLOAD_CURRENT) * 100);

  // make sure we stay within percentage boundaries.
  if (newLoad < 0) {
    newLoad = 0;
  } else if (newLoad > 100) {
    newLoad = 100;
  }

  loadMedian[loadMedianIndex++ % LOAD_MEDIAN_SAMPLES] = newLoad;  // enter new reading into array.
  load = Utils::calculateMedian<uint8_t>(loadMedian);
  
  // keep count of how long we have been overloaded. This handles short burst of overload.
  if (load > Definitions::CUTTER_LOAD_THRESHOLD) {
    overloadCounter++;
  } else if (overloadCounter > 0) {
    overloadCounter--;
  }

  //Log.notice("%F mA, %d%%" CR, current, load);
}

void Cutter::setCutterSpeed(uint8_t speed) {

  if (speed == 0) {
    ledcWrite(cutter_id, 0);
  } else {
    // calculate duty, 8191 from 2 ^ 13 - 1
    uint32_t duty = ((pow(2, Definitions::MOTOR_TIMER_13_BIT) - 1) / 100) * abs(speed);
    ledcWrite(cutter_id, duty);
  }
}

uint8_t Cutter::getLoad() {
  return load;
}

/**
 * Signals that cuttermotor has surpassed the CUTTER_LOAD_THRESHOLD and is working too hard keeping up.
 * Continue working at this load may blow a fuse.
 */
bool Cutter::isOverloaded() {
  return overloadCounter > 15;
}

/**
 * Signals that the cuttermotor fuse has blown and we are no longer cutting any grass.
 * Stop cutter and wait a while for polyfuse to reset or replace fuse.
 */
bool Cutter::isFuseblown() {
  return isCutting() && load < 2; // are we cutting without any load? That is not possible unless motor is standing still.
}

void Cutter::process() {
  // slowly ramp cutter speed up to reach target ("cutterSpeed"), this is to save fuses and electronics from current surges.
  if (cutterCurrentSpeed < cutterSpeed) {

    if (millis() - cutterLastSpeedRamp > 50) {
      cutterLastSpeedRamp = millis();
      cutterCurrentSpeed++;
      setCutterSpeed(cutterCurrentSpeed);
    }

  } else if (cutterCurrentSpeed > cutterSpeed) {

    cutterCurrentSpeed--;
    setCutterSpeed(cutterCurrentSpeed);

  }
}