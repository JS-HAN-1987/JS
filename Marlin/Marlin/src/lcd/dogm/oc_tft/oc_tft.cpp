//
//  oc_tft.c
//
//  OpenCreators Firmware TFT LCD module
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//

#include "oc_tft.h"
#include "oc_icons.h"

int pt_x = 0;
int pt_y = 0;

bool in_tft = false;

#include "fonts.h"

// TFT control pins
#define LCD_RESET 47 // A4 // 47
#define LCD_CS 32 // A3 // 32
#define LCD_CD 59 // A2 // 59(A5)
#define LCD_WR 58 // A1 // 58(A4)
#define LCD_RD 40 // A0 // 40

#include <Adafruit_GFX.h>    // Core graphics library

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

bool tft_enable_ = true;

void tft_enable(bool enable)
{
	//tft_enable_ = enable;
}





tft_type_e tft_type;

void lcdInit(void);
void lcdReset(void);


void oc_logo(void)
{
	tft_off();

#ifdef UI_V2
	tft_fillscreen(BLACK);
	tft_setbitmapcolor(WHITE, BLACK);
	tft_drawbitmap(37, 108, 0, bitmap_logo);
#else
	tft_fillscreen(WHITE);
	tft_setbitmapcolor(WHITE, BLACK);
	#ifdef USE_OCLOGO
		tft_drawbitmap(160 - 40, 120 - 40 - 20, 0, bitmap_logo0);
		tft_drawbitmap(160 - 80, 120 - 8 + 60, 0, bitmap_logo1);
	#endif
#endif

	tft_on();
}

void tft_init(void)
{
	if (!tft_enable_) return;
	in_tft = true;
	//LOGP("Using Adafruit 2.4\" TFT Breakout Board Pinout");
	//LOG("TFT size is %d x %d", tft.width(), tft.height());

	lcdInit();
	lcdReset();
	delay(500);

	uint16_t identifier = tft.readID();
//LOG("identifier=%04x", identifier);
	if (identifier == 0x0154) {
		//LOGP("Found S6D0154 LCD driver");
		tft_type = TFTTYPE_S6D0154;
	} else if (identifier == 0x9325 ) {
		//LOGP("Found ILI9325 LCD driver");	 
		tft_type = TFTTYPE_ILI9325;
	} else if (identifier == 0xc505) {
		//LOGP("Found C505(ILI9325?) LCD driver");	 
		tft_type = TFTTYPE_C505;
	} else if (identifier == 0x9341) {
		//LOGP("Found chip ID 0x9341");	 
		tft_type = TFTTYPE_ILI9341;
		identifier = 0x9341;
	} else if (identifier == 0x9595 ) {
		//LOGP("Found chip ID 0x9595");	 
		//LOGP("Initialize using HX8347G LCD driver");	 
		tft_type = TFTTYPE_HX8347G;
		identifier = 0x7575;
	} else {
		//LOG(LT_ERR "Unknown LCD driver chip: 0x%04x", identifier);

		in_tft = false;
		
		
		return;
	}
	tft.begin(identifier);
	if (tft_type == TFTTYPE_ILI9341) tft.setRotation(1);
	else tft.setRotation(3);
	tft_setbitmapcolor(WHITE, BLACK);
	in_tft = false;
	_delay_ms(250);  // wait 1sec to display the splash screen
	tft_on();
}

void tft_clear(void)
{
	tft_fillrect(0, 0, 320, 240, BLACK);
}

void tft_off(void)
{
	if (!tft_enable_) return;
	in_tft = true;
	tft.off();
	in_tft = false;
}

void tft_on(void)
{
	if (!tft_enable_) return;
	in_tft = true;
	tft.on();
	in_tft = false;
}

void tft_setrotation(uint8_t rot)
{
	tft.setRotation(rot);
}

void tft_drawpixel(int x, int y, uint16_t color)
{
	tft.drawPixel((int16_t)x, (int16_t)y, color);
}

void tft_drawline(int x0, int y0, int x1, int y1, uint16_t color)
{
	if (!tft_enable_) return;
	in_tft = true;
	tft.drawLine(x0, y0, x1, y1, color);
	in_tft = false;
}

