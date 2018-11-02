#ifndef _log_store_h
#define _log_store_h

#include <Arduino.h>
#include <deque>
#include "HardwareSerial.h"
#include "definitions.h"

class LogStore : public HardwareSerial {
  public:
    LogStore();
    size_t write(uint8_t) override;
    size_t write(const uint8_t* buffer, size_t size) override;
    const std::deque<String>& getLogMessages() const;

  private:
    std::deque<String> log_messages;
    uint32_t current_linenumber;
    String current_line;
    void writeInternal(uint8_t c);
};

extern LogStore LoggingSerial;

#endif