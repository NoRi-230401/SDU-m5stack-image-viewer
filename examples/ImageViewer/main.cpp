// clang-format off
#include "ImageViewer.hpp"
// clang-format on

void setup_MDxx(int mode);
void loop_MDxx(void);
void disp_init();

#define MDM2   -2    // init
#define MDM1   -1    // setup start
#define MD00   0     // setup don (Normal mode)
#define MD01   1     // MD01  AutoMode On/Off
#define MD02   2     // MD02  load menu.bin
#define MD03   3     // MD03  save bin to SD
#define MD04   4     // MD04  power off
#define MD_END 4     //
int MODE_ST = MDM2;  // mode status = init
#define D1_SERI 1
#define D2_DISP 2
#define D3_BOTH 3

extern void prt(String sData, int direction = D3_BOTH);
extern void prtln(String sData, int direction = D3_BOTH);
extern void loadMenu(void);
extern void saveBin(void);
extern void POWER_OFF();
extern void REBOOT();
// extern void MDxx_BtnChk();
extern void FOREVER_LOOP();
void MD00_disp();
// void MDxx_BtnChk();
void doWork(int mode);

String get_MDxx_msg(int mode);

ImageViewer viewer;

void forever(void) {
    while (true) {
        delay(1);
    }
}

extern void sdu_lobby();

void setup(void) {
    MODE_ST = MDM1;
    auto cfg = M5.config();
    M5.begin(cfg);
    sdu_lobby();
    disp_init();
    M5.Display.setTextScroll(true);
    delay(500);

    if (!viewer.begin()) {
        forever();
    }
    MODE_ST = MD00;
}

void loop(void) {
    if (MODE_ST == MD00) {
        M5.update();
        if (M5.BtnB.wasHold()) {
            prtln("BtnB was Hold ,  goto Special Mode", D1_SERI);

            // 何故か１回目の文字が乱れるのでDUMMYを入れ２回呼び出す。
            // MODE_ST = MD_END;    // *** DUMMY ****
            // setup_MDxx(MD_END);  // **************
            MODE_ST = MD01;  // menu mode in
            disp_init();
            setup_MDxx(MD01);
            delay(500);
        } else {
            viewer.update();
            delay(10);
        }

        delay(1);
    } else {
        loop_MDxx();
    }
}

// void MDxx_BtnChk() {
//     if (M5.BtnA.wasClicked()) {
//         prtln("BtnA Cliked! [EXIT]", D1_SERI);
//         // MD00_disp();
//         MODE_ST = MD00;  // -- normal mode
//     } else if (M5.BtnB.wasClicked()) {
//         prtln("BtnB Cliked!  [OK]", D1_SERI);
//         doWork(MODE_ST);
//         // MDxx_disp(MODE_ST);
//     } else if (M5.BtnC.wasClicked()) {
//         prtln("BtnC Cliked!  [NEXT]", D1_SERI);
//         MODE_ST++;
//         if (MODE_ST > MD_END)
//             MODE_ST = MD01;

//         // MDxx_disp(MODE_ST);
//     }
// }

void MD00_disp() {
    // M5.Display.setTextFont(1);
    // M5.Display.setTextSize(2);
    // M5.Display.fillScreen(BLACK);

    disp_init();
    // M5.Display.fillScreen(TFT_BLACK);
    // M5.Display.setFont(&fonts::DejaVu18);
    // M5.Display.setTextColor(WHITE, BLACK);
    // M5.Display.setTextDatum(0);
    // M5.Display.setCursor(0, 0);

    M5.Display.setTextScroll(true);
    M5.Display.printf("***  SDU-imageViewer  ***\n\n\n");
    M5.Display.printf("(BtnA)click: PREV image\n\n");
    M5.Display.printf("(BtnB)hold : Special Mode\n\n");
    M5.Display.printf("(BtnC)click: NEXT image\n");
    // delay(1000);
}

String get_MDxx_msg(int mode) {
    String msg = "";

    switch (mode) {
        case MD01:
            if (viewer.isAutoMode())
                msg = "AutoMode : ON";
            else
                msg = "AutoMode : OFF";
            break;

        case MD02:
            msg = "Load SD-Updater menu.bin";
            break;

        case MD03:
            msg = "Save bin-file to SD";
            break;

        case MD04:
            msg = "Power Off";
            break;

        default:
            break;
    }

    return msg;
}

