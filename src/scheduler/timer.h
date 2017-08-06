#ifndef _timer_util_h
#define _timer_util_h

class Timer {
  public:
    Timer();
    unsigned long startTimer();
    unsigned long millisSinceLastCheck();
    bool hasAmountTimePassed(unsigned long time);

  private:
    unsigned long previousMillis;
};

#endif
