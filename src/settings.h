#ifndef _settings_h
#define _settings_h

#include <Arduino.h>
#include "definitions.h"

class Settings {
  public:
    static const uint8_t DEMUX_PIN1;
    static const uint8_t DEMUX_PIN2;
    static const uint8_t LEFT_WHEEL_MOTOR_PIN;
    static const uint8_t LEFT_WHEEL_MOTOR_DIRECTION_PIN;
    static const uint8_t RIGHT_WHEEL_MOTOR_PIN;
    static const uint8_t RIGHT_WHEEL_MOTOR_DIRECTION_PIN;
    static const uint8_t LEFT_WHEEL_MOTOR_SPEED;
    static const uint8_t RIGHT_WHEEL_MOTOR_SPEED;
    static const uint8_t WHEEL_MOTOR_MIN_SPEED;
    static const uint8_t WHEEL_MOTOR_TURN_SPEED;
    static const bool WHEEL_MOTOR_DECREASE_SPEED_AT_CUTTER_LOAD;
    static const uint8_t CUTTER_MOTOR_PIN;
    static const uint8_t CUTTER_LOAD_DEMUX_ADDRESS;
    static const uint8_t CUTTER_MAX_SPEED;
    static const uint8_t LEFT_BWF_PIN;
    static const uint8_t RIGHT_BWF_PIN;
    static const uint32_t BWF_SENSE_TIMEOUT;
    static const Definitions::BATTERY_TYPE battery_type;
    static const uint8_t BATTERY_SENSOR_DEMUX_ADDRESS;
    static const uint16_t BATTERY_FULLY_CHARGED;
    static const uint16_t BATTERY_EMPTY;
    static const uint8_t TILT_SENSOR_DEMUX_ADDRESS;
    static const uint8_t TILT_ANGLE_MAX;
    static const uint8_t BUMPER_SENSOR_DEMUX_ADDRESS;
    static const uint8_t GPS_RX_PIN;
    static const uint8_t GPS_TX_PIN;
    static const char* SSID;
    static const char* WIFI_PASSWORD;
    static const char* OTA_PASSWORD;
    static const char* MQTT_SERVER;
    static const uint16_t MQTT_PORT;
    static const char MQTT_TOPIC[];
};

#endif
