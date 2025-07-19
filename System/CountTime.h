#ifndef COUNT_TIME_H
#define COUNT_TIME_H

#include <Arduino.h>

class CountTime {
public:
  static void begin();
  static uint64_t read();

private:
  static IRAM_ATTR hw_timer_t *timer;
};

#endif
