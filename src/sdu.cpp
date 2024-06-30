#include <SD.h>
#include <M5Unified.h>
#include <ESP32-targz.h>
#include <M5StackUpdater.h>
#include "sdu.hpp"
#include "menu.hpp"

#define APP_VER "SDU-imageViewer-v201-230624"
#define APP_NAME "SDU-imageViewer" // app Name
#define APP_BIN "/21_imgView.bin"  // app bin file-name
#define TIMEOUT00 5000             // lobby countdouwn timeout : msec

bool SdBegin();
void setupSDUpdater(const char *appName);
void sdu_lobby();
void loadMenu(void);
void saveBin(void);

void loadMenu(void)
{
  updateFromFS(SD);
}

void saveBin(void)
{
  saveSketchToFS(SD, APP_BIN);
}

bool SdBegin()
{
  // --- SD begin -------
  int i = 0;
  bool success = false;
  prtln("SD.begin Start", D1_SERI);

  while (i < 3)
  { // SDカードマウント待ち
    success = SD.begin((int)M5.getPin(m5::sd_spi_cs), SPI, 4000000U, "/sd",
                       10U, false);
    if (success)
      return true;

    prtln("SD Wait...", D1_SERI);
    delay(500);
    i++;
  }

  if (i >= 3)
  {
    prtln("SD.begin faile", D3_BOTH);
    return false;
  }
  else
    return true;
}

void setupSDUpdater(const char *appName)
{
  SDUCfg.setLabelMenu("< Menu"); // load menu.bin
  SDUCfg.setLabelSkip("Skip");   // skip the lobby countdown and run the app
  SDUCfg.setAppName(appName);    // lobby screen label: application name
  checkSDUpdater(SD,             // filesystem (default=SD)
                 MENU_BIN,       // path to binary (default=/menu.bin, empty
                                 // string=rollback only)
                 TIMEOUT00,      // wait delay, (default=0, will be forced to 2000
                                 // upon ESP.restart() )
                 (int)M5.getPin(m5::sd_spi_cs));
}

void sdu_lobby()
{
  setupSDUpdater(APP_NAME);
}
