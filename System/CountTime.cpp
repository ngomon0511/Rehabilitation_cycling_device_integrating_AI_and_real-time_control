#include "CountTime.h"

IRAM_ATTR hw_timer_t* CountTime::timer = nullptr;

void CountTime::begin() {
  // Timer 1 on Timer Group 0 - 1 us resolution
  timer = timerBegin(1, 80, true);  
  timerStart(timer);
}

uint64_t CountTime::read() {
  return timerRead(timer);
}
