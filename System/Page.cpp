#include "Page.h"

Page::Page(String l1, const char *const *optList, uint8_t optNum, Type pageType) {
  line1 = l1;
  opts = optList;
  optCount = optNum;
  type = pageType;
}

Page::Page(String l1, Type pageType) {
  line1 = l1;
  type = pageType;
}

void Page::showLine1(LiquidCrystal_I2C &lcd) {
  lcd.setCursor(0, 0);
  lcd.print("               ");
  lcd.setCursor(0, 0);
  lcd.print(line1);
}

void Page::showLine2(LiquidCrystal_I2C &lcd) {
  lcd.setCursor(0, 1);

  switch (type) {
    case MENU:
    case REGIS:
    case SELECT_MODE:
    case SET_LEVEL:
    case PRE_RUN:
    case PAUSE:
      lcd.print(">>");

      if (showState)
        lcd.print(opts[curOpt]);
      else
        lcd.print("              ");
      break;

    case RUN:
      lcd.print(">> Running ! <<");
      break;

    case PROCESS:
      lcd.print(">> Processing...");
      break;

    default:
      break;
  }
}

void Page::blinkOpt(LiquidCrystal_I2C &lcd) {
  if (type == RUN || type == PROCESS) return;

  uint64_t now = CountTime::read();

  if (showState && (now - lastShow >= showInterval)) {
    showState = false;
    lastHide = now;
  } else if (!showState && (now - lastHide >= hideInterval)) {
    showState = true;
    lastShow = now;
  }

  showLine2(lcd);
}

void Page::nextOpt(LiquidCrystal_I2C &lcd) {
  if (type == RUN || type == PROCESS) return;

  switch (type) {
    case MENU:
      lcd.setCursor(5, 1);
      lcd.print("        ");
      break;

    case REGIS:
      lcd.setCursor(4, 1);
      lcd.print("         ");
      break;

    case SELECT_MODE:
      lcd.setCursor(9, 1);
      lcd.print("   ");
      break;

    case SET_LEVEL:
      lcd.setCursor(8, 1);
      lcd.print("  ");
      break;

    case PRE_RUN:
      lcd.setCursor(10, 1);
      lcd.print(" ");
      break;

    case PAUSE:
      lcd.setCursor(11, 1);
      lcd.print(" ");
      break;

    default:
      break;
  }

  curOpt = (curOpt + 1) % optCount;
  showState = true;
  showLine2(lcd);
}

void Page::setModeLine1(String newLine1) {
  switch (type) {
    case PRE_RUN:
    case RUN:
    case PAUSE:
      line1 = newLine1;
      break;

    case MENU:
    case REGIS:
    case PROCESS:
    case SELECT_MODE:
    case SET_LEVEL:
    default:
      break;
  }
}

void Page::setIDLine1(String ID) {
  switch (type) {
    case REGIS:
      line1 = ID;
      break;

    case PRE_RUN:
    case RUN:
    case PAUSE:
    case MENU:
    case PROCESS:
    case SELECT_MODE:
    case SET_LEVEL:
    default:
      break;
  }
}
