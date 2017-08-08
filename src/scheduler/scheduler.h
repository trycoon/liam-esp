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
    ~Scheduler();
    uint16_t schedule(std::function<void(void)> fn, uint32_t time, bool repeat = false);
    void unschedule(uint16_t id);
    void clear();
    void process();

  private:
    std::list<scheduled_fn_t> scheduled_fn_list;
    uint16_t task_counter;
    bool in_series;
};

#endif
