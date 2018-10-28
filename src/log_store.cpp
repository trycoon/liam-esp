#include "log_store.h"

LogStore::LogStore() : HardwareSerial(0), log_messages(Settings::MAX_LOGMESSAGES), current_line("") {}

size_t LogStore::write(uint8_t c) {
  
  auto result = HardwareSerial::write(c);
  
  if (result > 0) {
    if (c == '\n') {
      if (log_messages.size() >= Settings::MAX_LOGMESSAGES) {
        log_messages.pop_front();
      }
      log_messages.push_back(current_line);
      current_line = "";
      current_linenumber++;
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
        if (log_messages.size() >= Settings::MAX_LOGMESSAGES) {
          log_messages.pop_front();
        }
        log_messages.push_back(current_line);
        current_line = "";
        current_linenumber++;
      } else {
        current_line.concat((char)buffer[i]);
      }
    }
  }
 
  return result;
}

std::deque<String> LogStore::getLogMessages() {

  return log_messages;
}