// --- Add by NoRi 2024-06-23 ------------------------------
// *****************************************************
//  SDU-WebDav  main.h           2024-06-20  by NoRi
// *****************************************************
#ifndef _SDU_HPP_
#define _SDU_HPP_

#include <Arduino.h>
#include <SD.h>
#include <M5Unified.h>
#include <ESP32-targz.h>
#include <M5StackUpdater.h>

#define APP_VER   "SDU-imageViewer-230623"
#define APP_NAME  "SDU-imageViewer"  // app Name
#define APP_BIN   "/21_imgView.bin"  // app bin file-name
#define TIMEOUT00 5000               // lobby countdouwn timeout : msec
#define D1_SERI 1
#define D2_DISP 2
#define D3_BOTH 3

extern int SDCARD_CS_PIN;
extern void setupSDUpdater(const char* appName);
extern bool SdBegin();
extern void prt(String sData, int direction = D3_BOTH);
extern void prtln(String sData, int direction = D3_BOTH);
extern void POWER_OFF();
extern void REBOOT();


#endif //_SDU_HPP_
