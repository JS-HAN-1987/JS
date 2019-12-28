//
//  oc_tft.h
//
//  OpenCreators Firmware TFT LCD module header
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//


#ifndef __OC_TFT_H__
#define __OC_TFT_H__

#include <stdint.h>
#include <avr/pgmspace.h>
#include "Adafruit_TFTLCD.h" // Hardware-specific library

#define WORKAROUND_FOR_ILI9325
#define BIG_PERCENT

// 565 colors 
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410 // 1000 0100 0001 0000
                       // 10000 100000 10000

#define DARK_ORANGE  OCE_COLOR_TO_RGB565(0xffff5c00)
#define ORANGE       OCE_COLOR_TO_RGB565(0xffff8106)
#define LIGHT_ORANGE OCE_COLOR_TO_RGB565(0xffff9f00)

#define OCE_COLOR_TO_RGB565(c) ((c >> 8) & 0xf800) | ((c >> 5) & 0x7e0) | ((c >> 3) & 0x1f)

#define DARK_BLUE   OCE_COLOR_TO_RGB565(0xff000080)
#define DARK_RED    OCE_COLOR_TO_RGB565(0xff800000)
#define DARK_GREEN  OCE_COLOR_TO_RGB565(0xff008000)
#define DARK_YELLOW OCE_COLOR_TO_RGB565(0xff808000)

#define LIGHT_GRAY   OCE_COLOR_TO_RGB565(0xffa0a0a0)
#define LIGHT_GREEN  OCE_COLOR_TO_RGB565(0xff80c080)

#define COLOR_MAIN_BG  OCE_COLOR_TO_RGB565(0xffffffff)

#define OC_TRANSPARENT_BACK 0x0001 // for tft_printf

void oc_logo(void);
void tft_init(void);
void tft_clear(void);
void tft_on(void);
void tft_off(void);
void tft_fillrect(int x, int y, int w, int h, uint16_t color);
void tft_fillscreen(uint16_t color);
void tft_drawtext(int x, int y, char *text, uint16_t color, int text_size);
void tft_drawtextfmt(int x, int y, int text_size, int fcolor, int bcolor, char *fmt, ...);
void tft_setbitmapcolor(uint16_t fcolor, uint16_t bcolor);
void tft_drawbitmap(int px, int py, int flags, const uint8_t bitmap[] PROGMEM);
void tft_drawbitmap2(int ix, int iy, int w, int h, const uint8_t bitmap[] PROGMEM);
int tft_printf(int x, int y, int flags, char *fmt, ...);
int tft_printchar(char c);
int tft_printchar(int x, int y, int flags, char c, bool draw);
int tft_printstring(int x, int y, int flags, char* string);
void tft_drawline(int x0, int y0, int x1, int y1, uint16_t color);
void drawHLine(int x0, int y0, int len, uint16_t bcolor);
void drawVLine(int x0, int y0, int len);
void setPrintPos(int x, int y);
void drawBox(int x, int y, int w, int h, uint16_t bcolor);
void tft_put_int(int x);
int tft_printstr(char* string);
int  getPrintRow();
int getPrintCol();
int tft_printBigNumStr(char* string);
int tft_getHeight();



  #define UI_V2


  // lcd update types
  #define LUT_LOOP             1
  #define LUT_PROCESS_COMMANDS 2
  #define LUT_PROCESS_COMMAND1 3
  #define LUT_PROCESS_COMMAND2 4
  #define LUT_PROCESS_COMMAND3 5
  #define LUT_PROCESS_COMMAND4 6
  #define LUT_PROCESS_COMMAND5 7
  #define LUT_PROCESS_COMMAND6 8
  #define LUT_PROCESS_COMMAND7 9
  #define LUT_PROCESS_COMMAND8 10
  #define LUT_PROCESS_COMMAND9 11
  #define LUT_KILL             12
  #define LUT_ST_SYNCHRONIZE   13
  #define LUT_PID_AUTOTUNE     14
  #define LUT_WATCHDOG         15
  #define LUT_PLAN_BUFFER_LINE 16
  #define LUT_ERROR_RESOLVED   17

  #define BUT_ISR_0_1  20
  #define BUT_ISR_0_2  21
  #define BUT_ISR_0_4  22
  #define BUT_ISR_0_6  23
  #define BUT_ISR_0_8  24
  #define BUT_ISR_0_10 25 
  
typedef enum {
	TFTTYPE_S6D0154,
	TFTTYPE_ILI9325,
	TFTTYPE_HX8347G,
	TFTTYPE_C505,
	TFTTYPE_ILI9341
} tft_type_e;

extern tft_type_e tft_type;



#define BIT4_TO_BIT8(n) (255*(n)/15)
#define BIT5_TO_BIT8(n) (255*(n)/31)
#define BIT6_TO_BIT8(n) (255*(n)/63)

#define RGB565_TO_OCE_COLOR(c) \
  0xff000000 | \
  (BIT5_TO_BIT8((c >> 11) & 0x1f) << 16) | \
  (BIT6_TO_BIT8((c >> 5) & 0x3f) << 8) | \
  BIT5_TO_BIT8(c & 0x1f)

  
#endif
