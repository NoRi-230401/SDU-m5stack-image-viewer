// clang-format off
#include "ImageViewer.hpp"
// clang-format on

#define MDM2   -2    // init
#define MDM1   -1    // setup start
#define MD00   0     // setup don (Normal mode)
#define MD01   1     // MD01  FileSystem change
#define MD02   2     // MD02  load menu.bin
#define MD03   3     // MD03  save bin to SD
#define MD04   4     // MD04  power off
#define MD_END 4     //
int MODE_ST = MDM2;  // mode status
#define D1_SERI 1
#define D2_DISP 2
#define D3_BOTH 3

extern void prt(String sData, int direction = D3_BOTH);
extern void prtln(String sData, int direction = D3_BOTH);
extern void loadMenu(void);
extern void saveBin(void);
extern void POWER_OFF();
extern void REBOOT();
extern void MDxx_BtnChk();
extern void MDxx_disp(int mode);

void MDxx_BtnChk();
void doWork(int mode);
void MD00_disp();
void MDxx_disp(int mode);

ImageViewer viewer;

void forever(void) {
    while (true) {
        delay(1);
    }
}

void setup(void) {
    MODE_ST = MDM1;
    if (!viewer.begin()) {
        forever();
    }
    MODE_ST = MD00;
}

// extern bool AUTOMODE_ST;
void loop(void) {
    M5.update();

    if (MODE_ST == MD00) {
        if (M5.BtnB.wasHold()) {
            // AUTOMODE_ST = viewer.isAutoMode();
            prtln("BtnB was Hold ,  goto Special Mode", D1_SERI);
            MODE_ST = MD01;  // Special Mode in
            MDxx_disp(MODE_ST);
            delay(100);
        } else
            viewer.update();

    } else if ((MODE_ST > MD00) && (MODE_ST <= MD_END))
    {
        MDxx_BtnChk();
    }

    // delay(100);
    delay(20);
}


void MDxx_BtnChk() {
    if (M5.BtnA.wasClicked()) {
        prtln("BtnA Cliked! [EXIT]", D1_SERI);
        MD00_disp();
        MODE_ST = MD00;  // -- normal mode
        delay(100);
    } else if (M5.BtnB.wasClicked()) {
        prtln("BtnB Cliked!  [OK]", D1_SERI);
        doWork(MODE_ST);
        MDxx_disp(MODE_ST);
    } else if (M5.BtnC.wasClicked()) {
        prtln("BtnC Cliked!  [NEXT]", D1_SERI);
        MODE_ST++;
        if (MODE_ST > MD_END)
            MODE_ST = MD01;

        MDxx_disp(MODE_ST);
    }
}


void MD00_disp() {
    M5.Display.setTextFont(1);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setTextDatum(0);
    M5.Display.setCursor(0, 0);
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextScroll(true);
    M5.Display.printf("***  SDU-imageViewer  ***\n\n\n");
    M5.Display.printf("(BtnA)click: NEXT image\n\n");
    M5.Display.printf("(BtnB)hold : Special Mode\n\n");
    M5.Display.printf("(BtnC)click: PREV image\n");
}

void MDxx_disp(int mode) {
    M5.Display.setTextFont(1);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLUE);
    M5.Display.fillScreen(BLUE);
    M5.Display.setTextDatum(0);
    M5.Display.setCursor(0, 0);
    M5.Display.setTextScroll(false);

    M5.Display.printf("**  Special Mode %d/%d  **\n\n", MODE_ST, MD_END);
    String msg = "";
    switch (MODE_ST) {
        case MD01:
            // if (AUTOMODE_ST)
            if (viewer.isAutoMode())
                msg = "AutoMode : ON  -> OFF";
            else
                msg = "AutoMode : OFF -> ON";
            break;

        case MD02:
            msg = "Load SD-Updater menu.bin";
            break;

        case MD03:
            msg = "Store bin-file to SD";
            break;

        case MD04:
            msg = "Power Off";
            break;

        default:
            break;
    }
    prtln(msg, D2_DISP);

    M5.Display.printf("\n\n\n(BtnA)click:    EXIT\n\n");
    M5.Display.printf("(BtnB)click:    OK\n\n");
    M5.Display.print("(BtnC)click:    NEXT\n");
}

void doWork(int mode) {
    switch (mode) {
        case MD01:
            viewer.setAutoMode(!viewer.isAutoMode());
            // ImageViewer.setAutoMode();
            break;

        case MD02:
            M5.Log.println("Will Load menu binary");
            // updateFromFS(SD);
            loadMenu();
            for (;;) {
                delay(10);
            }
            // REBOOT();
            break;

        case MD03:
            M5.Log.println("Will store BIN_FILE to SD");
            // saveSketchToFS(SD, APP_BIN);
            saveBin();
            delay(500);
            break;

        case MD04:
            POWER_OFF();
            break;

        default:
            break;
    }
}
