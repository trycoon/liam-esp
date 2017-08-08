#ifndef _i_processable_h
#define _i_processable_h

/**
* Interface for interactive classes that needs to update their internal state on every run of the main loop.
*/
class Processable {
  public:
    /**
    * This method should be be called upon to update the objects internal state.
    */
    virtual void process() = 0;
};

#endif
