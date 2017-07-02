#ifndef _bwf_h
#define _bwf_h

#include <Arduino.h>

/**
* Border Wire Fence, the class that takes care of detecting if the mower is inside or outside of the perimiter wire dug down in your lawn.
* This is a wire the mower should never pass to keep it inside our garden but also away from static obstacles like flower-beds and trees.
* The mower may use the wire to follow it back to the charging station.
*/
class BWF {
  public:
    BWF();

};

#endif
