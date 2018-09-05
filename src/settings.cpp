#include "settings.h"

/*
* Global settings and pin-assignments.
*/

namespace Settings {

    /*
      I2C pins
    */
    const uint8_t SDA_PIN = 21; // could be any free and suitable pin. We use default 21 here.
    const uint8_t SCL_PIN = 22; // could be any free and suitable pin. We use default 22 here.

    /*
      Generic motor PWM settings
    */
    const uint8_t MOTOR_TIMER_13_BIT = 13;  // use 13 bit precission for MOTOR timer
    const uint16_t MOTOR_BASE_FREQ = 5000;   // use 5000 Hz as a MOTOR base frequency

    /*
      Accelerometer/Gyro/Compass
    */
    const uint8_t TILT_ANGLE_MAX = 35;    // Max angle (degrees) the mower is allowed to tilt, going above this value will stop mower and enter FLIPPED-state. This is a safety setting!
    const uint8_t IO_ACCELEROMETER_INT_PIN = 23;  // Interrupt pin signaling when new data is available from Accelerometer/Gyro/Compass

    /*
      Wheel motor settings
    */
     const uint8_t LEFT_WHEEL_MOTOR_PIN = 19;
     const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN = 18;
     const uint8_t LEFT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the right.
     const bool LEFT_WHEEL_MOTOR_INVERTED = false; // Set to "true" if left wheel runs backward when mower should be running forward.

     const uint8_t RIGHT_WHEEL_MOTOR_PIN = 17;
     const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN = 16;
     const uint8_t RIGHT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the left.
     const bool RIGHT_WHEEL_MOTOR_INVERTED = true;  // Set to "true" if right wheel runs backward when mower should be running forward.

     const uint8_t WHEEL_MOTOR_MIN_SPEED = 30;    // 0-100 (%), set the minimum speed that the wheel motors should use. This is used in combination with e.g. WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD.
     const uint8_t WHEEL_MOTOR_TURN_SPEED = 50;   // 0-100 (%), speed to use when turning.
     const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD = false;

    /*
      Cutter settings
    */
     const uint8_t CUTTER_MOTOR_PIN = 0;    // Pin to PWM-control motor.
     const uint8_t CUTTER_BRAKE_PIN = 4;    // Pin for braking cutter motor.
     const uint8_t CUTTER_LOAD_PIN = 36;    // Pin on ADC for measuring cutter motor load.
     const float CUTTER_LOAD_RESISTOR_MULTIPLIER = 20.0/170.0;   // Setting of RV3-trim resistor divided by size of RV3-trim resistor, e.g. 20/170 (kilo ohm). This is to read the correct cutter load.
     const uint8_t CUTTER_MAX_SPEED = 100;  // 0-100 (%), lower this value if cutter spinning too fast.
     // When the load on the cuttor motor surpasses this limit, the cutter is working too hard cutting the grass (and we should reduce speed of wheels to compensate).
     const uint16_t CUTTER_LOAD_THRESHOLD = 1000;

    /*
      Border Wire Fence settings
    */
     const uint8_t LEFT_BWF_PIN = 34;
     const uint8_t RIGHT_BWF_PIN = 35;
    // How long the mower should be running without hitting the border wire, in seconds.
    // This is a safety setting that prevent the mower to escape too far away.
    // If the mower react this time setting before it react the border fence it will stop and enter the STUCK-state.
     const uint32_t BWF_SENSE_TIMEOUT = 3 * 60;

    /*
      Battery settings
    */
     const uint8_t BATTERY_SENSOR_PIN = 39;    // Pin on ADC for measuring battery voltage.

     // Lithium-ion / LiPo  http://batteryuniversity.com/learn/article/types_of_lithium_ion
     // Normally a fully charged cell is 4.2 volt and quickly drops down to 3.7 volt, as battery is closing depleated it will drop from 3.7 down to 3.0 volt.
     // 3.0 volts should be considered an completely discharged cell, below that will damage the cell.
     const float BATTERY_FULLY_CHARGED = 16.8;        // in volt. e.g. 4.2 volt * 4 cells = 16.8 volt.
     const float BATTERY_EMPTY = 14.00;               // in volt. e.g. 3.0 volt * 4 cells = 12.0 volt and then we add a few volts to give us enough power to get us back to the charger.
     const float BATTERY_MULTIPLIER = BATTERY_FULLY_CHARGED / 1.1;   // Battery voltage divided by ADC max value, used to calculate real battery voltage.

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
    const uint8_t CHARGER_SENSOR_PIN = 32;    // Pin on ADC for measuring charger voltage.
    const float CHARGER_MULTIPLIER = 24.0 / 1.1; // Charger voltage divided by ADC max value, used to calculate real charger voltage.

    /*
      Bumper sensor
    */
     const uint8_t BUMPER_SENSOR_PIN = 0;  // Pin on digital port expander for detecting obstacles (bumper), set to 0 if not used.

    /*
      GPS (Global Positioning System)
      Tested with GY-NEO6MV2 (https://www.openimpulse.com/blog/wp-content/uploads/wpsc/downloadables/GY-NEO6MV2-GPS-Module-Datasheet.pdf)
      Set GPS_RX_PIN and GPS_TX_PIN to "0" if not used!
    */
     const uint8_t GPS_RX_PIN = 3;  // GPIO3 (RXD0)
     const uint8_t GPS_TX_PIN = 1;  // GPIO1 (TDX0)

    // Max number of messages awaiting to be sent to broker. Higher value consumes more RAM-memory but let us be disconnected from WiFi for a longer period.
    const uint16_t MQTT_QUEUE_LENGTH = 50;
}
