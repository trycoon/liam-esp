// Borrowed from Kris Winers https://github.com/kriswiner/MPU9250/blob/master/Dual_MPU9250/MadgwickFilters

// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
#ifndef madgwick_filters_h
#define madgwick_filters_h

#include <Arduino.h>

struct Quaternion {
  float q1 = 1.0f;
  float q2 = 0.0f;
  float q3 = 0.0f;
  float q4 = 0.0f;
};

class MadgwickFilters {
  private:
    float GyroMeasError = PI * (40.0f / 180.0f);        // gyroscope measurement error in rads/s (start at 40 deg/s)
    float beta = sqrtf(3.0f / 4.0f) * GyroMeasError;    // compute beta
    Quaternion quaternion;

    
  public:
    MadgwickFilters();
    __attribute__((optimize("O3"))) void madgwickQuaternionUpdate(float deltaTime, float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
    const Quaternion& getQuaternions() const;
};

#endif