#ifndef _scheduler_util_h
#define _scheduler_util_h

#include <functional>
#include <list>

struct scheduled_fn_t
{
    unsigned int id;
    unsigned long currentMillis;
    unsigned long delay;
    std::function<void(void)> func;
};

class Scheduler {
  public:
    Scheduler(bool inSeries);
    ~Scheduler();
    unsigned int schedule(std::function<void(void)> fn, uint32_t time);
    //void unschedule(unsigned int);
    void process();

  private:
    std::list<scheduled_fn_t*> scheduled_fn_list;    
    unsigned int task_counter;
    bool in_series;
};

#endif
