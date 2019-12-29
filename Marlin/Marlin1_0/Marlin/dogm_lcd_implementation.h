/**
 *dogm_lcd_implementation.h
 *
 *Graphics LCD implementation for 128x64 pixel LCDs by STB for ErikZalm/Marlin
 *Demonstrator: http://www.reprap.org/wiki/STB_Electronics
 *License: http://opensource.org/licenses/BSD-3-Clause
 *
 *With the use of:
 *u8glib by Oliver Kraus
 *http://code.google.com/p/u8glib/
 *License: http://opensource.org/licenses/BSD-3-Clause
 */


#ifndef ULTRA_LCD_IMPLEMENTATION_DOGM_H
#define ULTRA_LCD_IMPLEMENTATION_DOGM_H

/**
* Implementation of the LCD display routines for a DOGM128 graphic display. These are common LCD 128x64 pixel graphic displays.
**/

#ifdef ULTIPANEL
#define BLEN_A 0
#define BLEN_B 1
#define BLEN_C 2

#define EN_C (1<<BLEN_C)
#define EN_B (1<<BLEN_B)
#define EN_A (1<<BLEN_A)

#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1
#define LCD_CLICKED (buttons&EN_C)
#endif

#include "ultralcd.h"
#include "oc_tft.h"

/* Russian language not supported yet, needs custom font

#ifdef LANGUAGE_RU
#include "LiquidCrystalRus.h"
#define LCD_CLASS LiquidCrystalRus
#else
#include <LiquidCrystal.h>
#define LCD_CLASS LiquidCrystal
#endif
*/


// DOGM parameters (size in pixels)
#define LCD_PIXEL_WIDTH			320
#define LCD_PIXEL_HEIGHT		240
#define DOG_CHAR_WIDTH			12
#define DOG_CHAR_HEIGHT			40
#define DOG_CHAR_WIDTH_LARGE	12
#define DOG_CHAR_HEIGHT_LARGE	40

#define START_ROW				0

#define STATUS_HEATERS_Y 6
#define STATUS_FANS_Y 6
#define STATUS_FR_Y 6 + DOG_CHAR_HEIGHT
#define STATUS_LINE_Y LCD_PIXEL_HEIGHT - DOG_CHAR_HEIGHT

/* Custom characters defined in font font_6x10_marlin.c */
//#define LCD_STR_DEGREE      "\xB0"
#define LCD_STR_REFRESH     "\xF8"
#define LCD_STR_FOLDER      "\xF9"
#define LCD_STR_ARROW_RIGHT "\xFA"
#define LCD_STR_UPLEVEL     "\xFB"
//#define LCD_STR_CLOCK       "\xFC"
//#define LCD_STR_FEEDRATE    "\xFD"
//#define LCD_STR_BEDTEMP     "\xFE"
//#define LCD_STR_THERMOMETER "\xFF"

//#define FONT_STATUSMENU	u8g_font_6x9

int lcd_contrast;



static void lcd_implementation_init()
{
	tft_init();
	tft_clear();
}

static void lcd_implementation_clear()
{
	
// NO NEED TO IMPLEMENT LIKE SO. Picture loop automatically clears the display.
//
// Check this article: http://arduino.cc/forum/index.php?topic=91395.25;wap2
//
//	firstPage();
//	do {	
//			//u8g.setColorIndex(0);
//			drawBox (0, 0, u8g.getWidth(), _getHeight());
//			//u8g.setColorIndex(1);
//		} while( 0 );
}

/* Arduino < 1.0.0 is missing a function to print PROGMEM strings, so we need to implement our own */
static void lcd_printPGM(const char* str)
{
    char c;
    while((c = pgm_read_byte(str++)) != '\0')
    {
		tft_printchar(c);
    }
}

static void _draw_heater_status(int x, int heater) {
  bool isBed = heater < 0;

  int y = STATUS_HEATERS_Y;
  //u8g.setFont(FONT_STATUSMENU);

  setPrintPos(x, y);
  tft_printstr(itostr3(int((heater >= 0 ? degTargetHotend(heater) : degTargetBed()) + 0.5)));
  setPrintPos(x, y  + DOG_CHAR_HEIGHT);
  tft_printstr(itostr3(int(heater >= 0 ? degHotend(heater) : degBed()) + 0.5));
  if (!isHeatingHotend(heater)) {
    drawBox(x, y, DOG_CHAR_WIDTH*3, DOG_CHAR_HEIGHT, WHITE);
  }
  else {
    drawBox(x, y, DOG_CHAR_WIDTH*3, DOG_CHAR_HEIGHT, BLACK);
  }
}