void tft_fillrect(int x, int y, int w, int h, uint16_t color)
{
	if (!tft_enable_) return;
	in_tft = true;
	tft.fillRect(x, y, w, h, color);
	in_tft = false;
}

void tft_drawrect(int x, int y, int w, int h, uint16_t color)
{
	tft_fillrect(x, y, w, 1, color);
	tft_fillrect(x, y, 1, h, color);
	tft_fillrect(x+w-1, y, 1, h, color);
	tft_fillrect(x, y+h-1, w, 1, color);
}

void tft_fillscreen(uint16_t color)
{
	if (!tft_enable_) return;
	in_tft = true;
	tft.fillScreen(color);
	in_tft = false;
}

void tft_drawtext(int x, int y, char *text, uint16_t color, int text_size)
{
	if (!tft_enable_) return;
	in_tft = true;
	tft.setCursor(x, y);
	tft.setTextColor(color);  tft.setTextSize(text_size);
	tft.println(text);
	in_tft = false;
}

void tft_drawtextfmt(int x, int y, int text_size, int fcolor, int bcolor, char *fmt, ...)
{
	if (!tft_enable_) return;
	va_list va;
	char buf[80];
 
	va_start(va, fmt);
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	in_tft = true;
	tft.setCursor(x, y);
	tft.setTextColor(fcolor, bcolor);  tft.setTextSize(text_size);
	tft.println(buf);
	in_tft = false;
}

uint16_t color565_table[16];

void tft_setbitmapcolor(uint16_t fcolor, uint16_t bcolor)
{
	if (!tft_enable_) return;
	int fr = BIT5_TO_BIT8(fcolor >> 11), fg = BIT6_TO_BIT8((fcolor >> 5) & 0x3f), fb = BIT5_TO_BIT8(fcolor & 0x1f);
	int br = BIT5_TO_BIT8(bcolor >> 11), bg = BIT6_TO_BIT8((bcolor >> 5) & 0x3f), bb = BIT5_TO_BIT8(bcolor & 0x1f);
	int i;

	in_tft = true;
	for (i=0; i<16; i++) {
		int r = (i*fr+(15-i)*br)/15;
		int g = (i*fg+(15-i)*bg)/15;
		int b = (i*fb+(15-i)*bb)/15;
		color565_table[i] = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
	}
	in_tft = false;
}


void tft_drawbitmap2(int ix, int iy, int w, int h, const uint8_t bitmap[] PROGMEM)
{
	if (!tft_enable_) return;
	int x, y, i;
	//w *= 2;
	in_tft = true;
	int pos = 0;
	bool first = true;
	tft.setAddrWindow(ix, iy, ix + w * 2 * 8 - 1, iy + h * 2 - 1);

	for (y = 0; y < h; y++)
	{
		uint16_t cbuf[320];
		int idx = 0;
		for (x = 0; x < w; x++)
		{
			uint8_t b = (pgm_read_byte(&bitmap[pos++]));
			if (b & 0x80) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
			if (b & 0x40) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
			if (b & 0x20) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
			if (b & 0x10) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
			if (b & 0x08) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
			if (b & 0x04) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
			if (b & 0x02) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
			if (b & 0x01) { cbuf[idx++] = WHITE; cbuf[idx++] = WHITE; }
			else { cbuf[idx++] = BLACK; cbuf[idx++] = BLACK; }
		}
		tft.pushColors(cbuf, idx, first); first = false;
		tft.pushColors(cbuf, idx, first);
	}
	in_tft = false;
}


