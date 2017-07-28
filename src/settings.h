#ifndef _settings_h
#define _settings_h

#include <string>
#include <Arduino.h>
#include "definitions.h"

namespace Settings {
  extern const uint8_t SDA_PIN;
  extern const uint8_t SCL_PIN;
  extern const uint8_t IO_ANALOG_ADDRESS;
  extern const uint8_t IO_ACCELEROMETER_ADDRESS;
  extern const uint8_t LEFT_WHEEL_MOTOR_PIN;
  extern const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN;
  extern const uint8_t LEFT_WHEEL_MOTOR_SPEED;
  extern const bool LEFT_WHEEL_MOTOR_INVERED;
  extern const uint8_t RIGHT_WHEEL_MOTOR_PIN;
  extern const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN;
  extern const uint8_t RIGHT_WHEEL_MOTOR_SPEED;
  extern const bool RIGHT_WHEEL_MOTOR_INVERED;
  extern const uint8_t WHEEL_MOTOR_MIN_SPEED;
  extern const uint8_t WHEEL_MOTOR_TURN_SPEED;
  extern const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD;
  extern const uint8_t CUTTER_MOTOR_PIN;
  extern const uint8_t CUTTER_LOAD_CHANNEL;
  extern const uint8_t CUTTER_MAX_SPEED;
  extern const uint8_t LEFT_BWF_PIN;
  extern const uint8_t RIGHT_BWF_PIN;
  extern const uint32_t BWF_SENSE_TIMEOUT;
  extern const uint8_t BATTERY_SENSOR_CHANNEL;
  extern const uint16_t BATTERY_FULLY_CHARGED;
  extern const uint16_t BATTERY_EMPTY;
  extern const uint8_t TILT_ANGLE_MAX;
  extern const uint8_t BUMPER_SENSOR_PIN;
  extern const uint8_t GPS_RX_PIN;
  extern const uint8_t GPS_TX_PIN;
  extern const char* const SSID;
  extern const char* const WIFI_PASSWORD;
  extern const char* const OTA_PASSWORD;
  extern const char* const MQTT_SERVER;
  extern const uint16_t MQTT_PORT;
  extern const uint16_t MQTT_QUEUE_LENGTH;
  extern const std::string MQTT_TOPIC;
};

#endif
