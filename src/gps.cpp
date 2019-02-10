#include <ArduinoLog.h>
#include "gps.h"
#include "definitions.h"

// RTK baserad GPS. Här finns karta över närliggande stationer: http://www.epncb.oma.be/_networkdata/data_access/real_time/map.php
// Köp denna: https://www.ardusimple.com/product/simplertk2b-starter-kit-mr/
// u-blox NEO-7N, ±6-10 meter
// u-blox NEO-8T, ±1-6 meter
// u-blox ZED-F9P, ±1 meter without RTCM, 1-2 centimeter when using RTCM signal. 
// More info about RTK: https://gssc.esa.int/navipedia/index.php/Real_Time_Kinematics
// useful RTK blogg: http://rtkexplorer.com

//https://www.youtube.com/watch?v=n8PUyOtiGKo
//https://github.com/vasimv/rtkzedf9p

//https://github.com/Ultimaker/CuraEngine/blob/master/src/slicer.h
//https://github.com/Ultimaker/CuraEngine/blob/master/src/slicer.cpp
//https://github.com/Ultimaker/CuraEngine/blob/master/src/infill.cpp
// C++ to Javascript transpilator: http://kripken.github.io/emscripten-site/docs/getting_started/Tutorial.html

GPS::GPS() : gpsSerial(2), gps(gpsSerial) {}

bool GPS::isEnabled() {
  return Definitions::GPS_RX_PIN > 0 && Definitions::GPS_TX_PIN > 0;
}

void GPS::init() {
  // Most of the following init-code have been grabbed from here: https://github.com/loginov-rocks/UbxGps/blob/master/extras/Configuration/Auto-configuration-Mega/Auto-configuration-Mega.ino
  if (isEnabled()) {
    
    // Restore the receiver default configuration, just to get us to a known state.
    for (byte i = 0; i < sizeof(possibleBaudrates) / sizeof(*possibleBaudrates); i++) {
        Log.trace(F("Trying to restore GPS module to defaults at %d baudrate." CR), possibleBaudrates[i]);

        if (i != 0) {
            delay(100);
            gpsSerial.flush();
        }

        gpsSerial.begin(possibleBaudrates[i], SERIAL_8N1, Definitions::GPS_RX_PIN, Definitions::GPS_TX_PIN);
        restoreDefaults();
    }

    Log.trace(F("Switched GPS module to the default baudrate: %d." CR), GPS_DEFAULT_BAUDRATE);
    delay(100);
    gpsSerial.flush();
    gpsSerial.begin(GPS_DEFAULT_BAUDRATE, SERIAL_8N1, Definitions::GPS_RX_PIN, Definitions::GPS_TX_PIN);

    // Disable unnecessary NMEA messages from GPS.
    Log.trace(F("Disabling NMEA messages..." CR));
    disableNmea();

    // Switch the receiver serial to the wanted baudrate.
    if (GPS_BAUDRATE != GPS_DEFAULT_BAUDRATE) {
        Log.trace(F("Switching GPS module to the wanted baudrate: %d." CR), GPS_BAUDRATE);

        changeBaudrate();

        delay(100);
        gpsSerial.flush();
        gpsSerial.begin(GPS_BAUDRATE, SERIAL_8N1, Definitions::GPS_RX_PIN, Definitions::GPS_TX_PIN);
    }

    // Increase receiver frequency to 100 ms (10 Hz).
    Log.trace(F("Changing GPS receiver frequency to 10 Hz (100 ms)..." CR));
    changeFrequency();

    // Enable NAV-PVT messages.
    Log.trace(F("Enabling GPS receiver NAV-PVT messages..." CR));
    enableNavPvt();

    Log.notice(F("GPS receiver configuration complete." CR));

    delay(100);
    gpsSerial.flush();
  }
}

void GPS::start() {
  if (isEnabled()) {
    if (gps.ready()) {
        /*snprintf(datetime, DATETIME_LENGTH, DATETIME_FORMAT, gps.year, gps.month, gps.day, gps.hour, gps.min, gps.sec);

        Serial.print(datetime);
        Serial.print(',');*/
        Serial.print(gps.lon / 10000000.0, 7);
        Serial.print(',');
        Serial.print(gps.lat / 10000000.0, 7);
        Serial.print(',');
        Serial.print(gps.height / 1000.0, 3);
        Serial.print(',');
        Serial.print(gps.gSpeed * 0.0036, 5);
        Serial.print(',');
        Serial.print(gps.heading / 100000.0, 5);
        Serial.print(',');
        Serial.print(gps.fixType);
        Serial.print(',');
        Serial.println(gps.numSV);
    }
  }
}

