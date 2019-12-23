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

/**
 * lcdprint.cpp
 */

#include "../inc/MarlinConfigPre.h"

#if HAS_SPI_LCD

#include "lcdprint.h"
#include "../core/language.h"

/**
 * lcd_put_u8str_ind_P
 * Print a string with an index substituted within it
 */
int lcd_put_u8str_ind_P(PGM_P const pstr, const uint8_t ind, const lcd_uint_t maxlen/*=LCD_WIDTH*/) {
  
  uint8_t *p = (uint8_t*)pstr;
  lcd_uint_t n = maxlen;
  for (; n; n--) {
    wchar_t ch;
	
    p = get_utf8_value_cb(p, read_byte_rom, &ch);
    if (!ch) break;
    lcd_put_wchar(ch);
  }
  return n;
}

#endif // HAS_SPI_LCD
