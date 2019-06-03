#include "mowing_schedule.h"
#include "configuration.h"
#include <Preferences.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>


MowingSchedule::MowingSchedule() {}

/**
 * Adds a new entry to the schedule list.
 * @param activeWeekdays represent the seven days in a week (MUST always sized seven)
 * @param time to start mowing, the format MUST be "HH:MM"
 * @param time to stop mowing, the format MUST be "HH:MM"
 * @return -1 malformated activeWeekdays, -2 malformated startTime, -3 malformated stopTime, -4 too many entries. 0 or greater = success
 */
int8_t MowingSchedule::addScheduleEntry(std::deque<bool> activeWeekdays, String startTime, String stopTime) {
  const std::regex timeRegex("(00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23):(0|1|2|3|4|5)\\d");

  if (mowingSchedule.size() >= 10) {
    return -4;
  }

  if (activeWeekdays.size() < 7 || activeWeekdays.size() > 7) {
    return -1;
  }

  if (!std::regex_match(startTime.c_str(), timeRegex)) {
    return -2;
  }
  
  if (!std::regex_match(stopTime.c_str(), timeRegex)) {
    return -3;
  }

  scheduleEntry entry;
  entry.activeWeekdays = activeWeekdays;
  entry.startTime = startTime;
  entry.stopTime = stopTime;

  mowingSchedule.push_front(entry);
  saveSchedulesToFlash();

  return 1;
}

const std::deque<scheduleEntry>& MowingSchedule::getScheduleEntries() const {
  return mowingSchedule;
}

/**
 * Removes an entry from the list
 * @param position the position of the entry in the list that should be removed (first position = 0)
 */
void MowingSchedule::removeScheduleEntry(uint8_t position) {
  if (position < mowingSchedule.size()) {
    mowingSchedule.erase(mowingSchedule.begin() + position);
    saveSchedulesToFlash();
  }
}

/**
 * Check if the mower should mow now, according to the mowing schedule and the current time.
 */
bool MowingSchedule::isTimeToMow() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo, 200)) { // tries for 200 ms
    return false;
  }

  // fix day-of-week to follow ISO-8601
  int8_t dayOfWeek = timeinfo.tm_wday == 0 ? 6 : timeinfo.tm_wday - 1;

  for (auto schedule : mowingSchedule) {

    if (schedule.activeWeekdays[dayOfWeek]) {
      int currentTimeInMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
      int startTimeInMinutes = schedule.startTime.substring(0, 2).toInt() * 60 + schedule.startTime.substring(3).toInt(); // turn string, like "08:45", into minutes.
      int stopTimeInMinutes = schedule.stopTime.substring(0, 2).toInt() * 60 + schedule.stopTime.substring(3).toInt();

      if (currentTimeInMinutes >= startTimeInMinutes && currentTimeInMinutes < stopTimeInMinutes) {
        return true;
      }
    }
  }

  return false;
}

void MowingSchedule::start() {
  loadSchedulesFromFlash();
}

void MowingSchedule::loadSchedulesFromFlash() {

  mowingSchedule.clear();

  Configuration::preferences.begin("liam-esp", false);
  auto jsonString = Configuration::preferences.getString("schedules", "[]");

  DynamicJsonBuffer jsonBuffer(1200);
  JsonArray& root = jsonBuffer.parseArray(jsonString);
  
  if (root.success()) {

    for (auto schedule : root) {
      scheduleEntry entry;
      std::deque<bool> activeWeekdays;

      for (const auto& day : schedule["activeWeekdays"].as<JsonArray>()) {
        activeWeekdays.push_back(day);
      }

      entry.activeWeekdays = activeWeekdays;
      entry.startTime = schedule["startTime"].as<char*>();
      entry.stopTime = schedule["stopTime"].as<char*>();

      mowingSchedule.push_back(entry);
    }
    
    Log.notice(F("loaded %i schedules" CR), root.size());
  }
}

void MowingSchedule::saveSchedulesToFlash() {
  // persist mowing schedules in case of power failure.
  DynamicJsonBuffer jsonBuffer(1200);
  JsonArray& root = jsonBuffer.createArray();

  for (auto schedule : mowingSchedule) {
    JsonObject& entry = root.createNestedObject();
    
    JsonArray& activeWeekdays = entry.createNestedArray("activeWeekdays");

    for (auto day : schedule.activeWeekdays) {
      activeWeekdays.add(day);
    }

    entry["startTime"] = schedule.startTime;
    entry["stopTime"] = schedule.stopTime;
  }

  String jsonString;
  root.printTo(jsonString);

  Configuration::preferences.begin("liam-esp", false);
  Configuration::preferences.putString("schedules", jsonString);
}