// Send a packet to the receiver to restore default configuration.
void GPS::restoreDefaults() {
    // CFG-CFG packet.
    byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x09, // id
        0x0D, // length
        0x00, // length
        0xFF, // payload
        0xFF, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xFF, // payload
        0xFF, // payload
        0x00, // payload
        0x00, // payload
        0x17, // payload
        0x2F, // CK_A
        0xAE, // CK_B
    };

    sendPacket(packet, sizeof(packet));
}

// Send a set of packets to the receiver to disable NMEA messages.
void GPS::disableNmea() {
    // Array of two bytes for CFG-MSG packets payload.
    byte messages[][2] = {
        {0xF0, 0x0A},
        {0xF0, 0x09},
        {0xF0, 0x00},
        {0xF0, 0x01},
        {0xF0, 0x0D},
        {0xF0, 0x06},
        {0xF0, 0x02},
        {0xF0, 0x07},
        {0xF0, 0x03},
        {0xF0, 0x04},
        {0xF0, 0x0E},
        {0xF0, 0x0F},
        {0xF0, 0x05},
        {0xF0, 0x08},
        {0xF1, 0x00},
        {0xF1, 0x01},
        {0xF1, 0x03},
        {0xF1, 0x04},
        {0xF1, 0x05},
        {0xF1, 0x06},
    };

    // CFG-MSG packet buffer.
    byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x01, // id
        0x03, // length
        0x00, // length
        0x00, // payload (first byte from messages array element)
        0x00, // payload (second byte from messages array element)
        0x00, // payload (not changed in the case)
        0x00, // CK_A
        0x00, // CK_B
    };

    byte packetSize = sizeof(packet);

    // Offset to the place where payload starts.
    byte payloadOffset = 6;

    // Iterate over the messages array.
    for (byte i = 0; i < sizeof(messages) / sizeof(*messages); i++) {
        // Copy two bytes of payload to the packet buffer.
        for (byte j = 0; j < sizeof(*messages); j++) {
            packet[payloadOffset + j] = messages[i][j];
        }

        // Set checksum bytes to the null.
        packet[packetSize - 2] = 0x00;
        packet[packetSize - 1] = 0x00;

        // Calculate checksum over the packet buffer excluding sync (first two) and checksum chars (last two).
        for (byte j = 0; j < packetSize - 4; j++) {
            packet[packetSize - 2] += packet[2 + j];
            packet[packetSize - 1] += packet[packetSize - 2];
        }

        sendPacket(packet, packetSize);
    }
}

// Send a packet to the receiver to change baudrate to 115200.
void GPS::changeBaudrate() {
    // CFG-PRT packet.
    byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x00, // id
        0x14, // length
        0x00, // length
        0x01, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xD0, // payload
        0x08, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xC2, // payload
        0x01, // payload
        0x00, // payload
        0x07, // payload
        0x00, // payload
        0x03, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xC0, // CK_A
        0x7E, // CK_B
    };

    sendPacket(packet, sizeof(packet));
}

// Send a packet to the receiver to change frequency to 100 ms.
void GPS::changeFrequency() {
    // CFG-RATE packet.
    byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x08, // id
        0x06, // length
        0x00, // length
        0x64, // payload
        0x00, // payload
        0x01, // payload
        0x00, // payload
        0x01, // payload
        0x00, // payload
        0x7A, // CK_A
        0x12, // CK_B
    };

    sendPacket(packet, sizeof(packet));
}

// Send a packet to the receiver to enable NAV-PVT messages.
void GPS::enableNavPvt() {
    // CFG-MSG packet.
    byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x01, // id
        0x03, // length
        0x00, // length
        0x01, // payload
        0x07, // payload
        0x01, // payload
        0x13, // CK_A
        0x51, // CK_B
    };

    sendPacket(packet, sizeof(packet));
}

// Send the packet specified to the receiver.
void GPS::sendPacket(const byte *packet, byte len) {
    for (byte i = 0; i < len; i++) {
        gpsSerial.write(packet[i]);
    }
}

const std::deque<gpsPosition>& GPS::getGpsPositionHistory() const {
  return gpsPosistionSamples;
}
