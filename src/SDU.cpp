// -------------- NoRi -----------------------------------------
#include "SDU.hpp"

void setupSDUpdater(const char* appName);
bool SdBegin();
void prt(String sData, int direction = D3_BOTH);
void prtln(String sData, int direction = D3_BOTH);
void POWER_OFF();
void REBOOT();

int SDCARD_CS_PIN = 4;

// -------------------------------------------------------------
bool SdBegin() {
    // --- SD begin -------
    int i = 0;
    bool success = false;
    prtln("SD.begin Start", D1_SERI);

    while (i < 3) {  // SDカードマウント待ち
        success = SD.begin(SDCARD_CS_PIN, SPI, 4000000U, "/sd", 10U, false);
        if (success)
            return true;

        prtln("SD Wait...", D1_SERI);
        delay(500);
        i++;
    }

    if (i >= 3) {
        prtln("SD.begin faile", D3_BOTH);
        return false;
    } else
        return true;
}

void prt(String sData, int direction = D3_BOTH) {
    switch (direction) {
        case D3_BOTH:
            M5.Log.print(sData.c_str());
            M5.Display.print(sData.c_str());
            break;

        case D2_DISP:
            M5.Display.print(sData.c_str());
            break;

        case D1_SERI:
            M5.Log.print(sData.c_str());
            break;

        default:
            break;
    }
}

void prtln(String sData, int direction = D3_BOTH) {
    String strData = sData + "\n";
    prt(strData, direction);
}

void setupSDUpdater(const char* appName) {
    SDUCfg.setLabelMenu("< Menu");  // load menu.bin
    SDUCfg.setLabelSkip("Skip");    // skip the lobby countdown and run the app
    SDUCfg.setAppName(appName);     // lobby screen label: application name
    checkSDUpdater(SD,              // filesystem (default=SD)
                   MENU_BIN,        // path to binary (default=/menu.bin, empty
                                    // string=rollback only)
                   TIMEOUT00,  // wait delay, (default=0, will be forced to 2000
                               // upon ESP.restart() )
                   SDCARD_CS_PIN);
}

void POWER_OFF() {
    prtln("\nPOWER_OFF", D3_BOTH);
    //   SPIFFS.end();
    SD.end();

    delay(3000);
    M5.Power.powerOff();
    for (;;) {
        delay(10);
    }
}

void REBOOT() {
    prtln("\nREBOOT", D3_BOTH);
    //   SPIFFS.end();
    SD.end();

    delay(3000);
    ESP.restart();
    for (;;) {
        delay(10);
    }
}
