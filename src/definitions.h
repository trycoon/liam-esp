#ifndef _definitions_h
#define _definitions_h

#include <Arduino.h>

namespace Definitions {

  /*
  Constants and other global stuff that you should probably never need to touch.
  */
  extern const char* const APP_NAME;
  extern const char* const APP_VERSION;

  enum class MOWER_STATES {
                            DOCKED,     // mower is docked in its charging station and is fully charged
                            LAUNCHING,  // mower is leaving its charging station to head out for mowing
                            MOWING,     // mower is currently mowing
                            DOCKING,    // mower has stopped mowing and is heading back to its charging station (battery may be running low)
                            CHARGING,   // mower is docked in its charging station and is currently charging
                            STUCK,      // mower is stuck somewhere and have shutdown (it may be stuck in a hole, the cutter may be stuck, or some other hardware issue...)
                            FLIPPED,    // mower is flipped upside down or tiled too much, stopp mowing and wait to be unflipped.
                            MANUAL,     // mower is in manual drive mode and controlled by user from REST-API.
                            STOP,       // mower is stopped from it's normal operation, this happens when user press the physical stopbutton or UI button.
                            TEST        // mower is in test mode.
                          };

  extern const uint8_t SDA_PIN;
  extern const uint8_t SCL_PIN;
  extern const uint8_t MOTOR_TIMER_13_BIT;
  extern const uint16_t MOTOR_BASE_FREQ;
  extern const uint8_t LEFT_WHEEL_MOTOR_PIN;
  extern const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN;
  extern const uint8_t LEFT_WHEEL_ODOMETER_PIN;
  extern const uint8_t LEFT_WHEEL_MOTOR_SPEED;
  extern const bool LEFT_WHEEL_MOTOR_INVERTED;
  extern const uint8_t RIGHT_WHEEL_MOTOR_PIN;
  extern const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN;
  extern const uint8_t RIGHT_WHEEL_ODOMETER_PIN;
  extern const uint8_t RIGHT_WHEEL_MOTOR_SPEED;
  extern const bool RIGHT_WHEEL_MOTOR_INVERTED;
  extern const uint8_t WHEEL_MOTOR_MIN_SPEED;
  extern const uint8_t WHEEL_MOTOR_TURN_SPEED;
  extern const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD;
  extern const uint8_t LAUNCH_DISTANCE;
  extern const uint16_t WHEEL_ODOMETERPULSES_PER_ROTATION;
  extern const uint8_t WHEEL_DIAMETER;
  extern const uint8_t WHEEL_PAIR_DISTANCE;
  extern const uint8_t CUTTER_MOTOR_PIN;
  extern const uint8_t CUTTER_BRAKE_PIN;
  extern const uint8_t CUTTER_LOAD_PIN;
  extern const uint16_t CUTTER_LOAD_RESISTOR;
  extern const float CUTTER_NOLOAD_CURRENT;
  extern const float CUTTER_MAX_CURRENT;
  extern const uint8_t CUTTER_MAX_SPEED;
  extern const uint16_t CUTTER_LOAD_THRESHOLD;
  extern const uint8_t EMERGENCY_STOP_PIN;
  extern const uint8_t BATTERY_SENSOR_PIN;
  extern const float BATTERY_FULLY_CHARGED;
  extern const float BATTERY_EMPTY;
  extern const float BATTERY_MULTIPLIER;
  extern const float CHARGE_CURRENT_THRESHOLD;
  extern const uint8_t TILT_ANGLE_MAX;
  extern const uint8_t FACTORY_RESET_PIN;
  extern const uint8_t GPS_RX_PIN;
  extern const uint8_t GPS_TX_PIN;
  extern const uint16_t MQTT_QUEUE_LENGTH;
  extern const uint16_t MAX_LOGMESSAGES;
  extern const uint8_t SONAR_LEFT_PIN;
  extern const uint8_t SONAR_FRONT_PIN;
  extern const uint8_t SONAR_RIGHT_PIN;
  extern const uint16_t SONAR_MAXDISTANCE;
  extern const uint16_t STUCK_RETRY_DELAY;
};

#endif
