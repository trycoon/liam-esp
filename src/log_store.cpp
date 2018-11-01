#include "log_store.h"

/**
 * Lowlevel class for writing log messages to serial output, but also to store them for later retreival with method getLogmessages.
 */
LogStore::LogStore() : HardwareSerial(0), log_messages(Definitions::MAX_LOGMESSAGES), current_line("") {
  current_line.reserve(100);
}

size_t LogStore::write(uint8_t c) {
  
  auto result = HardwareSerial::write(c);
  
  if (result > 0) {
    if (c == '\n') {
      if (log_messages.size() >= Definitions::MAX_LOGMESSAGES) {
        log_messages.pop_front();
      }
      log_messages.emplace_back(std::move(current_line));
      current_line = "";
      current_line.reserve(100);
      ++current_linenumber;
    } else {
      current_line.concat((char)c);
    }
  }
 
  return result;
}

size_t LogStore::write(const uint8_t* buffer, size_t size) {
  auto result = HardwareSerial::write(buffer, size);

  if (result > 0) {
    for (int i = 0; i < size; i++) {
      if (buffer[i] == '\n') {
        if (log_messages.size() >= Definitions::MAX_LOGMESSAGES) {
          log_messages.pop_front();
        }
        log_messages.emplace_back(std::move(current_line));
        current_line = "";
        current_line.reserve(100);
        ++current_linenumber;
      } else {
        current_line.concat((char)buffer[i]);
      }
    }
  }
 
  return result;
}

std::deque<String>& LogStore::getLogMessages() {

  return log_messages;
}