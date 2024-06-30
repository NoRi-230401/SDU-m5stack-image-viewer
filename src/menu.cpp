#include <M5Unified.h>
#include "menu.hpp"
#include "sdu.hpp"
#include "ImageViewer.hpp"


void setup_MDxx(int mode);
void loop_MDxx();
void disp_init();
void forever(void);
void MD00_disp();
static void func01_on();
static void func01_off();
static void func02_SDU_menu();
static void func03_SDU_saveBin();
static void func04_PowerOff();
size_t get_menu_count(int mode);
void M5Disp(String msg, int32_t x, int32_t y);
String get_MDxx_msg(int mode);
void MD00_disp();
void prt(String sData, int direction);
void prtln(String sData, int direction);
void FOREVER_LOOP();
void POWER_OFF();
void REBOOT();

extern ImageViewer viewer;
int MODE_ST = MDM2; // mode status = init
static int menu_x = 2;
static int menu_y = 20;
static int menu_w = 120;
static int menu_h = 30;
static int menu_padding = 36;

void forever(void)
{
  while (true)
  {
    delay(1);
  }
}

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

static void func01_on()
{
  prtln("AutoMode set on", D1_SERI);
  viewer.setAutoMode(true);

  M5Disp("  AutoMode", SX1, SY1);
  M5Disp("        --> ON", SX2, SY2);

  delay(100);
}

static void func01_off()
{
  prtln("AutoMode set off", D1_SERI);
  viewer.setAutoMode(false);

  M5Disp("  AutoMode", SX1, SY1);
  M5Disp("        --> OFF", SX2, SY2);

  delay(100);
}

static void func02_SDU_menu()
{
  prtln("Will Load SD-Updater menu.bin", D1_SERI);
  M5Disp(" Load menu.bin", SX1, SY1);
  delay(3000);
  disp_init();
  delay(1000);
  loadMenu();
  FOREVER_LOOP();
}

static void func03_SDU_saveBin()
{
  prtln("Will Save bin_file to SD", D1_SERI);
  M5Disp(" Save bin to SD", SX1, SY1);
  delay(3000);
  disp_init();
  delay(500);
  saveBin();
  delay(500);
  disp_init();
  delay(500);
  setup_MDxx(MD03);
  delay(500);
}

static void func04_PowerOff()
{
  prtln("PowerOff", D1_SERI);
  M5Disp(" Power Off", SX1, SY1);
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
    {"on", func01_on},
    {"off", func01_off},
};
/// メニュー02の定義
static const menu_item_t menu02[] = {
    {"SDU-menu", func02_SDU_menu},
};
/// メニュー03の定義
static const menu_item_t menu03[] = {
    {"SaveBin", func03_SDU_saveBin},
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
  }
}

void select_menu(size_t index)
{
  /// 操作音を鳴らす。
  float Hz = 880 * powf(2.0, index / 12.0f);
  // M5.Speaker.tone(Hz, 100);
  cursor_index = index;
}

void move_menu(bool back = false)
{
  switch (MODE_ST)
  {
  case MD01:
    if (back)
    {
      select_menu((cursor_index ? cursor_index : menu01_count) - 1);
    }
    else
    {
      select_menu((cursor_index + 1) % menu01_count);
    }
    break;

  case MD02:
    if (back)
    {
      select_menu((cursor_index ? cursor_index : menu02_count) - 1);
    }
    else
    {
      select_menu((cursor_index + 1) % menu02_count);
    }
    break;

  case MD03:
    if (back)
    {
      select_menu((cursor_index ? cursor_index : menu03_count) - 1);
    }
    else
    {
      select_menu((cursor_index + 1) % menu03_count);
    }
    break;

  case MD04:
    if (back)
    {
      select_menu((cursor_index ? cursor_index : menu04_count) - 1);
    }
    else
    {
      select_menu((cursor_index + 1) % menu04_count);
    }
    break;
  }
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
  M5.Display.setTextWrap(false);    // テキスト自動折返し
  M5.Display.fillScreen(TFT_BLACK); // 画面クリア

  delay(20);
}

const int M_PADDING[4] = {70, 60, 50, 40};
const int M_H[4] = {60, 50, 40, 35};
const int M_Y[4] = {70, 60, 50, 50};

void setup_MDxx(int mode)
{
  String msg = "";
  M5.Display.fillScreen(TFT_BLACK);
  delay(20);

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
  M5.Display.startWrite();
  M5.Display.setEpdMode(epd_mode_t::epd_fastest);

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
  }
  else if (M5.BtnC.wasClicked())
  {
    prtln("BtnC Cliked!  [next]", D1_SERI);
    MODE_ST++;
    if (MODE_ST > MD_END)
      MODE_ST = MD01;

    disp_init();
    setup_MDxx(MODE_ST);
  }
  else if (M5.BtnB.wasClicked())
  {
    prtln("BtnB Cliked!  [exit]", D1_SERI);
    MODE_ST = MD00;
    disp_init();
    MD00_disp();
    delay(1000);
  }
}

void MD00_disp()
{
  M5.Display.setTextScroll(true);
  M5.Display.printf("****   SDU-imageViewer   ****\n\n\n");
  M5.Display.printf("(BtnA)click: prev image\n\n");
  M5.Display.printf("(BtnB)hold : setting menu\n\n");
  M5.Display.printf("(BtnC)click: next image\n");
  // delay(1000);
}

String get_MDxx_msg(int mode)
{
  String msg = "";

  switch (mode)
  {
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

void prt(String sData, int direction)
{
  switch (direction)
  {
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

void prtln(String sData, int direction)
{
  String strData = sData + "\n";
  prt(strData, direction);
}


void FOREVER_LOOP()
{
  while (true)
  {
    delay(10);
  }
}

void POWER_OFF()
{
  prtln("\n\n*** POWER_OFF ***", D1_SERI);
  //   SPIFFS.end();
  SD.end();

  delay(3000);
  M5.Power.powerOff();
  FOREVER_LOOP();
}

void REBOOT()
{
  prtln("\n\n*** REBOOT ***", D3_BOTH);
  //   SPIFFS.end();
  SD.end();

  delay(3000);
  ESP.restart();
  FOREVER_LOOP();
}
