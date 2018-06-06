#include "io_accelerometer.h"
#include "settings.h"

// https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/pinouts
// https://github.com/adafruit/Adafruit_BNO055
// https://github.com/adafruit/Adafruit_BNO055/pull/21/files
IO_Accelerometer::IO_Accelerometer() {

/*  if (Settings::IO_ACCELEROMETER_ADDRESS > 0) {
    accel = Adafruit_ADXL345_Unified(11225);  // just some dummy id.
    available = accel.begin(Settings::IO_ACCELEROMETER_ADDRESS, Settings::SDA_PIN, Settings::SCL_PIN);

    if (available == true) {
      // Set the range to whatever is appropriate.
      accel.setRange(ADXL345_RANGE_16_G);
      // accel.setRange(ADXL345_RANGE_8_G);
      // accel.setRange(ADXL345_RANGE_4_G);
      // accel.setRange(ADXL345_RANGE_2_G);
    }
  }*/
}

bool IO_Accelerometer::isAvailable() {
  return available;
}

orientation IO_Accelerometer::getOrientation() {
  if (available == false) {
    orientation empty;

    return empty;

  } else {
  //  sensors_event_t event;
  //  accel.getEvent(&event);

    orientation result;
  /*  result.pitch = event.acceleration.pitch;
    result.roll = event.acceleration.roll;
    result.heading = event.acceleration.heading;
*/
    return result;
  }
}

bool IO_Accelerometer::isFlipped() {
  if (available == false) {
    return false;
  } else {
  //  sensors_event_t event;
  //  accel.getEvent(&event);
return false;
//return (abs(event.acceleration.pitch) >= Settings::TILT_ANGLE_MAX || abs(event.acceleration.roll) >= Settings::TILT_ANGLE_MAX);
  }
}
