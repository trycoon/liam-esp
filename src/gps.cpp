#include "gps.h"
#include "settings.h"

//TODO: read this: https://github.com/SlashDevin/NeoGPS/issues/8
// implement the async ISR version using this example: https://github.com/SlashDevin/NeoGPS/blob/master/examples/NMEA_isr/NMEA_isr.ino
// since we do not have a tight loop, we need the interrupt driven version to be sure not loosing any GPS data.

GPS::GPS() {
  if (isEnabled()) {
    //Serial1.begin(9600, SERIAL_8N1);
  }

  //gps_port.attachInterrupt(GPS_isr);
  //gps_port.begin(9600);
}

bool GPS::isEnabled() {
  return Settings::GPS_RX_PIN > 0 && Settings::GPS_TX_PIN > 0;
}

void GPS::GPS_isr(uint8_t c) {
  gps.handle(c);
}

/*void readGPS() {
  if (gps.available()) {
    currentFix = gps.read();
  }
}*/
