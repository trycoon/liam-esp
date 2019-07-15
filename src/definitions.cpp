#include "definitions.h"

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
  const uint8_t LEFT_WHEEL_MOTOR_PIN = 19;
  const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN = 18;
  const uint8_t LEFT_WHEEL_ODOMETER_PIN = 5;
  const uint8_t LEFT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the right.
  const bool LEFT_WHEEL_MOTOR_INVERTED = false; // Set to "true" if left wheel runs backward when mower should be running forward.

  const uint8_t RIGHT_WHEEL_MOTOR_PIN = 17;
  const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN = 16;
  const uint8_t RIGHT_WHEEL_ODOMETER_PIN = 4;
  const uint8_t RIGHT_WHEEL_MOTOR_SPEED = 100;   // 0-100 (%), used to compensate for drifting motors, lower this value if mower drift to the left.
  const bool RIGHT_WHEEL_MOTOR_INVERTED = true;  // Set to "true" if right wheel runs backward when mower should be running forward.

  const uint8_t WHEEL_MOTOR_MIN_SPEED = 20;    // 0-100 (%), set the minimum speed that the wheel motors should use. This is used in combination with e.g. WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD.
  const uint8_t WHEEL_MOTOR_TURN_SPEED = 50;   // 0-100 (%), speed to use when turning.
  const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD = true;  // reduce forward movement of mower when there is a high load on cutter (like high grass)
  const uint16_t WHEEL_ODOMETERPULSES_PER_ROTATION = 90;        // number of odometer pulses from motor that equals a full rotation of the shaft (check with motor manufacturer).
  const uint8_t WHEEL_DIAMETER = 190;     // in millimeter
  const uint8_t WHEEL_PAIR_DISTANCE = 27; // distance meassured between the wheel pairs, in centimeter
  
  /*
    Settings for "launching" mower from charging station
  */
  const uint8_t LAUNCH_DISTANCE = 100;  // the distance the mower should back out of the charging station before turning around and begin mowing, in centimeters.

  /*
    Cutter settings
  */
  const uint8_t CUTTER_MOTOR_PIN = 32;   // Pin to PWM-control motor.
  const uint8_t CUTTER_BRAKE_PIN = 33;   // Pin for braking cutter motor.
  const uint8_t CUTTER_LOAD_PIN = 36;    // Pin on ADC for measuring cutter motor load.
  const float CUTTER_LOAD_RESISTOR_MULTIPLIER = 20.0f/170.0f;   // Setting of RV3-trim resistor divided by size of RV3-trim resistor, e.g. 20/170 (kilo ohm). This is to read the correct cutter load.
  const float CUTTER_LOAD_MAXVALUE = 800;// Max value of cutter load, milliamperes when cutter motor has almost stalled. (used to calculate cutter load percentage)
  const uint8_t CUTTER_MAX_SPEED = 100;  // 0-100 (%), lower this value if cutter spinning too fast.
  // When the load on the cuttor motor surpasses this limit, the cutter is working too hard cutting the grass (and we should reduce speed of wheels to compensate).
  const uint16_t CUTTER_LOAD_THRESHOLD = 1000;

  /*
    Battery settings
  */
  const uint8_t BATTERY_SENSOR_PIN = 39;    // Pin on ADC for measuring battery voltage.

  // Lithium-ion / LiPo  http://batteryuniversity.com/learn/article/types_of_lithium_ion
  // Normally a fully charged cell is 4.2 volt and quickly drops down to 3.7 volt, as battery is closing depleated it will drop from 3.7 down to 3.0 volt.
  // 3.0 volts should be considered an completely discharged cell, below that will damage the cell.
  const float BATTERY_FULLY_CHARGED = 16.8;        // in volt. e.g. 4.2 volt * 4 cells = 16.8 volt.
  const float BATTERY_EMPTY = 14.00;               // in volt. e.g. 3.0 volt * 4 cells = 12.0 volt and then we add a few volts to give us enough power to get us back to the charger.
  const float BATTERY_MULTIPLIER = BATTERY_FULLY_CHARGED / 3.04;   // Battery voltage divided by ADC max value, used to calculate real battery voltage.

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
  const float CHARGE_CURRENT_THRESHOLD = 100;    // Current going into the battery that are above this threshold will be interpreted as the battery is charging. 

  /*
    Factory reset switch
    Pulling this pin LOW will wipe Flash-memory from ALL settings and reboot mower.
  */
  const uint8_t FACTORY_RESET_PIN = 35;

  /*
    GPS positioning system (also using others if available)
    Set GPS_RX_PIN and GPS_TX_PIN to "0" if not used!
  */
  const uint8_t GPS_RX_PIN = 3;  // GPIO3 (RXD0)
  const uint8_t GPS_TX_PIN = 1;  // GPIO1 (TDX0)

  // Pull this pin LOW to emergency stop mower, pull HIGH to continue
  const uint8_t EMERGENCY_STOP_PIN = 2;

  // Max number of messages awaiting to be sent to broker. Higher value consumes more RAM-memory but let us be disconnected from WiFi for a longer period.
  const uint16_t MQTT_QUEUE_LENGTH = 50;

  // How many lines of log messages that are kept, increase to have longer log message history at the expense of higher memory consumption.
  const uint16_t MAX_LOGMESSAGES = 50;

  // Pin used to send and detect a ultrasonic ping for obstacle detection.
  // SONAR-sensor #1 is the one pointing straight forward.
  // SONAR-sensor #2 is the one pointing to the left when viewing mower from above (facing the same direction as the mower).
  // SONAR-sensor #3 is the one pointing to the right when viewing mower from above (facing the same direction as the mower).
  const uint8_t SONAR1_PIN = 34;
  const uint8_t SONAR2_PIN = 23;
  const uint8_t SONAR3_PIN = 27;
  // Distance ahead the mower should detect obstacles (in centimeters). Between 5-400cm.
  const uint16_t SONAR_MAXDISTANCE = 200;
}
