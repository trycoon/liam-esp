#ifndef timer_h
#define timer_h

class Timer {
  public:
    Timer();
    unsigned long GetMillis(void);
    unsigned long MillisSinceLast(void);
    bool HasTimePassed(unsigned long time);
  private:
    unsigned long previousMillis;
};

#endif
