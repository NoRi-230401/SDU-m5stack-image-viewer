#include <M5Unified.h>
#include "menu.hpp"
#include "util.hpp"
#include "ImageViewer.hpp"

void setup_MDxx(int mode);
void loop_MDxx();
void disp_init();

void draw_menu(size_t index, bool focus);
void select_menu(size_t index);
void exec_menu(bool holding);
size_t get_menu_count(int mode);
void MD00_disp();
void M5Disp(String msg, int32_t x, int32_t y);
String get_MDxx_msg(int mode);
void menuDisp(String msg, int lineNo);

static void func01_AUTOMODE_OFF();
static void func01_AUTOMODE_FORWARD();
static void func01_AUTOMODE_BACKWARD();
static void func01_AUTOMODE_RND();
static void func02_intval_01();
static void func02_intval_02();
static void func02_intval_03();
static void func03_intvalRnd_01();
static void func03_intvalRnd_02();
static void func04_SDU_menu();
static void func05_SDU_saveBin();
static void func06_PowerOff();

extern ImageViewer viewer;
int MODE_ST = MDM2; // mode status = init
static int menu_x = 0;
static int menu_y = 25;
static int menu_w = 120;
static int menu_h = 40;
static int menu_padding = 48;

// menu string disp area for M5Disp() --
#define SX1 165 // width(320) /  2  + 5
// #define SY1 90  // height(240) / 2 - 30
// #define SX2 165 //  = sx1
// #define SY2 120 //  sy1 + 30

void M5Disp(String msg, int32_t x, int32_t y)
{
  M5.Display.setTextScroll(false);
  M5.Display.setTextDatum(0);
  M5.Display.setCursor(x, y);
  M5.Display.print("                      ");
  ;
  M5.Display.setCursor(x, y);
  M5.Display.print(msg.c_str());
}

// lineNo : 0 to 6
void menuDisp(String msg, int lineNo)
{
  if(lineNo<0 || lineNo>6)
    return;

  M5.Display.setTextScroll(false);
  M5.Display.setTextDatum(0);

  int32_t y = (lineNo + 2) * 25;
  
  // space for void 
  M5.Display.setCursor(SX1, y);
  M5.Display.print("                      ");  
  
  M5.Display.setCursor(SX1, y);
  M5.Display.print(msg.c_str());
}

static void func01_AUTOMODE_OFF()
{
  prtln("AutoMode set OFF", D1_SERI);
  viewer.setAutoMode(AUTOMODE_OFF);

  menuDisp("AutoMode", 2);
  menuDisp(" -> off", 3);

  delay(100);
}

static void func01_AUTOMODE_FORWARD()
{
  prtln("AutoMode set FORWARD", D1_SERI);
  viewer.setAutoMode(AUTOMODE_FORWARD);

  menuDisp("AutoMode", 2);
  menuDisp(" -> forward", 3);
  delay(100);
}

static void func01_AUTOMODE_BACKWARD()
{
  prtln("AutoMode set BACKWARD", D1_SERI);
  viewer.setAutoMode(AUTOMODE_BACKRWARD);

  menuDisp("AutoMode", 2);
  menuDisp(" -> backward", 3);
  delay(100);
}

static void func01_AUTOMODE_RND()
{
  prtln("AutoMode set RND", D1_SERI);
  viewer.setAutoMode(AUTOMODE_RND);

  menuDisp("AutoMode", 2);
  menuDisp("-> random", 3);
  delay(100);
}

static void func02_intval_01()
{
  prtln("Interval 3sec", D1_SERI);
  menuDisp("Interval", 2);
  menuDisp("  -> 3sec", 3);
  viewer.setIntval(3000);
  delay(100);
}

static void func02_intval_02()
{
  prtln("Interval 5sec", D1_SERI);
  menuDisp("Interval", 2);
  menuDisp("  -> 5sec", 3);
  viewer.setIntval(5000);
  delay(100);
}

static void func02_intval_03()
{
  prtln("Interval 10sec", D1_SERI);
  menuDisp("Interval", 2);
  menuDisp("  -> 10sec", 3);
  viewer.setIntval(10000);
  delay(100);
}

static void func03_intvalRnd_01()
{
  prtln("Interval Rnd Off", D1_SERI);
  menuDisp("Interval Rnd", 2);
  menuDisp("  -> off", 3);
  viewer.setIntvalRnd(false);
  delay(100);
}

static void func03_intvalRnd_02()
{
  prtln("Interval Rnd On", D1_SERI);
  menuDisp("Interval Rnd", 2);
  menuDisp("  -> on", 3);
  viewer.setIntvalRnd(true);
  delay(100);
}

static void func04_SDU_menu()
{
  prtln("Will Load SD-Updater menu.bin", D1_SERI);
  menuDisp(" Load menu.bin", 2);
  delay(3000);
  disp_init();
  delay(100);
  loadMenu();
  FOREVER_LOOP();
}

