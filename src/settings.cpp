#include "settings.h"

namespace Settings {
    /*
      WiFi settings
    */
    const char* const SSID = "<your WiFi SSID>";
    const char* const WIFI_PASSWORD = "<your WiFi password>";
    const char* const OTA_PASSWORD = "<a OTA password>";

    /*
      MQTT settings (https://en.wikipedia.org/wiki/MQTT)
    */
    const char* const MQTT_SERVER = "<your MQTT servers IP-address>"; // leave this as it is or set it to an empty string if MQTT should not be used.
    const uint16_t MQTT_PORT = 1883;
    const std::string MQTT_TOPIC = "home/liam-esp";
    // Max number of messages awaiting to be sent to broker. Higher value consumes more RAM-memory but let us be disconnected from WiFi for a longer period.
    const uint16_t MQTT_QUEUE_LENGTH = 500;

    /*
      I2C pins
    */
    const uint8_t SDA_PIN = D2; // could be any free and suitable pin. We use default D2 here.
    const uint8_t SCL_PIN = D1; // could be any free and suitable pin. We use default D1 here.

    /*
      A/D converter I2C address
    */
    const uint8_t IO_ANALOG_ADDRESS = 0x48;

    /*
      Accelerometer
    */
    const uint8_t IO_ACCELEROMETER_ADDRESS = 0x53; // I2C address, set to "0" if not available.
    const uint8_t TILT_ANGLE_MAX = 60;    // Max angle (degrees) the mower is allowed to tilt, going above this value will stop mower and enter FLIPPED-state. This is a safety setting!

    /*
      Wheel motor settings
    */
     const uint8_t LEFT_WHEEL_MOTOR_PIN = D0;
     const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN = D3;
     const uint8_t LEFT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the right.
     const bool LEFT_WHEEL_MOTOR_INVERTED = false; // Set to "true" if left wheel runs backward when mower should be running forward.

     const uint8_t RIGHT_WHEEL_MOTOR_PIN = D7;
     const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN = D4;
     const uint8_t RIGHT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the left.
     const bool RIGHT_WHEEL_MOTOR_INVERTED = true;  // Set to "true" if right wheel runs backward when mower should be running forward.

     const uint8_t WHEEL_MOTOR_MIN_SPEED = 50;    // 0-100 (%), set the minimum speed that the wheel motors should use. This is used in combination with e.g. WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD.
     const uint8_t WHEEL_MOTOR_TURN_SPEED = 50;   // 0-100 (%), speed to use when turning.
     const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD = false;

    /*
      Cutter settings
    */
     const uint8_t CUTTER_MOTOR_PIN = D8;
     const uint8_t CUTTER_BRAKE_PIN = 2;     // Pin on digital port expander for braking cutter motor.
     const uint8_t CUTTER_LOAD_CHANNEL = 0;  // Channel on ADC for measuring cutter motor load.
     const uint8_t CUTTER_MAX_SPEED = 100;   // 0-100 (%), lower this value if cutter spinning too fast.
     // When the load on the cuttor motor surpasses this limit, the cutter is working too hard cutting the grass (and we should reduce speed of wheels to compensate).
     const uint16_t CUTTER_LOAD_THRESHOLD = 1000;

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
     const uint8_t BATTERY_SENSOR_CHANNEL = 1;    // Channel on ADC for measuring battery voltage.

     // Lithium-ion / LiPo  http://batteryuniversity.com/learn/article/types_of_lithium_ion
     // Normally a fully charged cell is 4.2 volt and quickly drops down to 3.7 volt, as battery is closing depleated it will drop from 3.7 down to 3.0 volt.
     // 3.0 volts should be considered an completely discharged cell, below that will damage the cell.
     const float BATTERY_FULLY_CHARGED = 16.8;        // in volt. e.g. 4.2 volt * 4 cells = 16.8 volt.
     const float BATTERY_EMPTY = 14.00;               // in volt. e.g. 3.0 volt * 4 cells = 12.0 volt and then we add a few volts to give us enough power to get us back to the charger.
     const float BATTERY_RESISTOR_DIVISOR = 20.0/170.0;   // Setting of RV2-trim resistor divided by size of RV2-trim resistor, e.g. 20/170 (kilo ohm). This is to read the correct battery voltage.

    // Nickel–metal hydride / NiMH example. http://batteryuniversity.com/learn/article/charging_nickel_metal_hydride
    // const float BATTERY_FULLY_CHARGED = 14.50;
    // const float BATTERY_EMPTY = 11.50;
    // const float BATTERY_RESISTOR_DIVISOR = ???;

    // Lead-acid example. http://www.solarnavigator.net/battery_charging.htm
    // const float BATTERY_FULLY_CHARGED = 13.30;
    // const float BATTERY_EMPTY = 11.90;
    // const float BATTERY_RESISTOR_DIVISOR = ???;

    /*
      Charger Settings
    */
    const uint8_t CHARGER_SENSOR_CHANNEL = 2;    // Channel on ADC for measuring charger voltage.
    const float CHARGER_RESISTOR_DIVISOR = 20.0/170.0; // Setting of RV1-trim resistor divided by size of RV1-trim resistor, e.g. 20/170 (kilo ohm). This is to read the correct charge voltage.

    /*
      Bumper sensor
    */
     const uint8_t BUMPER_SENSOR_PIN = 0;  // Pin on digital port expander for detecting obstacles (bumper), set to 0 if not used.

    /*
      GPS (Global Positioning System)
      Tested with GY-NEO6MV2 (https://www.openimpulse.com/blog/wp-content/uploads/wpsc/downloadables/GY-NEO6MV2-GPS-Module-Datasheet.pdf)
      Set GPS_RX_PIN and GPS_TX_PIN to "0" if not used!
    */
     const uint8_t GPS_RX_PIN = 3;  // D9 GPIO3 (RXD0)
     const uint8_t GPS_TX_PIN = 1;  // D10 GPIO1 (TDX0)
}
