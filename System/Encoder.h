#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

#define ENCODER_A 12
#define ENCODER_B 14

#define PULSE_PER_RND 2400
#define TRANS_RATIO 2.4

class Encoder {
private:
  static void IRAM_ATTR encoderISR();

public:
  static volatile int encoderPulse;

  Encoder();
  void setInterrupt();
};

#endif
