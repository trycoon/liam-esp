#include <Arduino.h>
#include "scheduler.h"

/**
* Constructor for a function scheduler.
* @param <bool> inSeries if true the delay time for a scheduled function will be releative the previous scheduled function. If false the delay will be relative the current time when the function was scheduled.
*/
Scheduler::Scheduler(bool inSeries) : in_series(inSeries) { }

Scheduler::~Scheduler() {
  for (auto f: scheduled_fn_list) {
    delete f;
  }

  scheduled_fn_list.clear();
}

/**
* Schedule a function to execute after the specified delay.
* The function will be executed only once.
* @param <std::function> fn function to be scheduled for later execution.
* @param <uint32_t> delay delay in milliseconds.
*/
unsigned int Scheduler::schedule(std::function<void(void)> fn, uint32_t delay) {

  scheduled_fn_t* it = new scheduled_fn_t;
  it->id = ++task_counter;
  it->func = fn;

  if (in_series && !scheduled_fn_list.empty()) {
    it->currentMillis = scheduled_fn_list.back()->currentMillis;
    it->delay = scheduled_fn_list.back()->delay + delay;
  } else {
    it->currentMillis = millis();
    it->delay = delay;
  }

  scheduled_fn_list.push_back(it);

  return it->id;
}

/**
* Method should be called upon repeatedly and requent to execute the function that may have reached their delay time.
*/
void Scheduler::process() {
  for (auto i = scheduled_fn_list.begin(); i != scheduled_fn_list.end();) {
    if (millis() - (*i)->currentMillis >= (*i)->delay) {  // handles timer overflow. millis() on ESP8266 has a roll over of 72 minutes. (Based on microsecond tick.)
      i = scheduled_fn_list.erase(i);
      (*i)->func();
      yield();
    } else {
      ++i;
    }
  }
}
