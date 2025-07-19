#ifndef BUTTONS_H
#define BUTTONS_H

#include "CountTime.h"

#define CHANGE_BTN_PIN 16
#define CONFIRM_BTN_PIN 17
#define STOP_BTN_PIN 5

class Buttons {
public:
  static void begin();

  static void IRAM_ATTR isrChange();
  static void IRAM_ATTR isrConfirm();
  static void IRAM_ATTR isrStop();

  static volatile bool checkChange;
  static volatile bool checkConfirm;
  static volatile bool checkStop;

private:
  static volatile uint64_t lastChange;
  static volatile uint64_t lastConfirm;
  static volatile uint64_t lastStop;

  static const uint64_t DEBOUNCE_TIME = 500000;  // 500 ms
};

#endif
