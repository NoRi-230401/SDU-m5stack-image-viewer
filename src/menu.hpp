#ifndef _MENU_HPP_
#define _MENU_HPP_

// print direction for prt() and prtln()
#define D1_SERI   1    // serial
#define D2_DISP   2    // display
#define D3_BOTH   3    // both

// mode status definition 
#define MDM2   -2    // init
#define MDM1   -1    // setup start
#define MD00   0     // setup done (Normal mode)
#define MD01   1     // MD01  AutoMode
#define MD02   2     // MD02  load menu.bin
#define MD03   3     // MD03  save bin to SD
#define MD04   4     // MD04  power off
#define MD_END 4     // end of mode 

// menu disp for function  
#define SX1 170    // width(320) / 2  + 10
#define SY1  90    // height(240) / 2 - 30 
#define SX2 170    //  = sx1 
#define SY2 120    //  sy1 + 30

extern int MODE_ST;
extern void setup_MDxx(int mode);
extern void loop_MDxx();
extern void disp_init();
extern void prt(String sData, int direction = D3_BOTH);
extern void prtln(String sData, int direction = D3_BOTH);
extern void POWER_OFF();
extern void REBOOT();
extern void FOREVER_LOOP();
// ----------------------------------------------------------------------------

#endif //_MENU_HPP_
