#ifndef PWM_H
#define PWM_H

#include <Arduino.h>

#define R_EN_PIN 26
#define L_EN_PIN 13

#define R_PWM_PIN 27
#define L_PWM_PIN 33

#define PWM_RES 10
#define PWM_FREQ 20000

// High speed channel 0 & 1 of Timer 0 on Timer Group 0
#define R_PWM_CHANNEL 0
#define L_PWM_CHANNEL 1

class PWM {
private:
  uint8_t _EN_PIN, _PWM_PIN, _PWM_CHANNEL;

public:
  PWM(uint8_t EN_PIN, uint8_t PWM_PIN, uint8_t PWM_CHANNEL);

  void initPWM();
  // Both ena of R & L = 1 -> enable BTS driver
  static void onBTS();
  static void offBTS();
  void setPWM(bool ena, int duty); 
};

#endif
