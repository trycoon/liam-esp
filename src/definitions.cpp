#include "definitions.h"

//
// NOTE! If changing pin definitions, please read up on pins that could be used on the ESP32 (some are special): https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
//

namespace Definitions
{
  /*
  * Constants and other global stuff that you should probably only need to adjust once.
  */
  const char* const APP_NAME = "liam-esp";
  const char* const APP_VERSION = "1.0.0";
 
  /*
    I2C pins
  */
  const uint8_t SDA_PIN = 21; // could be any free and suitable pin. We use default 21 here.
  const uint8_t SCL_PIN = 22; // could be any free and suitable pin. We use default 22 here.

  /*
    I2C address for MCP23017 port expander
  */
  const uint16_t DIGITAL_EXPANDER_ADDR = 0x20;
  const uint16_t DIGITAL_EXPANDER_INTERRUPT_PIN = 34;

  /*
    I2C address for Analog to Digital Converters
  */
  const uint16_t ADC1_ADDR = 0x48;
  const uint16_t ADC2_ADDR = 0x49;
  
  /*
    Frequency used for LoRa radio module, please select value based upon what LoRa module you are using and your country regulation:
    https://github.com/jgromes/LoRaLib/wiki/Supported-LoRa-modules
  */
  const float LORA_FREQ = 434.0;  // (in Megahertz)
  /*
    LoRa interrupt pins
  */
  const uint8_t LORA_DIO0_PIN = 17;
  const uint8_t LORA_DIO1_PIN = 33;

  /*
    Generic motor PWM settings
  */
  const uint8_t MOTOR_TIMER_13_BIT = 13;  // use 13 bit precission for MOTOR timer
  const uint16_t MOTOR_BASE_FREQ = 5000;  // use 5000 Hz as a MOTOR base frequency

  /*
    Accelerometer/Gyro/Compass
  */
  const uint8_t TILT_ANGLE_MAX = 35;    // Max angle (degrees) the mower is allowed to tilt, going above this value will stop mower and enter FLIPPED-state. This is a safety setting!
  
  /*
    Wheel motor settings
  */
  const uint8_t LEFT_WHEEL_MOTOR_PIN = 27;
  const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN = 25;
  const uint8_t LEFT_WHEEL_MOTOR_LOAD_CHANNEL = 1;  // Channel on ADC for measuring motor load.
  const uint8_t LEFT_WHEEL_ODOMETER_PIN = 5;  // TODO: Remove this?
  const uint8_t LEFT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the right.
  const bool LEFT_WHEEL_MOTOR_INVERTED = false; // Set to "true" if left wheel runs backward when mower should be running forward.

  const uint8_t RIGHT_WHEEL_MOTOR_PIN = 32;
  const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN = 26;
  const uint8_t RIGHT_WHEEL_MOTOR_LOAD_CHANNEL = 2;  // Channel on ADC for measuring motor load.
  const uint8_t RIGHT_WHEEL_ODOMETER_PIN = 4;  // TODO: Remove this?
  const uint8_t RIGHT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the left.
  const bool RIGHT_WHEEL_MOTOR_INVERTED = true;  // Set to "true" if right wheel runs backward when mower should be running forward.

  const uint8_t WHEEL_MOTOR_MIN_SPEED = 20;    // 0-100 (%), set the minimum speed that the wheel motors should use. This is used in combination with e.g. WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD.
  const uint8_t WHEEL_MOTOR_TURN_SPEED = 40;   // 0-100 (%), speed to use when turning.
  const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD = true;  // reduce forward movement of mower when there is a high load on cutter (like high grass)
  const uint16_t WHEEL_ODOMETERPULSES_PER_ROTATION = 90;        // number of odometer pulses from motor that equals a full rotation of the shaft (check with motor manufacturer).
  const uint8_t WHEEL_DIAMETER = 190;     // in millimeter
  const uint8_t WHEEL_PAIR_DISTANCE = 27; // distance measured between the wheel pairs, in centimeters
  
  /*
    Settings for "launching" mower from charging station
  */
  const uint8_t LAUNCH_DISTANCE = 100;  // the distance the mower should back out of the charging station before turning around and begin mowing, in centimeters.

