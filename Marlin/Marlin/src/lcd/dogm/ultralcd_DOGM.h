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
#pragma once

/**
 * lcd/dogm/ultralcd_DOGM.h
 */

#include "../../inc/MarlinConfigPre.h"



#ifndef LCD_PIXEL_WIDTH
  #define LCD_PIXEL_WIDTH 320
#endif
#ifndef LCD_PIXEL_HEIGHT
  #define LCD_PIXEL_HEIGHT 240
#endif

extern int page;

// For selective rendering within a Y range
#define PAGE_OVER(ya)         ((ya) >= (page*LCD_PIXEL_HEIGHT) ) // Does the current page follow a region top?
#define PAGE_UNDER(yb)        ((yb) <= (page+1)*LCD_PIXEL_HEIGHT - 1 ) // Does the current page precede a region bottom?
#define PAGE_CONTAINS(ya, yb) ((yb) <= ((page+1)*LCD_PIXEL_HEIGHT - 1) && (ya) >= ((page*LCD_PIXEL_HEIGHT))) // Do two vertical regions overlap?



