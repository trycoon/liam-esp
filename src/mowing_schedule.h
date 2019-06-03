#ifndef _mowing_schedule_h
#define _mowing_schedule_h

#include <Arduino.h>
#include <deque>
#include <regex>

struct scheduleEntry {
  std::deque<bool> activeWeekdays;
  String startTime;
  String stopTime;
};

class MowingSchedule {
  public:
    MowingSchedule();
    int8_t addScheduleEntry(std::deque<bool> activeWeekdays, String startTime, String stopTime);
    const std::deque<scheduleEntry>& getScheduleEntries() const;
    void removeScheduleEntry(uint8_t position);
    bool isTimeToMow();
    void start();
    
  private:
    std::deque<scheduleEntry> mowingSchedule;
    void saveSchedulesToFlash();
    void loadSchedulesFromFlash();
};

#endif