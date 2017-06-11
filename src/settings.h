#ifndef settings_h
#define settings_h

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <definitions.h>


/*
  Wheel motor settings
*/
const uint8_t LEFT_WHEEL_MOTOR_PIN = D1;
const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN = D2;
const uint8_t RIGHT_WHEEL_MOTOR_PIN = D3;
const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN = D4;
const uint8_t LEFT_WHEEL_MOTOR_SPEED = 100;  // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the right.
const uint8_t RIGHT_WHEEL_MOTOR_SPEED = 100; // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the left.
const uint8_t WHEEL_MOTOR_MIN_SPEED = 50;    // 0-100 (%), set the minimum speed that the wheel motors should use. This is used in combination with e.g. WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD.
const uint8_t WHEEL_MOTOR_TURN_SPEED = 50;   // 0-100 (%), speed to use when turning.
const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD = false;

/*
  Cutter settings
*/
const uint8_t CUTTER_MOTOR_PIN = D5;
const uint8_t CUTTER_LOAD_PIN = 0;    // pin to use to measure load of cutter motor, set to 0 if not used.
const uint8_t CUTTER_MAX_SPEED = 100; // 0-100 (%), lower this value if cutter spinning too fast.

/*
  Border Wire Fence settings
*/
const uint8_t LEFT_BWF_PIN = D6;
const uint8_t RIGHT_BWF_PIN = D7;
// How long the mower should be running without hitting the border wire, in milliseconds.
// This is a safety setting that prevent the mower to escape too far away.
// If the mower react this time setting before it react the border fence it will stop and enter the STUCK-state.
const uint32_t BWF_SENSE_TIMEOUT = 3 * 60 * 1000;

/*
  Battery settings
*/
const uint8_t BATTERY_SENSOR_PIN = A0;    // pin to sensor that monitor the voltage of the battery.
const BATTERY_TYPE battery_type = BATTERY_TYPE::LIPO; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.h).
const uint16_t BATTERY_FULLY_CHARGED = 1560;
const uint16_t BATTERY_EMPTY = 1400;

// Nickel–metal hydride example. http://www.solarnavigator.net/battery_charging.htm
//const BATTERY_TYPE battery_type = BATTERY_TYPE::NIMH; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.h).
//const uint16_t BATTERY_FULLY_CHARGED = 1450;
//const uint16_t BATTERY_EMPTY = 1150;


// Lead-acid example. http://www.solarnavigator.net/battery_charging.htm
//const BATTERY_TYPE battery_type = BATTERY_TYPE::LEAD_ACID; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.h).
//const uint16_t BATTERY_FULLY_CHARGED = 1330;
//const uint16_t BATTERY_EMPTY = 1190;

/*
  Tilt sensor
*/
const uint8_t TILT_ANGLE_MAX = 60;    // Max angle (degrees) the mower is allowed to tilt, going above this value will stop mower and enter STUCK-state. This is a safety setting!

/*
  WiFi settings
*/
const char* SSID = "<your WiFi SSID";
const char* WIFI_PASSWORD = "<your WiFi password>";
const char* OTA_PASSWORD = "<a OTA password>";

/*
  MQTT settings (https://en.wikipedia.org/wiki/MQTT)
*/
const char* MQTT_SERVER = "<your MQTT servers IP-address>";
const uint16_t MQTT_PORT = 1883;
const char MQTT_TOPIC[] = "home/liam-esp";

#endif