static void func05_SDU_saveBin()
{
  prtln("Will Save bin_file to SD", D1_SERI);
  menuDisp(" Save bin to SD", 2);
  delay(3000);
  disp_init();
  delay(100);
  saveBin();
  delay(100);
  disp_init();
  delay(100);
  setup_MDxx(MD05);
  delay(100);
}

static void func06_PowerOff()
{
  prtln("PowerOff", D1_SERI);
  menuDisp(" Power Off", 2);
  delay(3000);
  M5.Power.powerOff();
  FOREVER_LOOP;
}

/// メニュー用の構造体。タイトルの文字列と対応する関数のポインタを持つ
struct menu_item_t
{
  const char *title;
  void (*func)(void);
};

/// メニュー01の定義
static const menu_item_t menu01[] = {
    {"off", func01_AUTOMODE_OFF},
    {"forward", func01_AUTOMODE_FORWARD},
    {"backward", func01_AUTOMODE_BACKWARD},
    {"random", func01_AUTOMODE_RND},
};

/// メニュー02の定義
static const menu_item_t menu02[] = {
    {"3sec", func02_intval_01},
    {"5sec", func02_intval_02},
    {"10sec", func02_intval_03},
};

/// メニュー03の定義
static const menu_item_t menu03[] = {
    {"off", func03_intvalRnd_01},
    {"on", func03_intvalRnd_02},
};

/// メニュー04の定義
static const menu_item_t menu04[] = {
    {"SDU-menu", func04_SDU_menu},
};
/// メニュー05の定義
static const menu_item_t menu05[] = {
    {"SaveBin", func05_SDU_saveBin},
};

/// メニュー06の定義
static const menu_item_t menu06[] = {
    {"PowerOff", func06_PowerOff},
};

/// メニュー要素数
static constexpr const size_t menu01_count = sizeof(menu01) / sizeof(menu01[0]);
static constexpr const size_t menu02_count = sizeof(menu02) / sizeof(menu02[0]);
static constexpr const size_t menu03_count = sizeof(menu03) / sizeof(menu03[0]);
static constexpr const size_t menu04_count = sizeof(menu04) / sizeof(menu04[0]);
static constexpr const size_t menu05_count = sizeof(menu05) / sizeof(menu05[0]);
static constexpr const size_t menu06_count = sizeof(menu06) / sizeof(menu06[0]);
static size_t menu_count = menu01_count;
/// 現在カーソルのある位置
size_t cursor_index = 0;

void draw_menu(size_t index, bool focus)
{
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

  switch (MODE_ST)
  {
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

  case MD05:
    M5.Display.drawString(
        menu05[index].title, menu_x + (menu_w >> 1),
        menu_y + index * menu_padding + (menu_h >> 1));
    break;

  case MD06:
    M5.Display.drawString(
        menu06[index].title, menu_x + (menu_w >> 1),
        menu_y + index * menu_padding + (menu_h >> 1));
    break;
  }
}

void select_menu(size_t index)
{
  /// 操作音を鳴らす。
  float Hz = 880 * powf(2.0, index / 12.0f);
  // M5.Speaker.tone(Hz, 100);
  cursor_index = index;
}

void exec_menu(bool holding)
{
  switch (MODE_ST)
  {
  case MD01:
    if (menu01[cursor_index].func != nullptr)
    {
      menu01[cursor_index].func();
    }
    break;

  case MD02:
    if (menu02[cursor_index].func != nullptr)
    {
      menu02[cursor_index].func();
    }
    break;

  case MD03:
    if (menu03[cursor_index].func != nullptr)
    {
      menu03[cursor_index].func();
    }
    break;

  case MD04:
    if (menu04[cursor_index].func != nullptr)
    {
      menu04[cursor_index].func();
    }
    break;

  case MD05:
    if (menu05[cursor_index].func != nullptr)
    {
      menu05[cursor_index].func();
    }
    break;

  case MD06:
    if (menu06[cursor_index].func != nullptr)
    {
      menu06[cursor_index].func();
    }
    break;
  }
}

size_t get_menu_count(int mode)
{
  size_t tmp = 1;

  switch (mode)
  {
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

  case MD05:
    tmp = menu05_count;
    break;

  case MD06:
    tmp = menu06_count;
    break;

  default:
    tmp = 1;
  }

  return tmp;
}

void disp_init()
{
  M5.Display.endWrite();
  M5.Display.fillScreen(TFT_BLACK); // 画面クリア
  M5.Display.setRotation(1);
  M5.Display.setTextSize(1.0);
  M5.Display.setFont(&fonts::DejaVu18);          // フォントセット
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK); // 描画色、背景色を設定。
  M5.Display.setTextDatum(0);
  M5.Display.setCursor(0, 0); // カーソルセット
  M5.Display.setTextScroll(false);
  M5.Display.setTextWrap(false); // テキスト自動折返し
}

