//
//  oc_firmware.h
//
//  OpenCreators Firmware header
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//

#ifndef __OC_FIRMWARE_H__
#define __OC_FIRMWARE_H__

#include "oc_events.h"

#define BIG_PERCENT

// 565 colors 
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
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

#define BIT4_TO_BIT8(n) (255*(n)/15)
#define BIT5_TO_BIT8(n) (255*(n)/31)
#define BIT6_TO_BIT8(n) (255*(n)/63)

#define RGB565_TO_OCE_COLOR(c) \
	0xff000000 | \
	(BIT5_TO_BIT8((c >> 11) & 0x1f) << 16) | \
	(BIT6_TO_BIT8((c >> 5) & 0x3f) << 8) | \
	BIT5_TO_BIT8(c & 0x1f)

// tft
void tft_init(void);
void tft_on(void);
void tft_off(void);
void tft_setrotation(uint8_t rot);
void tft_drawpixel(int x, int y, uint16_t color);
void tft_drawline(int x0, int y0, int x1, int y1, uint16_t color);
void tft_fillrect(int x, int y, int w, int h, uint16_t color);
void tft_drawrect(int x, int y, int w, int h, uint16_t color);
void tft_fillscreen(uint16_t color);
void tft_setbitmapcolor(uint16_t fcolor, uint16_t bcolor);
void tft_drawbitmap(int px, int py, int flags, const uint8_t bitmap[] PROGMEM);
int  tft_printf(int x, int y, int flags, int fcolor, int bcolor, char *oldstr, char *fmt, ...);
int  tft_stringwidth(char *text);
int  tft_stringheight(char *text);
int16_t tft_bitmap_x(const uint8_t *p);
int16_t tft_bitmap_y(const uint8_t *p);
int16_t tft_bitmap_width(const uint8_t *p);
int16_t tft_bitmap_height(const uint8_t *p);
void tft_drawtext(int x, int y, char *text, uint16_t color, int text_size);
void tft_drawtextfmt(int x, int y, int text_size, int fcolor, int bcolor, char *fmt, ...);
void tft_drawbitmap_from_gcode(int x, int y, char *gcode_file);
void tft_counter_inc(int x, int y, int *pcnt, int max);
#ifdef UI_V2
int draw_bignumber_char(int x, int y, int flags, char c, bool draw);
#endif

void get_endstop_status(char *pxt, char *pyt, char *pzt);

//#include "oc_sd.h"

// Marlin
void fw_home(void);
#if 0
void fw_left(void);
void fw_right(void);
void fw_rear(void);
void fw_front(void);
void fw_up(void);
void fw_down(void);
void fw_extrude(void);
void fw_retract(void);
#endif
void fw_g29(void);
void fw_npdm(void);
void fw_preheat(void);
void fw_cooldown(void);
void fw_motoroff(void);
void fw_npdm_prepare(void);

void fw_settemp(int temp);
void fw_setspeed(int speed);
void fw_setfanspeed(int speed);

// Marlin_main.cpp
bool oc_batch_is_running(void);

#endif // __OC_FIRMWARE_H__

