#include "PageManager.h"

//------------------------- Define page -------------------------
const char* menuOpts[] = { "    Start", "   User Id" };
Page Menu("   REHAB BIKE", menuOpts, 2, MENU);

const char* regisOpts[] = { "  New Login", "   Logout", "    Back" };
Page Regis(" ID: NULL", regisOpts, 3, REGIS);

Page Process("  WEB APP LOGIN", PROCESS);

const char* selectModeOpts[] = { "   Passive", "   Active", "    Free", "    Back" };
Page SelectMode("  SELECT MODE:", selectModeOpts, 4, SELECT_MODE);

const char* setLevelOpts[] = { "     1", "     2", "     3", "     4", "     5", "    Back" };
Page SetLevel("    SET LEVEL:", setLevelOpts, 6, SET_LEVEL);

const char* preRunOpts[] = { "     Run", "     Back" };
Page PreRun("", preRunOpts, 2, PRE_RUN);

Page Run("", RUN);

const char* pauseOpts[] = { "    Resume", "     Exit" };
Page Pause("", pauseOpts, 2, PAUSE);

//------------------------- PageManager Implementation -------------------------
PageManager::PageManager()
  : lcd(0x27, 16, 2), curPage(nullptr), mode(NULL_MODE), detailMode(""), run(false) {}

void PageManager::begin() {
  lcd.init();
  setPage(Menu);
}

void PageManager::setPage(Page& page) {
  curPage = &page;
  lcd.clear();
  curPage->curOpt = 0;

  switch (curPage->type) {
    case PRE_RUN:
      runTim = 0;
      setDetailMode();
      break;
    case RUN:
      startRunTim = CountTime::read();
      setDetailMode();
      break;
    case PAUSE:
      runTim += CountTime::read() - startRunTim;
      setDetailMode();
      break;
    case REGIS:
      setID();
      break;
    default:
      break;
  }

  showPage();
}

void PageManager::showPage() {
  curPage->showLine1(lcd);
  curPage->showLine2(lcd);
}

void PageManager::showOpt() {
  curPage->blinkOpt(lcd);
}

void PageManager::goNextOpt() {
  curPage->nextOpt(lcd);
}

void PageManager::goNextPage() {
  Type curPageType = curPage->type;
  uint8_t curOptIdx = curPage->curOpt;

  switch (curPageType) {
    case MENU:
      if (curOptIdx == 0)
        setPage(SelectMode);
      else
        setPage(Regis);
      break;

    case REGIS:
      switch (curOptIdx) {
        case 0:
          setPage(Process);
          process = true;
          break;
        case 1:
          userID = "NULL";
          setPage(Regis);
          break;
        case 2:
          setPage(Menu);
          break;
      }
      break;

    case SELECT_MODE:
      switch (curOptIdx) {
        case 0:
          mode = PASSIVE;
          setPage(SetLevel);
          break;
        case 1:
          mode = ACTIVE;
          setPage(SetLevel);
          break;
        case 2:
          mode = FREE;
          detailMode = "Free";
          setPage(PreRun);
          break;
        case 3:
          setPage(Menu);
          break;
      }
      break;

    case SET_LEVEL:
      if (curOptIdx < 5) {
        detailMode = (mode == PASSIVE) ? "Passive" + String(curOptIdx + 1) : "Active" + String(curOptIdx + 1);
        setPage(PreRun);
      } else {
        setPage(SelectMode);
      }
      break;

    case PRE_RUN:
      if (curOptIdx == 0) {
        setPage(Run);
        run = true;
      } else {
        setPage(mode == FREE ? SelectMode : SetLevel);
      }
      break;

    case PAUSE:
      if (curOptIdx == 0) {
        setPage(Run);
        run = true;
      } else {
        if (userID != "NULL") {
          String modeStr = "";
          String levelStr = "";
          if (detailMode == "Free") {
            modeStr = "free";
            levelStr = "0";
          } else {
            levelStr = String(detailMode.charAt(detailMode.length() - 1));
            modeStr = detailMode.substring(0, detailMode.length() - 1);
            modeStr.toLowerCase();
          }

          float runTime = (float)runTim / 60000000;

          MyFs.addLesson(userID, today, modeStr, levelStr, String(runTime));
        }
        setPage(Menu);
        run = false;
        mode = NULL_MODE;
        detailMode = "";
      }
      break;

    default:
      break;
  }
}

void PageManager::goPausePage() {
  if (curPage->type == RUN) {
    setPage(Pause);
    run = false;
  }
}

void PageManager::goMenuPage() {
  if (curPage->type == PROCESS) {
    setPage(Menu);
    process = false;
  }
}

void PageManager::setDetailMode() {
  PreRun.setModeLine1(" MODE: " + detailMode);
  Run.setModeLine1(" MODE: " + detailMode);
  Pause.setModeLine1(" " + detailMode + ": PAUSE");
}

void PageManager::regisID(String inputId, String inputToday) {
  today = inputToday;
  userID = inputId;
  setPage(Regis);
  process = false;
}

void PageManager::setID() {
  Regis.setIDLine1(" ID: " + userID);
}

bool PageManager::modeRunning() const {
  return run;
}

bool PageManager::regisProcessing() const {
  return process;
}

Mode PageManager::getMode() const {
  return mode;
}

String PageManager::getDetailMode() const {
  return detailMode;
}

String PageManager::getUserID() const {
  return userID;
}
