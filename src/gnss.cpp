#include <ArduinoLog.h>
#include "gnss.h"
#include "definitions.h"

// RTK baserad GPS. Här finns karta över närliggande stationer: http://www.epncb.oma.be/_networkdata/data_access/real_time/map.php
// u-blox NEO-7N, ±6-10 meter
// u-blox NEO-8T, ±1-6 meter
// u-blox ZED-F9P, ±1 meter without RTCM, 1-2 centimeter when using RTCM signal.
//
// Information about RTK: https://gssc.esa.int/navipedia/index.php/Real_Time_Kinematics and //https://www.youtube.com/watch?v=n8PUyOtiGKo
// Useful RTK blog: http://rtkexplorer.com

//https://github.com/Ultimaker/CuraEngine/blob/master/src/slicer.h
//https://github.com/Ultimaker/CuraEngine/blob/master/src/slicer.cpp
//https://github.com/Ultimaker/CuraEngine/blob/master/src/infill.cpp

// OBS!
// Sparkfun rekommenderar att man inte kör några pull-ups på I2C bussen då kortet redan har egna motstånd för pullups! Och de rekommendeerar att man kör busseen i 100KHz hastighet! Stäng också av NMEA occh UART om vi inte behöver dom, för att avlast kortet.
///
GNSS::GNSS(TwoWire &w) : _Wire(w)
{
}

void GNSS::init()
{
  if (gnss.begin(_Wire) == false) //Connect to the Ublox module using Wire port
  {
    Log.warning(F("Ublox device not detected at default I2C address. Please check wiring, and restart mower!"));
    while (1)
      ;
  }

  Serial.print(F("Version: "));
  byte versionHigh = gnss.getProtocolVersionHigh();
  Serial.print(versionHigh);
  Serial.print(".");
  byte versionLow = gnss.getProtocolVersionLow();
  Serial.print(versionLow);

  // do not overload the buffer system from the device, disable UART output
  gnss.setUART1Output(0); //Disable the UART1 port output
  gnss.setUART2Output(0); //Disable Set the UART2 port output

  gnss.setI2COutput(COM_TYPE_UBX); // Set the I2C port to output UBX only (turn off NMEA noise)
  gnss.setNavigationFrequency(10); // Set output to 10 times a second
  gnss.setAutoPVT(true);           // Don't block for updates, library will internally serve the latest readings is has instead of waiting for new ones to finish.
  gnss.saveConfiguration();        // Save the current settings to flash and battery backed RAM

  byte rate = gnss.getNavigationFrequency();
  Serial.print("Current update rate:");
  Serial.println(rate);
}

void GNSS::start()
{
  // TODO: remove this code when done debugging, not needed since setAutoPVT(true)!
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    /* Note: Long/lat are large numbers because they are * 10^9. To convert lat/long
    to something google maps understands simply divide the numbers by 100,000,000. We
    do this so that we don't have to use floating point numbers. */
    int32_t latitude = gnss.getHighResLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    int32_t longitude = gnss.getHighResLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);

    int32_t altitude = gnss.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);

    int32_t speed = gnss.getGroundSpeed();
    Serial.print(F(" Speed: "));
    Serial.print(speed);
    Serial.print(F(" (mm/s)"));

    int32_t heading = gnss.getHeading();
    Serial.print(F(" Heading: "));
    Serial.print(heading);
    Serial.print(F(" (degrees * 10^-5)"));

    int pDOP = gnss.getPDOP();
    Serial.print(F(" pDOP: "));
    Serial.print(pDOP / 100.0, 2);

    byte fixType = gnss.getFixType();
    Serial.print(F(" Fix: "));
    if (fixType == 0)
      Serial.print(F("No fix"));
    else if (fixType == 1)
      Serial.print(F("Dead reckoning"));
    else if (fixType == 2)
      Serial.print(F("2D"));
    else if (fixType == 3)
      Serial.print(F("3D"));
    else if (fixType == 4)
      Serial.print(F("GNSS+Dead reckoning"));

    byte RTK = gnss.getCarrierSolutionType();
    Serial.print(" RTK: ");
    Serial.print(RTK);
    if (RTK == 1)
      Serial.print(F("High precision float fix!"));
    if (RTK == 2)
      Serial.print(F("High precision fix!"));
    Serial.println();

    int32_t accuracy = gnss.getPositionAccuracy();
    Serial.print(F(" 3D Positional Accuracy: "));
    Serial.print(accuracy);
    Serial.println(F("mm"));

    uint32_t horizontalAccuracy = gnss.getHorizontalAccuracy();
    Serial.print(F(" Horizontal accuracy: "));
    Serial.print(horizontalAccuracy);
    Serial.println(F("mm"));

    // https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library/blob/master/examples/Example13_PVT/Example1_AutoPVT/Example1_AutoPVT.ino
    // https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library/commit/63fb62ebd12c46c062d059c0fabe309f2d280098
    // https://overheadtransport.hostingerapp.com/autolawnmover/Index.htm
  }
}

const std::deque<gnssPosition> &GNSS::getGnssPositionHistory() const
{
  return gnssPosistionSamples;
}
