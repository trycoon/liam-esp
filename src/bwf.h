#ifndef _bwf_h
#define _bwf_h

#include <Arduino.h>

// TODO:
// Currently the software navigate the lawn using a RTK enabled GPS receiver, and even tough this solution has many positive aspects it's much more expensive than using a BWF.
// So it would be nice is this class get implemented to offer a cheeper alternative for those that think that a 500$ "GPS"-receiver is a bit pricey.

/**
* Border Wire Fence, the class that takes care of detecting if the mower is inside or outside of the perimiter wire dug down in your lawn.
* This is a wire the mower should never pass to keep it inside our garden but also away from static obstacles like flower-beds and trees.
* The mower may use the wire to follow it back to the charging station.
*/
class BWF {
  public:
    BWF();
    bool isLeftInsideFence();
    bool isRightInsideFence();

  private:
    const int BFW_STEP_DELAY = 100;  // microseconds

    // Signal the Border Wire Fence sensors should look for, as transmitted by the charging station.
    // When this signal is detected the sensor-part of the mower is on the INSIDE of the border fence.
    const int BFW_INSIDE_SIGNAL[10] = {+1, +1, -1, +1, 0, 0, 0, 0, 0, 0 }; // https://en.wikipedia.org/wiki/Barker_code (length 4)
    //  +---+       +---+
    //  |   |       |   |     Expected received signal
    //  |   |       |   |
    //  +   +---+   |   +------------------------
    //          |   |
    //          |   |
    //          +---+

    // https://github.com/G6EJD/ESP32-8-Octave-Audio-Spectrum-Display/blob/master/ESP32_Spectrum_Display_03.ino
    // http://www.grauonline.de/alexwww/ardumower/filter/filter.html
    // https://www.ardumower.de/index.php/en/induktion
    // http://grauonline.de/wordpress/?page_id=364
    // https://www.youtube.com/watch?v=NIer_kITelc
    // https://en.wikipedia.org/wiki/Barker_code
};

#endif
