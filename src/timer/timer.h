#ifndef timer_util_h
#define timer_util_h

class Timer {
  public:
    Timer();
    unsigned long GetMillis();
    unsigned long MillisSinceLast();
    bool HasAmountTimePassed(unsigned long time);
  private:
    unsigned long previousMillis;
};

#endif
