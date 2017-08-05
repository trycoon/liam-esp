#include "gps.h"
#include "settings.h"

GPS::GPS() {
  if (isAvailable()) {
    Serial.begin(9600, SERIAL_8N1);
  }
}

bool GPS::isAvailable() {
  return Settings::GPS_RX_PIN > 0 && Settings::GPS_TX_PIN > 0;
}

void GPS::getPosition() {

}
