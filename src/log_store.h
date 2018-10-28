#ifndef _log_store_h
#define _log_store_h

#include <Arduino.h>
#include <deque.h>
#include "HardwareSerial.h"
#include "settings.h"

class LogStore : public HardwareSerial {
  public:
    LogStore();
    size_t write(uint8_t) override;
    size_t write(const uint8_t *buffer, size_t size) override;
    String getLogMessages();

  private:
    etl::deque<uint8_t, 2000> log_messages;
};

extern LogStore LoggingSerial;

#endif