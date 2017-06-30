#include "settings.h"

/*
  4051 demultiplexer address pins
*/
const uint8_t Settings::DEMUX_PIN1 = 10; // "SD3"
const uint8_t Settings::DEMUX_PIN2 = 9;  // "SD2"

/*
  Wheel motor settings
*/
const uint8_t Settings::LEFT_WHEEL_MOTOR_PIN = D1;
const uint8_t Settings::LEFT_WHEEL_MOTOR_DIRECTION_PIN = D0;
const uint8_t Settings::RIGHT_WHEEL_MOTOR_PIN = D2;
const uint8_t Settings::RIGHT_WHEEL_MOTOR_DIRECTION_PIN = D3;
const uint8_t Settings::LEFT_WHEEL_MOTOR_SPEED = 100;  // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the right.
const uint8_t Settings::RIGHT_WHEEL_MOTOR_SPEED = 100; // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the left.
const uint8_t Settings::WHEEL_MOTOR_MIN_SPEED = 50;    // 0-100 (%), set the minimum speed that the wheel motors should use. This is used in combination with e.g. WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD.
const uint8_t Settings::WHEEL_MOTOR_TURN_SPEED = 50;   // 0-100 (%), speed to use when turning.
const bool Settings::WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD = false;

/*
  Cutter settings
*/
const uint8_t Settings::CUTTER_MOTOR_PIN = D4;
const uint8_t Settings::CUTTER_LOAD_DEMUX_ADDRESS = 2;  // Address on 4051 demultiplexer for measuring cutter motor load, set to 0 if not used.
const uint8_t Settings::CUTTER_MAX_SPEED = 100; // 0-100 (%), lower this value if cutter spinning too fast.

/*
  Border Wire Fence settings
*/
const uint8_t Settings::LEFT_BWF_PIN = D5;
const uint8_t Settings::RIGHT_BWF_PIN = D6;
// How long the mower should be running without hitting the border wire, in seconds.
// This is a safety setting that prevent the mower to escape too far away.
// If the mower react this time setting before it react the border fence it will stop and enter the STUCK-state.
const uint32_t Settings::BWF_SENSE_TIMEOUT = 3 * 60;

/*
  Battery settings
*/
const Definitions::BATTERY_TYPE Settings::battery_type = Definitions::BATTERY_TYPE.LIION; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.cpp).
const uint8_t Settings::BATTERY_SENSOR_DEMUX_ADDRESS = 1;    // Address on 4051 demultiplexer for measuring battery voltage.
const uint16_t Settings::BATTERY_FULLY_CHARGED = 1560;       // Battery-voltage multiplied with 10 to avoid decimal values. e.g. 15,60 volts -> 1560.
const uint16_t Settings::BATTERY_EMPTY = 1400;               // Battery-voltage multiplied with 10 to avoid decimal values. e.g. 14,00 volts -> 1400.

// Nickel–metal hydride example. http://www.solarnavigator.net/battery_charging.htm
//const Definitions::BATTERY_TYPE Settings::battery_type = Definitions::BATTERY_TYPE.NIMH; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.cpp).
//const uint16_t Settings::BATTERY_FULLY_CHARGED = 1450;
//const uint16_t Settings::BATTERY_EMPTY = 1150;


// Lead-acid example. http://www.solarnavigator.net/battery_charging.htm
//const Definitions::BATTERY_TYPE Settings::battery_type = Definitions::BATTERY_TYPE.LEAD_ACID; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.cpp).
//const uint16_t Settings::BATTERY_FULLY_CHARGED = 1330;
//const uint16_t Settings::BATTERY_EMPTY = 1190;

/*
  Tilt sensor
*/
const uint8_t Settings::TILT_SENSOR_DEMUX_ADDRESS = 3;    // Address on 4051 demultiplexer for measuring inclination (tilting) of mower.
const uint8_t Settings::TILT_ANGLE_MAX = 60;    // Max angle (degrees) the mower is allowed to tilt, going above this value will stop mower and enter STUCK-state. This is a safety setting!

/*
  Bumper sensor
*/
const uint8_t Settings::BUMPER_SENSOR_DEMUX_ADDRESS = 4;  // Address on 4051 demultiplexer for detecting obstacles (bumper), set to 0 if not used.

/*
  GPS (Global Positioning System)
  Tested with GY-NEO6MV2 (https://www.openimpulse.com/blog/wp-content/uploads/wpsc/downloadables/GY-NEO6MV2-GPS-Module-Datasheet.pdf)
*/
const uint8_t Settings::GPS_RX_PIN = D7;
const uint8_t Settings::GPS_TX_PIN = D8;

/*
  WiFi settings
*/
const char* Settings::SSID = "<your WiFi SSID";
const char* Settings::WIFI_PASSWORD = "<your WiFi password>";
const char* Settings::OTA_PASSWORD = "<a OTA password>";

/*
  MQTT settings (https://en.wikipedia.org/wiki/MQTT)
*/
const char* Settings::MQTT_SERVER = "<your MQTT servers IP-address>";
const uint16_t Settings::MQTT_PORT = 1883;
const char Settings::MQTT_TOPIC[] = "home/liam-esp";
