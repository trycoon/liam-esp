#ifndef _gps_h
#define _gps_h

#include <Arduino.h>
#include <TinyGPS++.h>

class GPS {
  public:
    GPS();
    bool isAvailable();
    void getPosition();

  private:
    TinyGPSPlus gps;
};

#endif
// http://arduiniana.org/libraries/tinygpsplus/

/*
Serial uses UART0, which is mapped to pins GPIO1 (TX) and GPIO3 (RX).
Serial may be remapped to GPIO15 (TX) and GPIO13 (RX) by calling Serial.swap() after Serial.begin. Calling swap again maps UART0 back to GPIO1 and GPIO3.
*/

/*
#include <TinyGPS++.h>
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;

void setup() {
  Serial1.begin(115200);
  Serial.begin(GPSBaud, SERIAL_8N1);

  Serial1.println("GPS example");
  Serial1.println(TinyGPSPlus::libraryVersion());
}


static char sentence[80],c;  // NMEA sentence from external device
static int line= 0; // data available flag
static int i=0;
static long timer=0;

void loop() {
  readGPS();
}

void readGPS(){
  int j;
  if (Serial.available() > 0) {
    c=Serial.read();
    sentence[i]=c;
    ++i;
    if (c== 0x0d) line=1;
  }
  if (i > 79) i=79;  // line too long
  if (line) {
    for (j=0; j<i;++j) {
      //Serial1.print(sentence[j]);
    }
    Serial1.println(sentence);
    gps.encode(*sentence);
    displayInfo();

    line=i=0; // reset the sentence buffer
  }
}


void displayInfo()
{
  Serial1.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial1.print(gps.location.lat(), 6);
    Serial1.print(F(","));
    Serial1.print(gps.location.lng(), 6);
  }
  else
  {
    Serial1.print(F("INVALID"));
  }

  Serial1.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial1.print(gps.date.month());
    Serial1.print(F("/"));
    Serial1.print(gps.date.day());
    Serial1.print(F("/"));
    Serial1.print(gps.date.year());
  }
  else
  {
    Serial1.print(F("INVALID"));
  }

  Serial1.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial1.print(F("0"));
    Serial1.print(gps.time.hour());
    Serial1.print(F(":"));
    if (gps.time.minute() < 10) Serial1.print(F("0"));
    Serial1.print(gps.time.minute());
    Serial1.print(F(":"));
    if (gps.time.second() < 10) Serial1.print(F("0"));
    Serial1.print(gps.time.second());
    Serial1.print(F("."));
    if (gps.time.centisecond() < 10) Serial1.print(F("0"));
    Serial1.print(gps.time.centisecond());
  }
  else
  {
    Serial1.print(F("INVALID"));
  }

  Serial1.println();
} - See more at: http://www.esp8266.com/viewtopic.php?f=32&t=3829#sthash.btpkuOHB.dpuf
*/