void doWork(int mode) {
    switch (mode) {
        case MD01:
            viewer.setAutoMode(!viewer.isAutoMode());
            break;

        case MD02:
            prtln("Will Load SD-Updater menu.bin", D1_SERI);
            loadMenu();
            FOREVER_LOOP();
            break;

        case MD03:
            prtln("Will Save bin_file to SD", D1_SERI);
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

#include <M5Unified.h>

//----------------------------------------------------------------

static int menu_x = 2;
static int menu_y = 20;
static int menu_w = 120;
static int menu_h = 30;
static int menu_padding = 36;

static void func_hello() {
    // for (int i = 0; i < 128; ++i) {
    for (int i = 0; i < 5; ++i) {
        M5.Display.drawString("hello", rand() % M5.Display.width(),
                              rand() % M5.Display.height());
    }
}

#define SX1 170    // width(320) / 2  + 10
#define SY1  90    // height(240) / 2 - 30 

#define SX2 170    //  = sx1 
#define SY2 120    //  sy1 + 30


static void func01_on() {
    prtln("AutoMode set on", D1_SERI);
    viewer.setAutoMode(true);

    M5.Display.setTextScroll(false);
    M5.Display.setTextDatum(0);
    M5.Display.setCursor(SX1, SY1);
    M5.Display.print("  AutoMode :    ");
    M5.Display.setCursor(SX2, SY2);
    M5.Display.print("      --> ON    ");

    // M5.Display.drawString("AutoMode:  ", SX1, SY1);
    // M5.Display.drawString("  -->  ON  ", SX2, SY2);

    delay(100);
}

static void func01_off() {
    prtln("AutoMode set off", D1_SERI);
    viewer.setAutoMode(false);

    M5.Display.setTextScroll(false);
    M5.Display.setTextDatum(0);
    M5.Display.setCursor(SX1, SY1);
    M5.Display.print("  AutoMode :    ");
    M5.Display.setCursor(SX2, SY2);
    M5.Display.print("      --> OFF ");

    // M5.Display.drawString("AutoMode:  ", SX1, SY1);
    // M5.Display.drawString("  -->  OFF ", SX2, SY2);

    delay(100);
}

static void func02_SDU_menu() {
    prtln("Will Load SD-Updater menu.bin", D1_SERI);
    M5.Display.drawString("Menu.bin   ", SX1, SY1);
    delay(3000);
    loadMenu();
    FOREVER_LOOP();
}

static void func03_SDU_saveBin() {
    prtln("Will Save bin_file to SD", D1_SERI);
    M5.Display.drawString("save bin   ", SX1, SY1);
    delay(3000);
    saveBin();
    delay(500);
}

static void func04_PowerOff() {
    prtln("PowerOff", D1_SERI);
    M5.Display.setTextScroll(false);
    M5.Display.setTextDatum(0);
    M5.Display.setCursor(SX1, SY1);
    M5.Display.print("  Power Off    ");
    delay(3000);

    M5.Power.powerOff();
    FOREVER_LOOP;
}

static void func_rect() {
    int w = M5.Display.width() >> 3;
    int h = M5.Display.height() >> 3;
    for (int i = 0; i < 128; ++i) {
        M5.Display.fillRect(rand() % M5.Display.width(),
                            rand() % M5.Display.height(), w, h,
                            (i & 1) ? TFT_BLACK : TFT_WHITE);
    }
}
static void func_line() {
    for (int i = 0; i < 128; ++i) {
        M5.Display.drawLine(
            rand() % M5.Display.width(), rand() % M5.Display.height(),
            rand() % M5.Display.width(), rand() % M5.Display.height(),
            (i & 1) ? TFT_BLACK : TFT_WHITE);
    }
}
static void func_circle() {
    int r = (M5.Display.width() + M5.Display.height()) >> 4;
    for (int i = 0; i < 64; ++i) {
        M5.Display.fillCircle(rand() % M5.Display.width(),
                              rand() % M5.Display.height(), r,
                              (i & 1) ? TFT_BLACK : TFT_WHITE);
    }
}
static void func_triangle() {
    for (int i = 0; i < 32; ++i) {
        M5.Display.fillTriangle(
            rand() % M5.Display.width(), rand() % M5.Display.height(),
            rand() % M5.Display.width(), rand() % M5.Display.height(),
            rand() % M5.Display.width(), rand() % M5.Display.height(),
            (i & 1) ? TFT_BLACK : TFT_WHITE);
    }
}
static void func_arc() {
    int r = (M5.Display.width() + M5.Display.height()) >> 4;
    for (int i = 0; i < 64; ++i) {
        M5.Display.fillArc(rand() % M5.Display.width(),
                           rand() % M5.Display.height(), r, r >> 1,
                           rand() % 360, rand() % 360,
                           (i & 1) ? TFT_BLACK : TFT_WHITE);
    }
}

/// メニュー用の構造体。タイトルの文字列と対応する関数のポインタを持つ
struct menu_item_t
{
    const char *title;
    void (*func)(void);
};

/// メニュー01の定義
static const menu_item_t menu01[] = {
    {"on", func01_on}, {"off", func01_off}, {"hello", func_hello},
    //  {"line", func_line},
    // {"circle", func_circle},
    // {"triangle", func_triangle},
    //  {"arc", func_arc},
};
/// メニュー02の定義
static const menu_item_t menu02[] = {
    {"SDU-menu", func02_SDU_menu},
    // {"rect", func_rect},
    // {"line", func_line},
};
/// メニュー03の定義
static const menu_item_t menu03[] = {
    {"SaveBin", func03_SDU_saveBin},
    // {"rect", func_rect},
};

/// メニュー04の定義
static const menu_item_t menu04[] = {
    {"PowerOff", func04_PowerOff},
};

/// メニュー01の要素数
static constexpr const size_t menu01_count = sizeof(menu01) / sizeof(menu01[0]);
static size_t menu_count = menu01_count;

/// メニュー02の要素数
static constexpr const size_t menu02_count = sizeof(menu02) / sizeof(menu02[0]);

/// メニュー03の要素数
static constexpr const size_t menu03_count = sizeof(menu03) / sizeof(menu03[0]);

/// メニュー04の要素数
static constexpr const size_t menu04_count = sizeof(menu04) / sizeof(menu04[0]);

/// 現在カーソルのある位置
size_t cursor_index = 0;

void draw_menu(size_t index, bool focus) {
    auto baseColor = M5.Display.getBaseColor();
    M5.Display.setColor(focus ? baseColor : ~baseColor);
    M5.Display.drawRect(menu_x, menu_y + index * menu_padding, menu_w, menu_h);
    M5.Display.drawRect(menu_x + 1, menu_y + index * menu_padding + 1,
                        menu_w - 2, menu_h - 2);
    M5.Display.setColor(focus ? ~baseColor : baseColor);
    M5.Display.fillRect(menu_x + 2, menu_y + index * menu_padding + 2,
                        menu_w - 4, menu_h - 4);
    M5.Display.setTextDatum(textdatum_t::middle_center);
    M5.Display.setTextColor(focus ? baseColor : ~baseColor,
                            focus ? ~baseColor : baseColor);

    switch (MODE_ST) {
        case MD01:
            M5.Display.drawString(
                menu01[index].title, menu_x + (menu_w >> 1),
                menu_y + index * menu_padding + (menu_h >> 1));
            break;

        case MD02:
            M5.Display.drawString(
                menu02[index].title, menu_x + (menu_w >> 1),
                menu_y + index * menu_padding + (menu_h >> 1));
            break;

        case MD03:
            M5.Display.drawString(
                menu03[index].title, menu_x + (menu_w >> 1),
                menu_y + index * menu_padding + (menu_h >> 1));
            break;

        case MD04:
            M5.Display.drawString(
                menu04[index].title, menu_x + (menu_w >> 1),
                menu_y + index * menu_padding + (menu_h >> 1));
            break;
    }
}

void select_menu(size_t index) {
    /// 操作音を鳴らす。
    float Hz = 880 * powf(2.0, index / 12.0f);
    // M5.Speaker.tone(Hz, 100);
    cursor_index = index;
}

void move_menu(bool back = false) {
    switch (MODE_ST) {
        case MD01:
            if (back) {
                select_menu((cursor_index ? cursor_index : menu01_count) - 1);
            } else {
                select_menu((cursor_index + 1) % menu01_count);
            }
            break;

        case MD02:
            if (back) {
                select_menu((cursor_index ? cursor_index : menu02_count) - 1);
            } else {
                select_menu((cursor_index + 1) % menu02_count);
            }
            break;

        case MD03:
            if (back) {
                select_menu((cursor_index ? cursor_index : menu03_count) - 1);
            } else {
                select_menu((cursor_index + 1) % menu03_count);
            }
            break;

        case MD04:
            if (back) {
                select_menu((cursor_index ? cursor_index : menu04_count) - 1);
            } else {
                select_menu((cursor_index + 1) % menu04_count);
            }
            break;
    }
}

void exec_menu(bool holding) {
    // holding は長押し中とそれ以外で処理を変えたい場合に利用できる。
    // if (holding == false) {
    //     // M5.Speaker.tone(880, 150);
    //     prtln("exec_menu : holding is false : cliced!", D1_SERI);
    // } else {
    //     prtln("exec_menu : holding is true : hold!", D1_SERI);
    // }

    switch (MODE_ST) {
        case MD01:
            if (menu01[cursor_index].func != nullptr) {
                // M5.Display.setClipRect(menu_x + menu_w + 5, 0,M5.Display.width(),M5.Display.height() - 30);
                menu01[cursor_index].func();
                // M5.Display.clearClipRect();
            }
            break;

        case MD02:
            if (menu02[cursor_index].func != nullptr) {
                // M5.Display.setClipRect(menu_x + menu_w + 5, 0,
                //                        M5.Display.width(),
                //                        M5.Display.height() - 30);
                menu02[cursor_index].func();
                // M5.Display.clearClipRect();
            }
            break;

        case MD03:
            if (menu03[cursor_index].func != nullptr) {
                // M5.Display.setClipRect(menu_x + menu_w + 5, 0,
                //                        M5.Display.width(),
                //                        M5.Display.height() - 30);
                menu03[cursor_index].func();
                // M5.Display.clearClipRect();
            }
            break;

        case MD04:
            if (menu04[cursor_index].func != nullptr) {
                // M5.Display.setClipRect(menu_x + menu_w + 5, 0,
                //                        M5.Display.width(),
                //                        M5.Display.height() - 30);
                menu04[cursor_index].func();
                // M5.Display.clearClipRect();
            }
            break;
    }
}

size_t get_menu_count(int mode);

size_t get_menu_count(int mode) {
    size_t tmp = 1;

    switch (mode) {
        case MD01:
            tmp = menu01_count;
            break;

        case MD02:
            tmp = menu02_count;
            break;
        case MD03:
            tmp = menu03_count;
            break;

        case MD04:
            tmp = menu04_count;
            break;

        default:
            tmp = 1;
    }

    return tmp;
}

void disp_init() {
    M5.Display.endWrite();

    M5.Display.fillScreen(TFT_BLACK);  // 画面クリア
    M5.Display.setRotation(1);
    M5.Display.setTextSize(1.0);
    M5.Display.setFont(&fonts::DejaVu18);           // フォントセット
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);  // 描画色、背景色を設定。
    M5.Display.setTextDatum(0);
    M5.Display.setCursor(0, 0);  // カーソルセット
    M5.Display.setTextScroll(false);
    M5.Display.setTextWrap(false);     // テキスト自動折返し
    M5.Display.fillScreen(TFT_BLACK);  // 画面クリア

    delay(20);

    // M5.Display.startWrite();
    // M5.Display.setEpdMode(epd_mode_t::epd_fastest);
}

void setup_MDxx(int mode) {
    M5.Display.fillScreen(TFT_BLACK);
    delay(20);

    cursor_index = 0;
    // M5.Display.setFont(&fonts::DejaVu18);

    menu_w = M5.Display.width() >> 1;
    menu_count = get_menu_count(mode);
    menu_padding = 50;
    menu_h = menu_padding - 10;
    menu_x = 0;
    menu_y = 50;

    prtln("mode = " + String(mode, 10), D1_SERI);
    prtln("menu_x = " + String(menu_x, 10), D1_SERI);
    prtln("menu_y = " + String(menu_y, 10), D1_SERI);
    prtln("menu_w = " + String(menu_w, 10), D1_SERI);
    prtln("menu_h = " + String(menu_h, 10), D1_SERI);
    prtln("menu_padding = " + String(menu_padding, 10), D1_SERI);
    prtln("menu_count = " + String(menu_count, 10), D1_SERI);

    /// このサンプルでは、startWriteをしたまま、対になるendWriteを使わないようにする。
    M5.Display.startWrite();
    M5.Display.setEpdMode(epd_mode_t::epd_fastest);
    // M5.Display.fillScreen(TFT_BLACK);

    M5.Display.setCursor(menu_x, 0);
    String msg = "MENU  " + String(mode, 10) + " / " + String(MD_END, 10);
    M5.Display.print(msg.c_str());

    M5.Display.setCursor(20, 27);
    msg = get_MDxx_msg(mode);
    M5.Display.print(msg.c_str());

    // msg = "123456789012345678901234567890";
    // M5.Display.setCursor(0, M5.Display.height() - 40);
    // M5.Display.print(msg.c_str());
    msg = "     prev         exit         next";
    M5.Display.setCursor(0, M5.Display.height() - 20);
    M5.Display.print(msg.c_str());

    for (size_t i = 0; i < menu_count; i++) {
        // draw_menu(i, i == cursor_index);
        draw_menu(i, false);
    }
}

void loop_MDxx(void) {
    /// ディスプレイがビジー状態でない場合のみ処理する。
    /// EPDでは画面の更新中はBusyとなるためここを通らない。
    // if (!M5.Display.displayBusy()) {
    //     // 選択しているメニューが変更されていれば再描画。

    //     static size_t prev_index = 0;
    //     if (prev_index != cursor_index) {
    //         draw_menu(prev_index, false);
    //         draw_menu(cursor_index, true);
    //         prev_index = cursor_index;
    //     }
    //     /// 表示内容を画面に反映する。
    //     M5.Display.display();
    //     /// ※ M5Paper, CoreInk, Unit OLED についてはここで画面が更新される。
    // }

    {  /// 10ミリ秒間隔で処理が進むように待機する。
        static uint32_t prev_ms;
        uint32_t ms = M5.millis();
        int diff = (10 - (ms - prev_ms));
        if (diff > 0) {
            ms += diff;
            M5.delay(diff);
        }
        prev_ms = ms;
    }

    M5.update();

    auto touch_count = M5.Touch.getCount();
    for (size_t i = 0; i < touch_count; i++) {
        auto detail = M5.Touch.getDetail(i);
        if (((size_t)detail.x - menu_x) < menu_w) {
            size_t index = (detail.y - menu_y) / menu_padding;
            if (index < menu_count) {
                if (detail.wasPressed()) {
                    select_menu(index);
                    ;
                } else if (index == cursor_index) {
                    if (detail.wasClicked()) {
                        exec_menu(false);
                    }
                    // else if (detail.isHolding()) {
                    //     // exec_menu(true);
                    //     ;
                    // }
                }
            }
        }
    }

    if (M5.BtnA.wasClicked()) {
        prtln("BtnA Cliked! [prev]", D1_SERI);
        MODE_ST--;
        if (MODE_ST < MD01)
            MODE_ST = MD_END;

        disp_init();
        setup_MDxx(MODE_ST);

    } else if (M5.BtnC.wasClicked()) {
        prtln("BtnC Cliked!  [next]", D1_SERI);
        MODE_ST++;
        if (MODE_ST > MD_END)
            MODE_ST = MD01;

        disp_init();
        setup_MDxx(MODE_ST);

    } else if (M5.BtnB.wasClicked()) {
        prtln("BtnB Cliked!  [exit]", D1_SERI);
        MODE_ST = MD00;
        disp_init();
        MD00_disp();
        delay(1000);
    }
}