static void lcd_implementation_status_screen()
{

 static unsigned char fan_rot = 0;
 
 //u8g.setColorIndex(1);	// black on white
 
 // Symbols menu graphics, animated fan
 //u8g.drawBitmapP(9,1,STATUS_SCREENBYTEWIDTH,STATUS_SCREENHEIGHT, (blink % 2) && fanSpeed ? status_screen0_bmp : status_screen1_bmp);
 
 #ifdef SDSUPPORT
 //SD Card Symbol
 //drawBox(42,42,8,7, WHITE);
 //drawBox(50,44,2,5, WHITE);
 //drawBox(42,49,10,4, WHITE);
 //u8g.drawPixel(50,43);
 // Progress bar
 //drawBox(54,49,73,4, WHITE);
 
 // SD Card Progress bar and clock
 //u8g.setFont(FONT_STATUSMENU);
 
 if (IS_SD_PRINTING)
   {
	// Progress bar
	drawBox(55,50, (unsigned int)( (71 * card.percentDone())/100) ,2, WHITE);
   }
    else {
			// do nothing
		 }
 
	setPrintPos(80,47);
 if(starttime != 0)
    {
        uint16_t time = millis()/60000 - starttime/60000;

		tft_printstr(itostr2(time/60));
		tft_printchar(':');
		tft_printstr(itostr2(time%60));
    }else{
			lcd_printPGM(PSTR("--:--"));
		 }
 #endif
 
  // Extruders
  _draw_heater_status(6, 0);
  #if EXTRUDERS > 1
    _draw_heater_status(31, 1);
    #if EXTRUDERS > 2
      _draw_heater_status(55, 2);
    #endif
  #endif

  // Heatbed
  //_draw_heater_status(81, -1);
 
 // Fan
 //u8g.setFont(FONT_STATUSMENU);
 setPrintPos(LCD_PIXEL_WIDTH - DOG_CHAR_WIDTH*5, STATUS_FANS_Y);
 #if defined(FAN_PIN) && FAN_PIN > -1
 tft_printstr(itostr3(int((fanSpeed*100)/256 + 1)));
 tft_printchar('%');
 #else
 tft_printstr("---");
 #endif
 
 
 // X, Y, Z-Coordinates
 //u8g.setFont(FONT_STATUSMENU);
 //drawBox(0,29,128,10, WHITE);
 //u8g.setColorIndex(0);	// white on black
 //setPrintPos(2,37);
 //tft_printchar('X');
 //u8g.drawPixel(8,33);
 //u8g.drawPixel(8,35);
 //setPrintPos(10,37);
 //tft_printstr(ftostr31ns(current_position[X_AXIS]));
 //setPrintPos(43,37);
 //lcd_printPGM(PSTR("Y"));
 //u8g.drawPixel(49,33);
 //u8g.drawPixel(49,35);
 //setPrintPos(51,37);
 //tft_printstr(ftostr31ns(current_position[Y_AXIS]));
 //setPrintPos(83,37);
 //tft_printchar('Z');
 //u8g.drawPixel(89,33);
 //u8g.drawPixel(89,35);
 //setPrintPos(91,37);
 //tft_printstr(ftostr31(current_position[Z_AXIS]));
 //u8g.setColorIndex(1);	// black on white
 
 // Feedrate
 //u8g.setFont(u8g_font_6x10_marlin);
 setPrintPos(6, STATUS_FR_Y);
 tft_printstr("F/R: ");
 //u8g.setFont(FONT_STATUSMENU);
 tft_printstr(itostr3(feedmultiply));
 tft_printchar('%');

 // Status line
 //u8g.setFont(FONT_STATUSMENU);
 setPrintPos(6, STATUS_LINE_Y);
 #ifndef FILAMENT_LCD_DISPLAY
 tft_printstr(lcd_status_message);
 #else
	if(message_millis+5000>millis()){  //Display both Status message line and Filament display on the last line
		tft_printstr(lcd_status_message);
 	}
 	else
	{
	 lcd_printPGM(PSTR("dia:"));
	 tft_printstr(ftostr12ns(filament_width_meas));
	 lcd_printPGM(PSTR(" factor:"));
	 tft_printstr(itostr3(extrudemultiply));
	 tft_printchar('%');
	}
 #endif 	

}

#define Y_PADDING 20
static void lcd_implementation_drawmenu_generic(uint8_t row, const char* pstr, char pre_char, char post_char)
{
    char c;
    
    uint8_t n = LCD_WIDTH - 1 - 2;
		
		if (pre_char == '>')
		   {
			//SERIAL_ECHO("row: ");
			//SERIAL_ECHOLN((int)row);
			//serialprintPGM(pstr);
			//SERIAL_ECHOLN("");
			//u8g.setColorIndex(1);		// black on white
			drawBox (0, row*DOG_CHAR_HEIGHT + 3 + Y_PADDING, LCD_PIXEL_WIDTH, DOG_CHAR_HEIGHT, WHITE);
			//u8g.setColorIndex(0);		// following text must be white on black
		   } 
		  else 
			drawBox(0, row * DOG_CHAR_HEIGHT + 3 + Y_PADDING, LCD_PIXEL_WIDTH, DOG_CHAR_HEIGHT, BLACK);
		
		setPrintPos(0 * DOG_CHAR_WIDTH, row * DOG_CHAR_HEIGHT + Y_PADDING);
		tft_printchar(pre_char == '>' ? ' ' : pre_char);	// Row selector is obsolete


    while( (c = pgm_read_byte(pstr)) != '\0' )
    {
		  tft_printchar(c);
        pstr++;
        n--;
    }
    while(n--){
					tft_printchar(' ');
		}
	   
		tft_printchar(post_char);
		tft_printchar(' ');
		//u8g.setColorIndex(1);		// restore settings to black on white
}

