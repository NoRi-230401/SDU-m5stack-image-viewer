#ifndef _MENU_HPP_
#define _MENU_HPP_

// mode status definition 
#define MDM2   -2    // init
#define MDM1   -1    // setup start
#define MD00   0     // setup done (Normal mode)
#define MD01   1     // MD01  AutoMode
#define MD02   2     // MD02  automode interval
#define MD03   3     // MD03  automode intval Rnd
#define MD04   4     // MD04  load menu.bin
#define MD05   5     // MD06  save bin to SD
#define MD06   6     // MD06  power off
#define MD_END 6     // end of mode 

// menu string disp area for M5Disp() -- 
#define SX1 170    // width(320) / 2  + 10
#define SY1  90    // height(240) / 2 - 30 
#define SX2 170    //  = sx1 
#define SY2 120    //  sy1 + 30

extern int MODE_ST;
extern void setup_MDxx(int mode);
extern void loop_MDxx();
extern void disp_init();

// ----------------------------------------------------------------------------
#endif //_MENU_HPP_