void setup_MDxx(int mode)
{
  const int M_PADDING[4] = {70, 60, 50, 48};
  const int M_H[4] = {60, 50, 40, 40};
  const int M_Y[4] = {70, 60, 50, 25};
  String msg = "";
  M5.Display.fillScreen(TFT_BLACK);

  cursor_index = 0;
  menu_count = get_menu_count(mode);
  menu_x = 0;
  menu_y = M_Y[menu_count - 1];
  menu_w = M5.Display.width() >> 1;
  menu_h = M_H[menu_count - 1];
  menu_padding = M_PADDING[menu_count - 1];
  prtln("mode = " + String(mode, 10), D1_SERI);
  prtln("menu_count = " + String(menu_count, 10), D1_SERI);
  prtln("menu_x = " + String(menu_x, 10), D1_SERI);
  prtln("menu_y = " + String(menu_y, 10), D1_SERI);
  prtln("menu_w = " + String(menu_w, 10), D1_SERI);
  prtln("menu_h = " + String(menu_h, 10), D1_SERI);
  prtln("menu_padding = " + String(menu_padding, 10), D1_SERI);

  /// このサンプルでは、startWriteをしたまま、対になるendWriteを使わないようにする。
  // M5.Display.startWrite();
  // M5.Display.setEpdMode(epd_mode_t::epd_fastest);

  msg = get_MDxx_msg(mode);
  M5Disp(msg, 0, 0);

  msg = "MENU  " + String(mode, 10) + " / " + String(MD_END, 10);
  M5Disp(msg, menu_w + 20, 25);

  msg = "     prev         exit         next";
  M5Disp(msg, 0, M5.Display.height() - 20);

  for (size_t i = 0; i < menu_count; i++)
  {
    draw_menu(i, false);
  }
}

void loop_MDxx()
{
  { /// 10ミリ秒間隔で処理が進むように待機する。
    static uint32_t prev_ms;
    uint32_t ms = M5.millis();
    int diff = (10 - (ms - prev_ms));
    if (diff > 0)
    {
      ms += diff;
      M5.delay(diff);
    }
    prev_ms = ms;
  }

  M5.update();

  auto touch_count = M5.Touch.getCount();
  for (size_t i = 0; i < touch_count; i++)
  {
    auto detail = M5.Touch.getDetail(i);
    if (((size_t)detail.x - menu_x) < menu_w)
    {
      size_t index = (detail.y - menu_y) / menu_padding;
      if (index < menu_count)
      {
        if (detail.wasPressed())
        {
          select_menu(index);
          ;
        }
        else if (index == cursor_index)
        {
          if (detail.wasClicked())
          {
            exec_menu(false);
          }
        }
      }
    }
  }

  if (M5.BtnA.wasClicked())
  {
    prtln("BtnA Cliked! [prev]", D1_SERI);
    MODE_ST--;
    if (MODE_ST < MD01)
      MODE_ST = MD_END;

    disp_init();
    setup_MDxx(MODE_ST);
    delay(500);
  }
  else if (M5.BtnC.wasClicked())
  {
    prtln("BtnC Cliked!  [next]", D1_SERI);
    MODE_ST++;
    if (MODE_ST > MD_END)
      MODE_ST = MD01;

    disp_init();
    setup_MDxx(MODE_ST);
    delay(500);
  }
  else if (M5.BtnB.wasClicked())
  {
    prtln("BtnB Cliked!  [exit]", D1_SERI);
    MODE_ST = MD00;
    disp_init();
    MD00_disp();
    delay(500);
  }
}

void MD00_disp()
{
  M5.Display.setTextScroll(true);
  M5.Display.printf("****   SDU-imageViewer   ****\n\n\n");
  M5.Display.printf("(BtnA)click: backward image\n\n");
  M5.Display.printf("(BtnB) hold: setting menu\n\n");
  M5.Display.printf("(BtnC)click: forward image\n");
}

String get_MDxx_msg(int mode)
{
  const String AUTOMODE[4] = {"off", "forward", "backward", "random"};
  String msg = "";

  switch (mode)
  {
  case MD01:
    msg = "AutoMode : " + AUTOMODE[viewer.getAutoMode()];
    break;

  case MD02:
    msg = "AutoMode Interval : " + String(viewer.getIntval() / 1000, 10) + "sec";
    break;

  case MD03:
    if (viewer.getIntvalRnd())
      msg = "AutoMode Interval Rnd : on";
    else
      msg = "AutoMode Interval Rnd : off";
    break;

  case MD04:
    msg = "Load SD-Updater menu.bin";
    break;

  case MD05:
    msg = "Save bin-file to SD";
    break;

  case MD06:
    msg = "Power Off";
    break;

  default:
    break;
  }

  return msg;
}