void tft_drawbitmap(int px, int py, int flags, const uint8_t bitmap[] PROGMEM)
{
	if (!tft_enable_) return;
	uint16_t ix  = pgm_read_byte(&bitmap[0]) | (pgm_read_byte(&bitmap[1]) << 8);
	uint16_t iy  = pgm_read_byte(&bitmap[2]) | (pgm_read_byte(&bitmap[3]) << 8);
	uint16_t w   = pgm_read_byte(&bitmap[4]) | (pgm_read_byte(&bitmap[5]) << 8);
	uint16_t h   = pgm_read_byte(&bitmap[6]) | (pgm_read_byte(&bitmap[7]) << 8);
	uint16_t bpp = pgm_read_byte(&bitmap[8]) | (pgm_read_byte(&bitmap[9]) << 8);
	int x, y, i;

	in_tft = true;
	int pos = 12;
	int n = 2;
	boolean first = true;
	if ((!flags & OC_TRANSPARENT_BACK)) tft.setAddrWindow(px+ix, py+iy, px+ix+w-1, py+iy+h-1);
	for (y = 0; y<h; y++) {
		uint16_t cbuf[320];
		for (x = 0; x<w/n; x++) {
			uint8_t b = (pgm_read_byte(&bitmap[pos++]));
			if (flags & OC_TRANSPARENT_BACK) {
				#if 0
				if (b >> 4)  tft_fillrect(px+x*2+0, py+y, 1, 1, color565_table[b >> 4]);
				if (b & 0xf) tft_fillrect(px+x*2+1, py+y, 1, 1, color565_table[b & 0xf]);
				#else
				if (b >> 4)  tft_drawpixel(px+ix+x*2+0, py+iy+y, color565_table[b >> 4]);
				if (b & 0xf) tft_drawpixel(px+ix+x*2+1, py+iy+y, color565_table[b & 0xf]);
				#endif
			} else {
				cbuf[x*2+0] = color565_table[b>>4];
				cbuf[x*2+1] = color565_table[b & 0xf];
			}
		}
		if ((!flags & OC_TRANSPARENT_BACK)) tft.pushColors(cbuf, w, first); first = false;
	}
	in_tft = false;
}

static int tft_printletter(int x, int y, int flags, const uint8_t bitmap[] PROGMEM, bool draw)
{
	uint16_t w = pgm_read_byte(&bitmap[4]) | (pgm_read_byte(&bitmap[5]) << 8);
	if (draw) {
		//tft_fillrect(x, y, w, 24, color565_table[0]);
		tft_drawbitmap(x, y, flags, bitmap);
	}
	return (int)w;
}

static int tft_printletter_09(int x, int y, int flags, const uint8_t bitmap[] PROGMEM, bool draw)
{
	uint16_t w = pgm_read_byte(&bitmap[4]) | (pgm_read_byte(&bitmap[5]) << 8);
	if (draw) {
		//tft_fillrect(x, y, 13, 24, color565_table[0]);
		tft_drawbitmap(x, y, flags, bitmap);
	}
	return (int)w;
}

#define Y_PADDING 10

int draw_bignumber_char(int x, int y, int flags, char c, bool draw)
{
	int yy = y + Y_PADDING;
	int dx = 28;
	switch (c) {
	case ' ': if (draw) tft_drawbitmap(x, yy, 0, font_42x44_space); break;
	case '0': if (draw) tft_drawbitmap(x, yy, 0, font42x44_0); break;
	case '1': if (draw) tft_drawbitmap(x, yy, 0, font42x44_1); break;
	case '2': if (draw) tft_drawbitmap(x, yy, 0, font42x44_2); break;
	case '3': if (draw) tft_drawbitmap(x, yy, 0, font42x44_3); break;
	case '4': if (draw) tft_drawbitmap(x, yy, 0, font42x44_4); break;
	case '5': if (draw) tft_drawbitmap(x, yy, 0, font42x44_5); break;
	case '6': if (draw) tft_drawbitmap(x, yy, 0, font42x44_6); break;
	case '7': if (draw) tft_drawbitmap(x, yy, 0, font42x44_7); break;
	case '8': if (draw) tft_drawbitmap(x, yy, 0, font42x44_8); break;
	case '9': if (draw) tft_drawbitmap(x, yy, 0, font42x44_9); break;
	case '%': if (draw) tft_drawbitmap(x, yy, 0, font42x44_percent); dx = 41; break;
	}

	if (pt_y == y)
		pt_x += dx;
	else
		pt_x = dx;
	return dx;
}

int tft_printBigNumStr(char* string) {
	int i;
	//tft_setbitmapcolor(fcolor, bcolor);
	int len = strlen(string);
	if (len < 6)
	{
		int diff = 6 - len;
		while(diff--)
			draw_bignumber_char(pt_x, pt_y, 0, ' ', true);
	}		

	for (i = 0; i < len; i++) {
		draw_bignumber_char(pt_x, pt_y, 0, string[i], true);
	}
	return pt_x;
}


