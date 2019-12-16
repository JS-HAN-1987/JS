/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../inc/MarlinConfigPre.h"
#include "ultralcd.h"
#include "ultralcd_OC_TFT.h"

#if ENABLED(OC_TFT)

void MarlinUI::clear_lcd() {

}
void MarlinUI::init_lcd() {

}

void MarlinUI::set_custom_characters(const HD44780CharSet screen_charset) {

}

void MarlinUI::draw_status_screen() {

}


void MarlinUI::draw_kill_screen() {

}

bool MarlinUI::detected() {

}

#endif




#if HAS_LCD_MENU

  #include "menu/menu.h"

  #if ENABLED(ADVANCED_PAUSE_FEATURE)

    void MarlinUI::draw_hotend_status(const uint8_t row, const uint8_t extruder) {
      /*if (row < LCD_HEIGHT) {
        lcd_moveto(LCD_WIDTH - 9, row);
        _draw_heater_status((heater_ind_t)extruder, LCD_STR_THERMOMETER[0], get_blink());
      }*/
    }

  #endif // ADVANCED_PAUSE_FEATURE

  // Draw a static item with no left-right margin required. Centered by default.
  void MenuItem_static::draw(const uint8_t row, PGM_P const pstr, const uint8_t style/*=SS_DEFAULT*/, const char * const valstr/*=nullptr*/) {
    /*int8_t n = LCD_WIDTH;
    lcd_moveto(0, row);
    if ((style & SS_CENTER) && !valstr) {
      int8_t pad = (LCD_WIDTH - utf8_strlen_P(pstr)) / 2;
      while (--pad >= 0) { lcd_put_wchar(' '); n--; }
    }
    n = lcd_put_u8str_ind_P(pstr, itemIndex, n);
    if (valstr) n -= lcd_put_u8str_max(valstr, n);
    for (; n > 0; --n) lcd_put_wchar(' ');*/
  }

  // Draw a generic menu item with pre_char (if selected) and post_char
  void MenuItemBase::_draw(const bool sel, const uint8_t row, PGM_P const pstr, const char pre_char, const char post_char) {
    /*lcd_put_wchar(0, row, sel ? pre_char : ' ');
    uint8_t n = lcd_put_u8str_ind_P(pstr, itemIndex, LCD_WIDTH - 2);
    for (; n; --n) lcd_put_wchar(' ');
    lcd_put_wchar(post_char);*/
  }

  // Draw a menu item with a (potentially) editable value
  void MenuEditItemBase::draw(const bool sel, const uint8_t row, PGM_P const pstr, const char* const data, const bool pgm) {
    /*const uint8_t vlen = data ? (pgm ? utf8_strlen_P(data) : utf8_strlen(data)) : 0;
    lcd_put_wchar(0, row, sel ? LCD_STR_ARROW_RIGHT[0] : ' ');
    uint8_t n = lcd_put_u8str_ind_P(pstr, itemIndex, LCD_WIDTH - 2 - vlen);
    if (vlen) {
      lcd_put_wchar(':');
      for (; n; --n) lcd_put_wchar(' ');
      if (pgm) lcd_put_u8str_P(data); else lcd_put_u8str(data);
    }*/
  }

  // Low-level draw_edit_screen can be used to draw an edit screen from anyplace
  void MenuEditItemBase::draw_edit_screen(PGM_P const pstr, const char* const value/*=nullptr*/) {
    //ui.encoder_direction_normal();

    //uint8_t n = lcd_put_u8str_ind_P(0, 1, pstr, itemIndex, LCD_WIDTH - 1);
    //if (value != nullptr) {
    //  lcd_put_wchar(':');
    //  int len = utf8_strlen(value);
    //  const lcd_uint_t valrow = (n < len + 1) ? 2 : 1;          // Value on the next row if it won't fit
    //  lcd_put_wchar((LCD_WIDTH - 1) - (len + 1), valrow, ' ');  // Right-justified, padded, leading space
    //  lcd_put_u8str(value);
    //}
  }

  // The Select Screen presents a prompt and two "buttons"
  void MenuItem_confirm::draw_select_screen(PGM_P const yes, PGM_P const no, const bool yesno, PGM_P const pref, const char * const string/*=nullptr*/, PGM_P const suff/*=nullptr*/) {
   /* ui.draw_select_screen_prompt(pref, string, suff);
    SETCURSOR(0, LCD_HEIGHT - 1);
    lcd_put_wchar(yesno ? ' ' : '['); lcd_put_u8str_P(no); lcd_put_wchar(yesno ? ' ' : ']');
    SETCURSOR_RJ(utf8_strlen_P(yes) + 2, LCD_HEIGHT - 1);
    lcd_put_wchar(yesno ? '[' : ' '); lcd_put_u8str_P(yes); lcd_put_wchar(yesno ? ']' : ' ');*/
  }

  #if ENABLED(SDSUPPORT)

    void MenuItem_sdbase::draw(const bool sel, const uint8_t row, PGM_P const, CardReader &theCard, const bool isDir) {
      /*lcd_put_wchar(0, row, sel ? LCD_STR_ARROW_RIGHT[0] : ' ');
      constexpr uint8_t maxlen = LCD_WIDTH - 2;
      uint8_t n = maxlen - lcd_put_u8str_max(ui.scrolled_filename(theCard, maxlen, row, sel), maxlen);
      for (; n; --n) lcd_put_wchar(' ');
      lcd_put_wchar(isDir ? LCD_STR_FOLDER[0] : ' ');*/
    }

  #endif

  #if ENABLED(LCD_HAS_STATUS_INDICATORS)

    void MarlinUI::update_indicators() {
      // Set the LEDS - referred to as backlights by the LiquidTWI2 library
      //static uint8_t ledsprev = 0;
      //uint8_t leds = 0;

      //#if HAS_HEATED_BED
      //  if (thermalManager.degTargetBed() > 0) leds |= LED_A;
      //#endif

      //#if HOTENDS
      //  if (thermalManager.degTargetHotend(0) > 0) leds |= LED_B;
      //#endif

      //#if FAN_COUNT > 0
      //  if (0
      //    #if HAS_FAN0
      //      || thermalManager.fan_speed[0]
      //    #endif
      //    #if HAS_FAN1
      //      || thermalManager.fan_speed[1]
      //    #endif
      //    #if HAS_FAN2
      //      || thermalManager.fan_speed[2]
      //    #endif
      //  ) leds |= LED_C;
      //#endif // FAN_COUNT > 0

      //#if HOTENDS > 1
      //  if (thermalManager.degTargetHotend(1) > 0) leds |= LED_C;
      //#endif

      //if (leds != ledsprev) {
      //  lcd.setBacklight(leds);
      //  ledsprev = leds;
      //}
    }

  #endif // LCD_HAS_STATUS_INDICATORS

  #if ENABLED(AUTO_BED_LEVELING_UBL)


    void MarlinUI::ubl_plot(const uint8_t x_plot, const uint8_t y_plot) {
  }
  #endif // AUTO_BED_LEVELING_UBL

#endif // HAS_LCD_MENU
