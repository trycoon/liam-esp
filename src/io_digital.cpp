#include <Wire.h>
#include "io_digital.h"
#include "settings.h"
#include "Arduino.h"

IO_Digital::IO_Digital() {
  Wire.begin(Settings::SDA_PIN, Settings::SCL_PIN);
}

bool IO_Digital::digitalWrite(uint8_t pin, bool state) {
  uint16_t pinstates = 0;

  // turn pin HIGH or LOW in 16-bit register
  if(state == HIGH) {
    pinstates |= 1 << pin;
  } else {
    pinstates &= ~(1 << pin);
  }

  return TWI_SUCCESS == device.digitalWrite(pinstates);
}
