#include "PWM.h"

PWM::PWM(uint8_t EN_PIN, uint8_t PWM_PIN, uint8_t PWM_CHANNEL) {
  _EN_PIN = EN_PIN;
  _PWM_PIN = PWM_PIN;
  _PWM_CHANNEL = PWM_CHANNEL;
}

void PWM::initPWM() {
  pinMode(_EN_PIN, OUTPUT);
  pinMode(_PWM_PIN, OUTPUT);
  ledcSetup(_PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(_PWM_PIN, _PWM_CHANNEL);
  setPWM(0, 0);
}

void PWM::setPWM(bool ena, int duty) {
  digitalWrite(_EN_PIN, ena);
  ledcWrite(_PWM_CHANNEL, duty);
}

void PWM::onBTS() {
  digitalWrite(R_EN_PIN, HIGH);
  digitalWrite(L_EN_PIN, HIGH);
}

void PWM::offBTS() {
  digitalWrite(R_EN_PIN, LOW);
  digitalWrite(L_EN_PIN, LOW);
}
