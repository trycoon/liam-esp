#include "log_store.h"

LogStore::LogStore() : HardwareSerial(0) {}

size_t LogStore::write(uint8_t c) {
  auto result = HardwareSerial::write(c);
  if (result > 0) {
   /* if (log_messages.size() >= Settings::LOGMESSAGES_LENGTH) {
      log_messages.pop_front();
    }
    log_messages.push_back(c);*/
  }
 
  return result;
}

size_t LogStore::write(const uint8_t *buffer, size_t size) {
  auto result = HardwareSerial::write(buffer, size);
  if (result > 0) {
    for (int i = 0; i < size; i++) {
      /*if (log_messages.size() >= Settings::LOGMESSAGES_LENGTH) {
        log_messages.pop_front();
      }
      log_messages.push_back(buffer[i]);*/
    }
  }
 
  return result;
}

String LogStore::getLogMessages() {
  //try {
  String currentLine = String(log_messages.size());
  //currentLine.concat(" ");
  /*currentLine.concat(String(log_messages.available()));
  currentLine.concat(" ");
  currentLine.concat(log_messages.empty());
  currentLine.concat(" ");
  currentLine.concat(log_messages.full());
  currentLine.concat(" ");
  currentLine.concat(log_messages.max_size());*/
  //currentLine.concat(log_messages.front());
  /*for (char const &i: log_messages) {
    currentLine += &i;
  }*/

  return currentLine;
  /*} catch (...) {
    std::exception_ptr p = std::current_exception();
    return p ? p.__cxa_exception_type()->name() : "null";
  }*/
}