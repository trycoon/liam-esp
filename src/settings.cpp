#include "settings.h"

namespace Settings {
    /*
      4051 demultiplexer address pins
    */
     const uint8_t DEMUX_PIN1 = 10; // "SD3"
     const uint8_t DEMUX_PIN2 = 9;  // "SD2"

    /*
      Wheel motor settings
    */
     const uint8_t LEFT_WHEEL_MOTOR_PIN = D1;
     const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN = D0;
     const uint8_t LEFT_WHEEL_MOTOR_SPEED = 100;  // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the right.
     const bool LEFT_WHEEL_MOTOR_INVERED = false; // Set to "true" if left wheel runs backward when mower should be running forward.

     const uint8_t RIGHT_WHEEL_MOTOR_PIN = D2;
     const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN = D3;
     const uint8_t RIGHT_WHEEL_MOTOR_SPEED = 100;  // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the left.
     const bool RIGHT_WHEEL_MOTOR_INVERED = false; // Set to "true" if right wheel runs backward when mower should be running forward.

     const uint8_t WHEEL_MOTOR_MIN_SPEED = 50;    // 0-100 (%), set the minimum speed that the wheel motors should use. This is used in combination with e.g. WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD.
     const uint8_t WHEEL_MOTOR_TURN_SPEED = 50;   // 0-100 (%), speed to use when turning.
     const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD = false;

    /*
      Cutter settings
    */
     const uint8_t CUTTER_MOTOR_PIN = D4;
     const uint8_t CUTTER_LOAD_DEMUX_ADDRESS = 2;  // Address on 4051 demultiplexer for measuring cutter motor load, set to 0 if not used.
     const uint8_t CUTTER_MAX_SPEED = 100; // 0-100 (%), lower this value if cutter spinning too fast.

    /*
      Border Wire Fence settings
    */
     const uint8_t LEFT_BWF_PIN = D5;
     const uint8_t RIGHT_BWF_PIN = D6;
    // How long the mower should be running without hitting the border wire, in seconds.
    // This is a safety setting that prevent the mower to escape too far away.
    // If the mower react this time setting before it react the border fence it will stop and enter the STUCK-state.
     const uint32_t BWF_SENSE_TIMEOUT = 3 * 60;

    /*
      Battery settings
    */
     const uint8_t BATTERY_SENSOR_DEMUX_ADDRESS = 1;    // Address on 4051 demultiplexer for measuring battery voltage.

     const Definitions::BATTERY_TYPE battery_type = Definitions::BATTERY_TYPE::LIION; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.cpp).
     const uint16_t BATTERY_FULLY_CHARGED = 1560;       // Battery-voltage multiplied with 10 to avoid decimal values. e.g. 15,60 volts -> 1560.
     const uint16_t BATTERY_EMPTY = 1400;               // Battery-voltage multiplied with 10 to avoid decimal values. e.g. 14,00 volts -> 1400.

    // Nickel–metal hydride example. http://www.solarnavigator.net/battery_charging.htm
    // const Definitions::BATTERY_TYPE battery_type = Definitions::BATTERY_TYPE.NIMH; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.cpp).
    // const uint16_t BATTERY_FULLY_CHARGED = 1450;
    // const uint16_t BATTERY_EMPTY = 1150;


    // Lead-acid example. http://www.solarnavigator.net/battery_charging.htm
    // const Definitions::BATTERY_TYPE battery_type = Definitions::BATTERY_TYPE.LEAD_ACID; // Important! Set to the correct type of battery you use (see BATTERY_TYPE in definitions.cpp).
    // const uint16_t BATTERY_FULLY_CHARGED = 1330;
    // const uint16_t BATTERY_EMPTY = 1190;

    /*
      Tilt sensor
    */
     const uint8_t TILT_SENSOR_DEMUX_ADDRESS = 3;    // Address on 4051 demultiplexer for measuring inclination (tilting) of mower.
     const uint8_t TILT_ANGLE_MAX = 60;    // Max angle (degrees) the mower is allowed to tilt, going above this value will stop mower and enter STUCK-state. This is a safety setting!

    /*
      Bumper sensor
    */
     const uint8_t BUMPER_SENSOR_DEMUX_ADDRESS = 4;  // Address on 4051 demultiplexer for detecting obstacles (bumper), set to 0 if not used.

    /*
      GPS (Global Positioning System)
      Tested with GY-NEO6MV2 (https://www.openimpulse.com/blog/wp-content/uploads/wpsc/downloadables/GY-NEO6MV2-GPS-Module-Datasheet.pdf)
    */
     const uint8_t GPS_RX_PIN = D7;
     const uint8_t GPS_TX_PIN = D8;

    /*
      WiFi settings
    */
     const char* const SSID = "<your WiFi SSID";
     const char* const WIFI_PASSWORD = "<your WiFi password>";
     const char* const OTA_PASSWORD = "<a OTA password>";

    /*
      MQTT settings (https://en.wikipedia.org/wiki/MQTT)
    */
     const char* const MQTT_SERVER = "<your MQTT servers IP-address>";
     const uint16_t MQTT_PORT = 1883;
     const char* const MQTT_TOPIC = "home/liam-esp";

}
