#ifndef PAGE_H
#define PAGE_H

#include "LiquidCrystal_I2C.h"
#include "CountTime.h"

enum Type {
  MENU,
  REGIS,
  PROCESS,
  SELECT_MODE,
  SET_LEVEL,
  PRE_RUN,
  RUN,
  PAUSE
};

class Page {
public:
  String line1;
  const char* line2;
  const char* const* opts;
  uint8_t optCount;
  Type type;
  uint8_t curOpt = 0;

  Page(String l1, const char* const* optList, uint8_t optNum, Type pageType);
  Page(String l1, Type pageType);

  void showLine1(LiquidCrystal_I2C& lcd);
  void showLine2(LiquidCrystal_I2C& lcd);
  void blinkOpt(LiquidCrystal_I2C& lcd);
  void nextOpt(LiquidCrystal_I2C& lcd);
  void setModeLine1(String newLine1);
  void setIDLine1(String ID);

private:
  bool showState = true;
  uint64_t lastShow = 0;
  uint64_t lastHide = 0;
  const uint64_t showInterval = 500000;  // 500 ms
  const uint64_t hideInterval = 300000;  // 300 ms
};

#endif