int tft_printchar(int x, int y, int flags, char c, bool draw)
{
	int yy = y+Y_PADDING;
	int dx = 12;
	switch (c) {
	case '0': dx = tft_printletter_09(x, yy, flags, font_0, draw); break;
	case '1': dx = tft_printletter_09(x+1, yy, flags, font_1, draw)-1; break;
	case '2': dx = tft_printletter_09(x, yy, flags, font_2, draw); break;
	case '3': dx = tft_printletter_09(x, yy, flags, font_3, draw); break;
	case '4': dx = tft_printletter_09(x, yy, flags, font_4, draw); break;
	case '5': dx = tft_printletter_09(x, yy, flags, font_5, draw); break;
	case '6': dx = tft_printletter_09(x, yy, flags, font_6, draw); break;
	case '7': dx = tft_printletter_09(x, yy, flags, font_7, draw); break;
	case '8': dx = tft_printletter_09(x, yy, flags, font_8, draw); break;
	case '9': dx = tft_printletter_09(x, yy, flags, font_9, draw); break;

	case 'A': dx = tft_printletter(x, yy, flags, font_upper_A, draw)-2; break;
	case 'B': dx = tft_printletter(x, yy, flags, font_upper_B, draw); break;
	case 'C': dx = tft_printletter(x, yy, flags, font_upper_C, draw)-1; break;
	case 'D': dx = tft_printletter(x, yy, flags, font_upper_D, draw); break;
	case 'E': dx = tft_printletter(x, yy, flags, font_upper_E, draw); break;
	case 'F': dx = tft_printletter(x, yy, flags, font_upper_F, draw); break;
	case 'G': dx = tft_printletter(x, yy, flags, font_upper_G, draw)-1; break;
	case 'H': dx = tft_printletter(x, yy, flags, font_upper_H, draw); break;
	case 'I': dx = tft_printletter(x, yy, flags, font_upper_I, draw); break;
	case 'J': dx = tft_printletter(x, yy, flags, font_upper_J, draw); break;
	case 'K': dx = tft_printletter(x, yy, flags, font_upper_K, draw); break;
	case 'L': dx = tft_printletter(x, yy, flags, font_upper_L, draw); break;
	case 'M': dx = tft_printletter(x, yy, flags, font_upper_M, draw)+1; break;
	case 'N': dx = tft_printletter(x, yy, flags, font_upper_N, draw); break;
	case 'O': dx = tft_printletter(x, yy, flags, font_upper_O, draw)-1; break;
	case 'P': dx = tft_printletter(x, yy, flags, font_upper_P, draw); break;
	case 'Q': dx = tft_printletter(x, yy, flags, font_upper_Q, draw)-2; break;
	case 'R': dx = tft_printletter(x, yy, flags, font_upper_R, draw); break;
	case 'S': dx = tft_printletter(x, yy, flags, font_upper_S, draw)-1; break;
	case 'T': dx = tft_printletter(x, yy, flags, font_upper_T, draw)-2; break;
	case 'U': dx = tft_printletter(x, yy, flags, font_upper_U, draw)+1; break;
	case 'V': dx = tft_printletter(x, yy, flags, font_upper_V, draw)-2; break;
	case 'W': dx = tft_printletter(x, yy, flags, font_upper_W, draw); break;
	case 'X': dx = tft_printletter(x, yy, flags, font_upper_X, draw); break;
	case 'Y': dx = tft_printletter(x, yy, flags, font_upper_Y, draw); break;
	case 'Z': dx = tft_printletter(x, yy, flags, font_upper_Z, draw); break;

	case 'a': dx = tft_printletter(x, yy+2, flags, font_lower_a, draw)-2; break;
	case 'b': dx = tft_printletter(x, yy+2, flags, font_lower_b, draw); break;
	case 'c': dx = tft_printletter(x, yy+2, flags, font_lower_c, draw)-1; break;
	case 'd': dx = tft_printletter(x, yy+2, flags, font_lower_d, draw)-1; break;
	case 'e': dx = tft_printletter(x, yy+2, flags, font_lower_e, draw)-1; break;
	case 'f': dx = tft_printletter(x, yy+2, flags, font_lower_f, draw)-2; break;
	case 'g': dx = tft_printletter(x, yy+2, flags, font_lower_g, draw)-2; break;
	case 'h': dx = tft_printletter(x, yy+2, flags, font_lower_h, draw)+1; break;
	case 'i': dx = tft_printletter(x, yy+2, flags, font_lower_i, draw)+2; break;
	case 'j': dx = tft_printletter(x+1, yy+2, flags, font_lower_j, draw)-2; break;
	case 'k': dx = tft_printletter(x, yy+2, flags, font_lower_k, draw); break;
	case 'l': dx = tft_printletter(x, yy+2, flags, font_lower_l, draw)+1; break;
	case 'm': dx = tft_printletter(x, yy+2, flags, font_lower_m, draw)+1; break;
	case 'n': dx = tft_printletter(x, yy+2, flags, font_lower_n, draw)+1; break;
	case 'o': dx = tft_printletter(x, yy+2, flags, font_lower_o, draw)-2; break;
	case 'p': dx = tft_printletter(x, yy+2, flags, font_lower_p, draw); break;
	case 'q': dx = tft_printletter(x, yy+2, flags, font_lower_q, draw)-2; break;
	case 'r': dx = tft_printletter(x, yy+2, flags, font_lower_r, draw); break;
	case 's': dx = tft_printletter(x, yy+2, flags, font_lower_s, draw); break;
	case 't': dx = tft_printletter(x, yy+2, flags, font_lower_t, draw)-2; break;
	case 'u': dx = tft_printletter(x, yy+2, flags, font_lower_u, draw)+1; break;
	case 'v': dx = tft_printletter(x, yy+2, flags, font_lower_v, draw); break;
	case 'w': dx = tft_printletter(x, yy+2, flags, font_lower_w, draw); break;
	case 'x': dx = tft_printletter(x, yy+2, flags, font_lower_x, draw)-1; break;
	case 'y': dx = tft_printletter(x, yy+2, flags, font_lower_y, draw)-1; break;
	case 'z': dx = tft_printletter(x, yy+2, flags, font_lower_z, draw); break;

	case '&': dx = tft_printletter(x, yy, flags, font_ampersand, draw); break;
	case '\'': dx = tft_printletter(x, yy, flags, font_apostrophe, draw); break;
	case '*': dx = tft_printletter(x, yy, flags, font_asterisk, draw); break;
	case '@': dx = tft_printletter(x, yy, flags, font_atsign, draw); break;
	case '^': dx = tft_printletter(x, yy, flags, font_circumflex, draw); break;
	case ':': dx = tft_printletter(x+2, yy, flags, font_colon, draw); break;
	case ',': dx = tft_printletter(x, yy, flags, font_comma, draw); break;
	case '=': dx = tft_printletter(x, yy, flags, font_equal, draw); break;
	case '!': dx = tft_printletter(x, yy, flags, font_exclamation, draw); break;
	case '`': dx = tft_printletter(x+1, yy, flags, font_do, draw); break;
	case '>': dx = tft_printletter(x, yy, flags, font_gt, draw); break;
	case '<': dx = tft_printletter(x, yy, flags, font_lt, draw); break;
	case ']': dx = tft_printletter(x, yy, flags, font_br, draw); break;
	case '[': dx = tft_printletter(x, yy, flags, font_bl, draw); break;
	case '-': dx = tft_printletter(x, yy, flags, font_minus, draw); break;
	case '%': dx = tft_printletter(x, yy, flags, font_percent, draw); break;
	case '.': dx = tft_printletter(x, yy, flags, font_period, draw); break;
	case '(': dx = tft_printletter(x, yy, flags, font_pl, draw)+3; break;
	case '+': dx = tft_printletter(x, yy, flags, font_plus, draw); break;
	case ')': dx = tft_printletter(x, yy, flags, font_pr, draw); break;
	case '?': dx = tft_printletter(x, yy, flags, font_question, draw); break;
	case '"': dx = tft_printletter(x, yy, flags, font_quotationmark, draw); break;
	case ';': dx = tft_printletter(x, yy, flags, font_semicolon, draw); break;
	case '#': dx = tft_printletter(x, yy, flags, font_sharp, draw); break;
	case '/': dx = tft_printletter(x, yy, flags, font_slash, draw); break;
	case '~': dx = tft_printletter(x, yy, flags, font_tilde, draw); break;
	case '_': dx = tft_printletter(x, yy, flags, font_underbar, draw); break;
	case '|': dx = tft_printletter(x, yy, flags, font_verticalbar, draw); break;
	case 3: dx = tft_printletter(x, yy, flags, font_3up, draw); break;
	case 4: dx = tft_printletter(x, yy, flags, font_4right, draw); break;
	}
	if (c >= '0' && c <= '9') dx = 12;
	
	if (pt_y == y)
		pt_x += dx;
	else
		pt_x = dx;
	return dx;	
}