  /*
    Cutter settings
  */
  const uint8_t CUTTER_MOTOR_PIN = 2;      // Pin to PWM-control motor.
  const uint8_t CUTTER_BRAKE_PIN = 4;      // Pin for braking cutter motor.
  const uint8_t CUTTER_LOAD_CHANNEL = 0;   // Channel on ADC for measuring cutter motor load.
  const float CUTTER_LOAD_RESISTOR = 0.1;  // Size of shunt resistor connected in serial with cutter motor, in Ohm.
  const float CUTTER_NOLOAD_CURRENT = 200; // Milliampere of cutter motor when no load is applied, see motor specs for no-load current or find a suitable value yourself. (used for calculating cutter load percentage)
  const float CUTTER_MAX_CURRENT = 3100;   // Max milliampere of cutter motor, see motor specs for stall current. (used to calculate cutter load percentage)
  const uint8_t CUTTER_MAX_SPEED = 100;    // 1-100 (%), lower this value if cutter spinning too fast.
  // When the load on the cutter motor surpasses this limit, the cutter is working too hard cutting the grass (and we should reduce speed of wheels to compensate).
  const uint16_t CUTTER_LOAD_THRESHOLD = 80;

  /*
    Battery settings
  */

  // Lithium-ion / LiPo  http://batteryuniversity.com/learn/article/types_of_lithium_ion
  // Normally a fully charged cell is 4.2 volt and quickly drops down to 3.6 volt, as battery is closing depleated it will drop from 3.6 down to 2.5 volt.
  // 2.5 volts should be considered an completely discharged cell, below that will damage the cell.
  const float BATTERY_FULLY_CHARGED = 16.8;        // in volt. e.g. 4.2 volt * 4 cells = 16.8 volt.
  const float BATTERY_EMPTY = 12.00;               // in volt. e.g. 2.5 volt * 4 cells = 10.0 volt and then we add a few volts to give us enough power to get us back to the charger.

  // Nickel–metal hydride / NiMH example. http://batteryuniversity.com/learn/article/charging_nickel_metal_hydride
  // const float BATTERY_FULLY_CHARGED = 14.50;
  // const float BATTERY_EMPTY = 11.50;

  // Lead-acid example. http://www.solarnavigator.net/battery_charging.htm
  // const float BATTERY_FULLY_CHARGED = 13.30;
  // const float BATTERY_EMPTY = 11.90;

  const float BATTERY_MULTIPLIER = BATTERY_FULLY_CHARGED / 3.04;   // Battery voltage divided by ADC max value, used to calculate real battery voltage.
  const uint8_t BATTERY_SENSOR_CHANNEL = 3;    // Channel on ADC for measuring battery voltage.
  const uint8_t DOCKED_DETECTION_PIN = 35;

  /*
    Charger Settings
  */
  const float CHARGE_CURRENT_THRESHOLD = 100;    // Current going into the battery that are above this threshold will be interpreted as the battery is charging. 
  const float CHARGE_SHUNT_VALUE = 0.1;          // Value of charge shunt resistor (in Ohm)

  /*
    Factory reset switch
    Pulling this pin LOW will wipe Flash-memory from ALL settings and reboot mower.
  */
  const uint8_t FACTORY_RESET_PIN = 0;

  // Pull this pin LOW to emergency stop mower, pull HIGH to continue
  const uint8_t EMERGENCY_STOP_PIN = 36;

  // How many lines of log messages that are kept, increase to have longer log message history at the expense of higher memory consumption.
  const uint16_t MAX_LOGMESSAGES = 50;

  // Pin used to send and detect a ultrasonic ping for obstacle detection.
  // when viewing mower from above (facing the same direction as the mower).
  const uint8_t SONAR_FRONT_PING_PIN = 16;
  const uint8_t SONAR_FRONT_SENSE_PIN = 39;
  // Distance ahead the mower should detect obstacles (in centimeters). Between 5-400cm.
  const uint16_t SONAR_MAXDISTANCE = 200;

  // If mower is in Stuck-state, how long (in seconds) should it wait before it tries again? Set "0" for no retries.
  const uint16_t STUCK_RETRY_DELAY = 20;

  // Total width of mower (in centimeters)
  const uint8_t MOWER_WIDTH = 30;
  // Total length of mower (in centimeters)
  const uint8_t MOWER_LENGTH = 40;
  // Diameter of cutter disc (in centimeters)
  const uint8_t CUTTER_DIAMETER = 10;
}
