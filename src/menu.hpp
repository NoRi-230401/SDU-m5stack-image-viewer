#ifndef _MENU_HPP_
#define _MENU_HPP_

// mode status definition
#define MODEM2 -2  // init
#define MODEM1 -1  // setup start
#define MODE00 0   // setup done (Normal mode)
#define MENU01 1   // MENU01  AutoMode
#define MENU02 2   // MENU02  automode interval
#define MENU03 3   // MENU03  automode intval Rnd
#define MENU04 4   // MENU04  load menu.bin
#define MENU05 5   // MENU06  save bin to SD
#define MENU06 6   // MENU06  power off
#define MD_END MENU06 // end of mode

extern int MODE_ST;
extern void setup_menu(int mode);
extern void loop_menu();
extern void disp_init();

// メニュー用構造体。タイトルの文字列と対応する関数のポインタを持つ
struct menu_item
{
  const char *btnName;
  void (*func)(void);
};

// ----------------------------------------------------------------------------
#endif //_MENU_HPP_