static bool cmp_char(char *_old, char *_new, int i, int olen)
{
	if (i > olen || _new[i] != _old[i]) return true;
	return false;
}

int tft_printstring(int x, int y, int flags, char *string)
{
	int i;
	for (i=0; i<strlen(string); i++) {
		x += tft_printchar(x, y, flags, string[i], true);
	}
	
	return x;
}

int tft_printf(int x, int y, int flags, char *fmt, ...)
{
	if (!tft_enable_) return 0;
	va_list va;
	char buf[80];
 
	va_start(va, fmt);
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	//tft_setbitmapcolor(fcolor, bcolor);
	return tft_printstring(x, y, flags, buf);
}

int tft_stringwidth(char *text)
{
	return tft_printstring(0, 0, 0, text);
}

int tft_stringheight(char *text)
{
	return 26;
}

int16_t tft_bitmap_x(const uint8_t *p)
{
	int16_t x = pgm_read_byte(&p[0]) | (pgm_read_byte(&p[1]) << 8);
	return x;
}

int16_t tft_bitmap_y(const uint8_t *p)
{
	int16_t y = pgm_read_byte(&p[2]) | (pgm_read_byte(&p[3]) << 8);
	return y;
}

int16_t tft_bitmap_width(const uint8_t *p)
{
	int16_t iw = pgm_read_byte(&p[4]) | (pgm_read_byte(&p[5]) << 8);
	return iw;
}

