#include "Encoder.h"

volatile int Encoder::encoderPulse = 0; 
volatile byte preEncoderState = 0;
volatile byte curEncoderState = 0;

Encoder::Encoder() {
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
}

void Encoder::setInterrupt(){
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderISR, CHANGE); 
}

void Encoder::encoderISR() {
  preEncoderState = curEncoderState;
  curEncoderState = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);

  switch ((preEncoderState << 2) | curEncoderState) {
    case 0b1000:
    case 0b0001:
    case 0b1110:
    case 0b0111:
      encoderPulse++;
      break;

    case 0b0100:
    case 0b1101:
    case 0b0010:
    case 0b1011:
      encoderPulse--;
      break;
  }
}
