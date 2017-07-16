#include "io_accelerometer.h"
#include "settings.h"

IO_Accelerometer::IO_Accelerometer() {
  accel = Adafruit_ADXL345_Unified(11225);  // just some dummy id.

  isAvailable = accel.begin(ADXL345_DEFAULT_ADDRESS, Settings::SDA_PIN, Settings::SCL_PIN);

  if(isAvailable) {
    // Set the range to whatever is appropriate.
    //accel.setRange(ADXL345_RANGE_16_G);
    // accel.setRange(ADXL345_RANGE_8_G);
    accel.setRange(ADXL345_RANGE_4_G);
    // accel.setRange(ADXL345_RANGE_2_G);

    sensor_t sensor;
    accel.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print ("Sensor: "); Serial.println(sensor.name);
    Serial.print ("Driver Ver: "); Serial.println(sensor.version);
    Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
    Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
    Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
    Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
    Serial.println("------------------------------------");
    Serial.println("");

    sensors_event_t event;
    accel.getEvent(&event);

    Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
    Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
    Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print(" ");Serial.println("m/s^2 ");
  }
}

bool IO_Accelerometer::isAvailable() {
  return isAvailable;
}

bool IO_Accelerometer::isFlipped() {
  if (isAvailable == false) {
    return false;
  } else {
    return false; // TODO
  }
}
