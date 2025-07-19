#ifndef PID_H
#define PID_H

#include <Arduino.h>

#define MAX_PWM_SIGNAL 1023

class PID {
public:
  float setpoint = 0, _Kp, _Ki, _Kd;
  PID(float Kp, float Ki, float Kd);
  void reset(void);
  int16_t compute(float input);

private:
  float _prevError, _integral, _derivative;   
};

#endif
