#ifndef _scheduler_util_h
#define _scheduler_util_h

#include <functional>
#include <list>

struct scheduled_fn_t
{
    bool repeat;
    uint16_t id;
    uint16_t delay;
    uint32_t startMillis;
    std::function<void(void)> func;
};

class Scheduler {
  public:
    Scheduler(bool inSeries = false);
    uint16_t schedule(const std::function<void(void)> fn, uint32_t time, bool repeat = false);
    void unschedule(uint16_t id);
    bool isEmpty();
    void clear();
    void process();

  private:
    std::list<scheduled_fn_t> scheduled_fn_list;
    uint16_t task_counter = 0;
    bool in_series = false;
};

#endif
