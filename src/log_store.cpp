#include "log_store.h"
#include "utils.h"

// https://github.com/espressif/arduino-esp32/blob/master/libraries/SPIFFS/examples/SPIFFS_time/SPIFFS_time.ino

/**
 * Lowlevel class for writing log messages to serial output, but also to store them for later retreival with method getLogmessages.
 */
LogStore::LogStore() : HardwareSerial(0) {
  current_line.reserve(100);
}

size_t LogStore::write(uint8_t c) { 
  auto result = HardwareSerial::write(c);
  
  if (result > 0) {
    writeInternal(c);
  }
 
  return result;
}

size_t LogStore::write(const uint8_t* buffer, size_t size) {
  auto result = HardwareSerial::write(buffer, size);

  if (result > 0) {
    for (int i = 0; i < result; i++) {
      writeInternal(buffer[i]);
    }
  }
 
  return result;
}

logmessage_response LogStore::getLogMessages() {
  logmessage_response response = {
    total: current_lastnr,
    messages: log_messages
  };

  return response;
}

void LogStore::writeInternal(uint8_t c) {

  if (c != '\n') {
    current_line.concat((char)c);
  } else {
    if (log_messages.size() >= Definitions::MAX_LOGMESSAGES) {
      log_messages.pop_front();
    }

    log_messages.emplace_back();
    auto& newMsg = log_messages.back();
  
    newMsg.id = ++current_lastnr;
    
    if (Utils::isTimeAvailable) {
      newMsg.message = Utils::getTime("%H:%M:%S") + " " + current_line;
      current_line = "";
    }
    else {
      newMsg.message = std::move(current_line);
      current_line.reserve(100);
    }
  }
}
