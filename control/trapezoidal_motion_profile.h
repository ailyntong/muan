/*
 * TrapezoidalMotionProfile.h
 *
 *  Created on: Sep 25, 2015
 *      Author: Kyle
 */

#ifndef MUAN_CONTROL_TRAPEZOIDALMOTIONPROFILE_H_
#define MUAN_CONTROL_TRAPEZOIDALMOTIONPROFILE_H_

#include "motion_profile.h"
#include <cmath>
#include <type_traits>

namespace muan {

template <typename DistanceU>
class TrapezoidalMotionProfile : public MotionProfile<DistanceU> {
  using VelocityU = typename std::remove_cv_t<decltype(DistanceU(0) / s)>;
  using AccelerationU =
      typename std::remove_cv_t<decltype(DistanceU(0) / s / s)>;
  Time _accel_time;
  Time _total_time;
  Time _deccel_time;
  VelocityU _max_speed;
  AccelerationU _max_acceleration, _max_decceleration;
  bool is_negative;
  DistanceU total_distance;

 public:
  TrapezoidalMotionProfile<DistanceU>(DistanceU distance, VelocityU max_speed,
                                      AccelerationU max_acceleration)
      : TrapezoidalMotionProfile<DistanceU>(
            distance, max_speed, max_acceleration, max_acceleration) {}

  TrapezoidalMotionProfile<DistanceU>(DistanceU distance, VelocityU max_speed,
                                      AccelerationU max_acceleration,
                                      AccelerationU max_decceleration) {
    total_distance = distance;
    if (distance < DistanceU(0)) {
      distance = -distance;
      is_negative = true;
    } else {
      is_negative = false;
    }
    _accel_time = max_speed / max_acceleration;
    _deccel_time = max_speed / max_decceleration;
    _max_speed = max_speed;
    _max_acceleration = max_acceleration;
    _max_decceleration = max_decceleration;

    if (distance > (_accel_time + _deccel_time) * _max_speed / 2) {
      _total_time = _accel_time + _deccel_time +
                    (distance - _max_speed * (_deccel_time + _accel_time) / 2) /
                        _max_speed;
    } else {
      DistanceU accel_dist = distance / 2;

      _accel_time = std::sqrt((2 * accel_dist / max_acceleration)()) * s;
      _deccel_time = std::sqrt((2 * accel_dist / max_decceleration)()) * s;
      _max_speed = _accel_time * _max_acceleration;
      _total_time = _accel_time + _deccel_time;
    }
  }

  virtual ~TrapezoidalMotionProfile() {}
  AccelerationU CalculateSecondDerivative(Time time) override {
    AccelerationU accel;
    if (time < _accel_time) {
      accel = _max_acceleration;
    } else if (time < _total_time - _deccel_time) {
      accel = AccelerationU(0);
    } else if (time < _total_time) {
      accel = -_max_decceleration;
    } else {
      accel = AccelerationU(0);
    }
    return is_negative ? -accel : accel;
  }
  VelocityU CalculateDerivative(Time time) override {
    VelocityU speed = 0;

    if (time <= _accel_time) {
      speed = _max_speed * (time / _accel_time);
    } else if (time <= _total_time - _deccel_time) {
      speed = _max_speed;
    } else if (time <= _total_time) {
      speed = _max_speed * ((_total_time - time) / _deccel_time);
    }

    return is_negative ? -speed : speed;
  }
  DistanceU Calculate(Time time) override {
    DistanceU accel_dist = _max_speed * _accel_time / 2;
    DistanceU deccel_dist = _max_speed * _deccel_time / 2;
    DistanceU full_speed_dist =
        _max_speed * (_total_time - _deccel_time - _accel_time);

    DistanceU distance =
        accel_dist + full_speed_dist + _deccel_time * _max_speed / 2;

    if (time <= _accel_time) {
      distance = time * time * _max_speed / _accel_time / 2;
    } else if (time <= _total_time - _deccel_time) {
      distance = accel_dist + _max_speed * (time - _accel_time);
    } else if (time <= _total_time) {
      Time time_left = _total_time - time;
      DistanceU dist_left =
          time_left * time_left * _max_speed / _accel_time / 2;
      distance = accel_dist + full_speed_dist + (deccel_dist - dist_left);
    }

    return is_negative ? -distance : distance;
  }
  bool finished(Time time) override { return time > _total_time; }
  DistanceU GetTotalDistance() override { return total_distance; }
};
}

#endif /* SRC_ROBOTCODE_TRAPEZOIDALMOTIONPROFILE_H_ */
