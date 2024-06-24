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
extern void MDxx_BtnChk();
extern void MDxx_disp(int mode);
#define D1_SERI 1
#define D2_DISP 2
#define D3_BOTH 3
extern void prt(String sData, int direction = D3_BOTH);
extern void prtln(String sData, int direction = D3_BOTH);

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

void loop(void) {
    M5.update();

    if (MODE_ST == MD00) {
        if (M5.BtnB.wasHold()) {
            prtln("BtnB was Hold ,  goto Special Mode", D1_SERI);
            MODE_ST = MD01;  // Special Mode in
            MDxx_disp(MODE_ST);
            delay(100);
        } else
            viewer.update();

    } else if ((MODE_ST > MD00) && (MODE_ST <= MD_END))
        MDxx_BtnChk();

    // delay(100);
    delay(20);
}
