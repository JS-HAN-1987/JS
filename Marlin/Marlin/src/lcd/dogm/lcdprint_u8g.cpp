/**
 * @file    lcdprint_u8g.cpp
 * @brief   LCD print api for u8glib
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2016-08-19
 * @copyright GPL/BSD
 */

#include "../../inc/MarlinConfigPre.h"

#if HAS_GRAPHICAL_LCD

#include "ultralcd_DOGM.h"

#include "../ultralcd.h"
#include "../../Marlin.h"

#include "../fontutils.h"
#include "../lcdprint.h"
#include "oc_tft/oc_tft.h"


int lcd_glyph_height() { return LCD_PIXEL_HEIGHT; }

bool isOutofRange(int x, int y){
	if( x > LCD_PIXEL_WIDTH || x < 0)
		return true;
	if( y > LCD_PIXEL_HEIGHT || y < 0)
		return true;
	return false;
}

void lcd_moveto(const lcd_uint_t col, const lcd_uint_t row) { 
	setPrintPos(col, row); 
}

void lcd_put_int(const int i) { 
	tft_put_int(i);
}

void readPGM(PGM_P str, char* pTargetStr) {
  int idx=0;
  while (const char c = pgm_read_byte(str++)) pTargetStr[idx++] = c;
}

// return < 0 on error
// return the advanced pixels
int lcd_put_wchar_max(wchar_t c, pixel_len_t max_length) {
  uint16_t x = getPrintCol(), y = getPrintRow();
  if( isOutofRange( x, y))
  {
	SERIAL_ECHO("lcd_put_wchar_max::outofRange ");
	SERIAL_ECHO((char)c); SERIAL_ECHO(' '); SERIAL_ECHO(x); SERIAL_ECHO(' '); SERIAL_ECHOLN(y);
	return LCD_PIXEL_WIDTH;
  }

  if (c < 256) {
    return tft_printstr((char*)&c);
  }
  
  uint16_t ret = tft_printf(x, y, 0, c);
  setPrintPos(x + ret, y);
  return ret;
}

int lcd_put_u8str_max(const char * utf8_str, pixel_len_t max_length) {
  uint16_t x = getPrintCol(), y = getPrintRow();
  if( isOutofRange( x, y))
  {
	  SERIAL_ECHO("lcd_put_u8str_max::outofRange ");
	SERIAL_ECHOLN(x); SERIAL_ECHOLN(y);
	return LCD_PIXEL_WIDTH;
  }
  uint16_t ret = tft_printstring(x, y, 0, utf8_str);
  setPrintPos(x + ret, y);
  return ret;
}

int lcd_put_BIGNUM_u8str_max(const char* utf8_str, pixel_len_t max_length) {
	uint16_t x = getPrintCol(), y = getPrintRow();
	if (isOutofRange(x, y))
	{
		SERIAL_ECHO("lcd_put_BIGNUM_u8str_max::outofRange ");
		SERIAL_ECHOLN(x); SERIAL_ECHOLN(y);
		return LCD_PIXEL_WIDTH;
	}
	uint16_t ret = tft_printBigNumStr(utf8_str);
	setPrintPos(x + ret, y);
	return ret;
}

int lcd_put_u8str_max_P(PGM_P utf8_str_P, pixel_len_t max_length) {
uint16_t x = getPrintCol(), y = getPrintRow();
  if( isOutofRange( x, y))
  {
	  SERIAL_ECHO("lcd_put_u8str_max_P::outofRange ");
	SERIAL_ECHOLN(x); SERIAL_ECHOLN(y);
	return LCD_PIXEL_WIDTH;
  }  

  char msg[64];
  readPGM( utf8_str_P, msg);
  
  uint16_t ret = tft_printstring(x, y, 0, msg);
  setPrintPos(x + ret, y);
  return ret;
}

#endif // HAS_GRAPHICAL_LCD
