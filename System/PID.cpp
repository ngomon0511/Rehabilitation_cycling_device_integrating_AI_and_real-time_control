#include "PID.h"

PID::PID(float Kp, float Ki, float Kd) {
  _Kp = Kp;
  _Ki = Ki;
  _Kd = Kd;
  _prevError = 0;
  _integral = 0;
  _derivative = 0;
}

void PID::reset(void) {
  _prevError = 0;
  _integral = 0;
  _derivative = 0;
}

int16_t PID::compute(float input) {
  float error = setpoint - input;
  _integral += error;
  _derivative = error - _prevError;
  _prevError = error;

  float output = _Kp * error + _Ki * _integral + _Kd * _derivative;
  output = (int16_t)(constrain(output, -MAX_PWM_SIGNAL, MAX_PWM_SIGNAL));

  if (output == MAX_PWM_SIGNAL && error > 0) {
    _integral -= error;
  } else if (output == -MAX_PWM_SIGNAL && error < 0) {
    _integral -= error;
  }

  return output;
}
