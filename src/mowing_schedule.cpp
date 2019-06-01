#include "mowing_schedule.h"

MowingSchedule::MowingSchedule() {

}

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
  }
}

bool MowingSchedule::isTimeToMow() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo, 5000)) { // tries for 5000 ms
    return F("Failed to obtain time");
  }
  return true;
}
