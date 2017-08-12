#include <Arduino.h>
#include <algorithm>
#include "scheduler.h"

/**
* Constructor for a function scheduler.
* @param <bool> inSeries if true the delay time for a scheduled function will be releative the previous scheduled function. If false the delay will be relative the current time when the function was scheduled.
*/
Scheduler::Scheduler(bool inSeries) : in_series(inSeries) { }

/**
* Schedule a function to execute after the specified delay.
* The function will be executed only once, unless the repeat-flag has been set.
* @param <std::function> fn function to be scheduled for later execution.
* @param <uint32_t> delay delay in milliseconds.
* @param <bool> repeat when the delay has been reached and the function has been executed, then reschedule the function for another delay milliseconds.
*/
uint16_t Scheduler::schedule(std::function<void(void)> fn, uint32_t delay, bool repeat) {

  scheduled_fn_t it;
  it.id = ++task_counter;
  it.func = fn;
  it.repeat = repeat;

  if (in_series && !scheduled_fn_list.empty()) {
    it.startMillis = scheduled_fn_list.back().startMillis;
    it.delay = scheduled_fn_list.back().delay + delay;
  } else {
    it.startMillis = millis();
    it.delay = delay;
  }

  scheduled_fn_list.push_back(std::move(it));

  // TODO: sort list if not in_series. delay and startMillis must be taken into account releative the current millis!
  /*if (!in_series) {
    scheduled_fn_list.sort([](const scheduled_fn_t &f, const scheduled_fn_t &s) {
      return f.delay < s.delay;
    });
  }*/

  return task_counter;
}

/**
* Unschedule a already scheduled function.
* @param <uint16_t> id id of already scheduled function.
*/
void Scheduler::unschedule(uint16_t id) {
  scheduled_fn_list.erase(
    find_if(scheduled_fn_list.begin(), scheduled_fn_list.end(),
        [id](const scheduled_fn_t& i) {
          return i.id == id;
        }),
    scheduled_fn_list.end()
  );
}

/**
* Returns whether no functions has been scheduled.
*/
bool Scheduler::isEmpty() {
  return scheduled_fn_list.empty();
}

/**
* Remove all scheduled functions.
*/
void Scheduler::clear() {
  scheduled_fn_list.clear();
}

/**
* Method should be called upon repeatedly and requent to execute the functions that may have reached their delay time.
*/
void Scheduler::process() {
  for (auto i = scheduled_fn_list.begin(); i != scheduled_fn_list.end();) {
    // Check if delay has expired. Handles timer overflow. millis() on ESP8266 has a roll over of 72 minutes. (Based on microsecond tick.)
    if (millis() - (*i).startMillis >= (*i).delay) {
      // execute scheduled function.
      (*i).func();

      yield();
      // reschedule if set to repeat.

      if ((*i).repeat) {
        if (in_series) {
          (*i).startMillis = scheduled_fn_list.back().startMillis;
          (*i).delay = scheduled_fn_list.back().delay + (*i).delay;
        } else {
          (*i).startMillis = millis();
        }
        // repush it to the last postition in the list.
        scheduled_fn_list.push_back((*i));
      }

      // remove from current position in list.
      i = scheduled_fn_list.erase(i);

    } else {
      ++i;
    }
  }

  yield();
}
