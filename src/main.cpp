#include <M5Unified.h>
#include "ImageViewer.hpp"
#include "menu.hpp"
#include "util.hpp"

ImageViewer viewer;

void setup(void) {
    MODE_ST = MDM1;

    auto cfg = M5.config();
    M5.begin(cfg);
    sdu_lobby();

    disp_init();
    M5.Display.setTextScroll(true);
    delay(500);

    if (!viewer.begin()) {
        FOREVER_LOOP();
    }
    
    MODE_ST = MD00;
}

void loop(void) {
    if (MODE_ST == MD00) {
        M5.update();
        if (M5.BtnB.wasHold()) {
            prtln("BtnB was Hold, go for  SETTING MENU", D1_SERI);

            MODE_ST = MD01;  // menu mode first
            disp_init();
            setup_MDxx(MD01);
            delay(500);
        } else {
            viewer.update();
            delay(10);
        }
        delay(1);

    } else {
        loop_MDxx();    // loop for SETTING MENU
    }
}