int16_t tft_bitmap_height(const uint8_t *p)
{
	int16_t ih = pgm_read_byte(&p[6]) | (pgm_read_byte(&p[7]) << 8);
	return ih;
}

void tft_drawbitmap_from_gcode(int x, int y, char *gcode_file)
{
  
}

void tft_counter_inc(int x, int y, int *pcnt, int max)
{
	if (!tft_enable_) return;
	(*pcnt)++;
	if (*pcnt > max) {
		*pcnt = 0;
	}
	in_tft = true;
	char tmp[10];
	sprintf(tmp, "%03d", *pcnt);
	tft.fillRect(x, y, 6*3, 10, BLACK);
	tft.setCursor(x, y);
	tft.setTextColor(YELLOW);  tft.setTextSize(1);
	tft.println(tmp);
	in_tft = false;
}

void chardraw_M(int x, int y)
{
}

void drawVLine(int x0, int y0, int len)
{
	tft_drawline(x0, y0, x0, y0 + len, WHITE);
}

void drawHLine(int x0, int y0, int len, uint16_t bcolor) {
	tft_drawline(x0, y0, x0 + len, y0, bcolor);
}

void setPrintPos(int x, int y) {
	pt_x = x;
	pt_y = y;
}

void drawBox(int x, int y, int w, int h) {
	tft_drawrect(x, y, w, h, WHITE);
}

void tft_put_int(int x) {
	tft_printf(pt_x, pt_y, 0, "%d", x);
}

int tft_printstr(char* string) {
	int i;
	//tft_setbitmapcolor(fcolor, bcolor);
	for (i = 0; i < strlen(string); i++) {
		tft_printchar(pt_x, pt_y, 0, string[i], true);
	}
	return pt_x;
}

int getPrintRow()
{
	return pt_y;
}
int getPrintCol()
{
	return pt_x;
}
