#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include "LiquidCrystal_I2C.h"
#include "Page.h"
#include "MyFs.h"

enum Mode {
  PASSIVE,
  ACTIVE,
  FREE,
  NULL_MODE
};

extern Page Menu;
extern Page Regis;
extern Page Process;
extern Page SelectMode;
extern Page SetLevel;
extern Page PreRun;
extern Page Running;
extern Page Pause;

class PageManager {
private:
  LiquidCrystal_I2C lcd;
  Page* curPage;
  Mode mode;
  String detailMode;
  String userID = "NULL";
  String today = "NULL";
  bool run = false;
  bool process = false;
  uint64_t startRunTim = 0;

public:
  uint64_t runTim = 0;

  PageManager();

  void begin();
  void setPage(Page& page);
  void showPage();
  void showOpt();
  void goNextOpt();
  void goNextPage();
  void goPausePage();
  void goMenuPage();
  void setDetailMode();
  void regisID(String inputId, String inputToday);
  void setID();

  bool modeRunning() const;
  bool regisProcessing() const;
  Mode getMode() const;
  String getDetailMode() const;
  String getUserID() const;
};

#endif
