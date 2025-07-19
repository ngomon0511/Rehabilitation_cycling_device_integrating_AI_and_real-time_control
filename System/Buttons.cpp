#include "Buttons.h"

volatile bool Buttons::checkChange = false;
volatile bool Buttons::checkConfirm = false;
volatile bool Buttons::checkStop = false;

volatile uint64_t Buttons::lastChange = 0;
volatile uint64_t Buttons::lastConfirm = 0;
volatile uint64_t Buttons::lastStop = 0;

void Buttons::begin() {
  pinMode(CHANGE_BTN_PIN, INPUT_PULLUP);
  pinMode(CONFIRM_BTN_PIN, INPUT_PULLUP);
  pinMode(STOP_BTN_PIN, INPUT_PULLUP);

  attachInterrupt(CHANGE_BTN_PIN, isrChange, FALLING);
  attachInterrupt(CONFIRM_BTN_PIN, isrConfirm, FALLING);
  attachInterrupt(STOP_BTN_PIN, isrStop, FALLING);
}

void IRAM_ATTR Buttons::isrChange() {
  uint64_t now = CountTime::read();
  if (now - lastChange > DEBOUNCE_TIME) {
    checkChange = true;
    lastChange = now;
  }
}

void IRAM_ATTR Buttons::isrConfirm() {
  uint64_t now = CountTime::read();
  if (now - lastConfirm > DEBOUNCE_TIME) {
    checkConfirm = true;
    lastConfirm = now;
  }
}

void IRAM_ATTR Buttons::isrStop() {
  uint64_t now = CountTime::read();
  if (now - lastStop > DEBOUNCE_TIME) {
    checkStop = true;
    lastStop = now;
  }
}
