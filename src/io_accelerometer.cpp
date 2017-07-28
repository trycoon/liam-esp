#include "io_accelerometer.h"
#include "settings.h"

IO_Accelerometer::IO_Accelerometer() {

  if (Settings::IO_ACCELEROMETER_ADDRESS > 0) {
    accel = Adafruit_ADXL345_Unified(11225);  // just some dummy id.
    available = accel.begin(Settings::IO_ACCELEROMETER_ADDRESS, Settings::SDA_PIN, Settings::SCL_PIN);

    if (available == true) {
      // Set the range to whatever is appropriate.
      accel.setRange(ADXL345_RANGE_16_G);
      // accel.setRange(ADXL345_RANGE_8_G);
      // accel.setRange(ADXL345_RANGE_4_G);
      // accel.setRange(ADXL345_RANGE_2_G);
    }
  }
}

bool IO_Accelerometer::isAvailable() {
  return available;
}

sensors_vec_t IO_Accelerometer::getOrientation() {
  if (available == false) {
    sensors_event_t event;

    return event.acceleration;

  } else {
    sensors_event_t event;
    accel.getEvent(&event);

    return event.acceleration;
  }
}

bool IO_Accelerometer::isFlipped() {
  if (available == false) {
    return false;
  } else {
    sensors_event_t event;
    accel.getEvent(&event);

    return (abs(event.acceleration.pitch) >= Settings::TILT_ANGLE_MAX || abs(event.acceleration.roll) >= Settings::TILT_ANGLE_MAX);
  }
}
