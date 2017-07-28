#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "settings.h"
#include "resources.h"
#include "io_analog.h"
#include "io_digital.h"
#include "io_accelerometer.h"
#include "mqtt/mqtt.h"
#include "ota/ota.h"
#include "controller.h"
#include "wheel.h"
#include "cutter.h"
#include "bwf.h"
#include "battery.h"
#include "gps.h"

/*
 * Software to control a LIAM robot mower using a NodeMCU/ESP-12E (or similar ESP8266) microcontroller.
 */
IO_Analog io_analog;
IO_Digital io_digital;
IO_Accelerometer io_accelerometer;
MQTT_Client mqtt;
OTA ota(mqtt);
Wheel leftWheel(Settings::LEFT_WHEEL_MOTOR_PIN, Settings::LEFT_WHEEL_MOTOR_DIRECTION_PIN, Settings::LEFT_WHEEL_MOTOR_INVERED);
Wheel rightWheel(Settings::RIGHT_WHEEL_MOTOR_PIN, Settings::RIGHT_WHEEL_MOTOR_DIRECTION_PIN, Settings::RIGHT_WHEEL_MOTOR_INVERED);
Cutter cutter(io_analog);
BWF bwf;
Battery battery(io_analog);
GPS gps;
Resources resources(mqtt, leftWheel, rightWheel, cutter, bwf, battery, gps);
Controller controller(resources, io_accelerometer);

void scan_I2C() {
  Wire.begin(Settings::SDA_PIN, Settings::SCL_PIN);
  byte error, address;
  int devices = 0;

  Serial.println("Scanning for I2C devices...");

  for (address = 1; address < 127; address++ ) {

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(address, HEX);
      devices++;

    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(address, HEX);
    }
  }

  if (devices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("scanning done.");
  }
}

void setup_WiFi() {
  WiFi.hostname(Definitions::APP_NAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(Settings::SSID, Settings::WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi connection failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.print(Definitions::APP_NAME);
  Serial.print(" v");
  Serial.println(Definitions::APP_VERSION);

  scan_I2C();
  setup_WiFi();
  mqtt.connect();
}

void loop() {
  ota.handle();
  controller.run();
  // ESP.getCycleCount() // "returns the cpu instruction cycle count since start as an unsigned 32-bit."
  // ESP.getFreeHeap() // "returns the free heap size."
  yield();
}