static void _drawmenu_setting_edit_generic(uint8_t row, const char* pstr, char pre_char, const char* data, bool pgm) {
  char c;
  uint8_t n = LCD_WIDTH - 1 - 2 - (pgm ? strlen_P(data) : strlen(data));
		
  setPrintPos(0 * DOG_CHAR_WIDTH, row * DOG_CHAR_HEIGHT + Y_PADDING);
  tft_printchar(pre_char);

  while( (c = pgm_read_byte(pstr)) != '\0' ) {
	  tft_printchar(c);
    pstr++;
    n--;
  }

  tft_printchar(':');

  while(n--) tft_printchar(' ');

  if (pgm) { lcd_printPGM(data); } else { tft_printstr((char*)data); }
}

#define lcd_implementation_drawmenu_setting_edit_generic(row, pstr, pre_char, data) _drawmenu_setting_edit_generic(row, pstr, pre_char, data, false)
#define lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, pre_char, data) _drawmenu_setting_edit_generic(row, pstr, pre_char, data, true)

#define lcd_implementation_drawmenu_setting_edit_int3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_int3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float43_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float43(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_bool_selected(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_bool(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))

//Add version for callback functions
#define lcd_implementation_drawmenu_setting_edit_callback_int3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_int3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float43_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float43(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_bool_selected(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_callback_bool(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))

void lcd_implementation_drawedit(const char* pstr, char* value)
{
		setPrintPos(0 * DOG_CHAR_WIDTH_LARGE, (tft_getHeight() - 1 - DOG_CHAR_HEIGHT_LARGE) - (1 * DOG_CHAR_HEIGHT_LARGE) - START_ROW );
		//u8g.setFont(u8g_font_9x18);
		lcd_printPGM(pstr);
		tft_printchar(':');
		setPrintPos((14 - strlen(value)) * DOG_CHAR_WIDTH_LARGE, (tft_getHeight() - 1 - DOG_CHAR_HEIGHT_LARGE) - (1 * DOG_CHAR_HEIGHT_LARGE) - START_ROW );
		tft_printstr(value);
}

static void _drawmenu_sd(uint8_t row, const char* pstr, const char* filename, char * const longFilename, bool isDir, bool isSelected) {
  char c;
  uint8_t n = LCD_WIDTH - 1;

  if (longFilename[0] != '\0') {
    filename = longFilename;
    longFilename[n] = '\0';
  }

  if (isSelected) {
    drawBox (0, row*DOG_CHAR_HEIGHT + 3 + Y_PADDING, LCD_PIXEL_WIDTH, DOG_CHAR_HEIGHT, WHITE);
  }
  else
  {
	  drawBox(0, row * DOG_CHAR_HEIGHT + 3+ Y_PADDING, LCD_PIXEL_WIDTH, DOG_CHAR_HEIGHT, BLACK);
  }

  setPrintPos(0 * DOG_CHAR_WIDTH, row * DOG_CHAR_HEIGHT+ Y_PADDING);
  tft_printchar(' ');	// Indent by 1 char

  if (isDir) tft_printstr((char *)LCD_STR_FOLDER);

  while((c = *filename) != '\0') {
	  tft_printchar(c);
    filename++;
    n--;
  }
  while(n--) tft_printchar(' ');

  //if (isSelected) //u8g.setColorIndex(1); // black on white
}

#define lcd_implementation_drawmenu_sdfile_selected(row, pstr, filename, longFilename) _drawmenu_sd(row, pstr, filename, longFilename, false, true)
#define lcd_implementation_drawmenu_sdfile(row, pstr, filename, longFilename) _drawmenu_sd(row, pstr, filename, longFilename, false, false)
#define lcd_implementation_drawmenu_sddirectory_selected(row, pstr, filename, longFilename) _drawmenu_sd(row, pstr, filename, longFilename, true, true)
#define lcd_implementation_drawmenu_sddirectory(row, pstr, filename, longFilename) _drawmenu_sd(row, pstr, filename, longFilename, true, false)

#define lcd_implementation_drawmenu_back_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, LCD_STR_UPLEVEL[0], LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_back(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_submenu_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_submenu(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_gcode_selected(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_gcode(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')
#define lcd_implementation_drawmenu_function_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_function(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')

static void lcd_implementation_quick_feedback()
{

#if BEEPER > -1
    SET_OUTPUT(BEEPER);
    for(int8_t i=0;i<10;i++)
    {
		WRITE(BEEPER,HIGH);
		delay(3);
		WRITE(BEEPER,LOW);
		delay(3);
    }
#endif
}
#endif//ULTRA_LCD_IMPLEMENTATION_DOGM_H
