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
#include "../inc/MarlinConfig.h"

#define LCD_PIXEL_WIDTH	  320
#define LCD_PIXEL_HEIGHT  240

#define MENU_FONT_WIDTH   12
#define MENU_FONT_ASCENT  20
#define MENU_FONT_DESCENT 20
#define MENU_FONT_HEIGHT  (MENU_FONT_ASCENT + MENU_FONT_DESCENT)

#define EDIT_FONT_WIDTH   MENU_FONT_WIDTH
#define EDIT_FONT_ASCENT  MENU_FONT_ASCENT
#define EDIT_FONT_DESCENT MENU_FONT_DESCENT
#define EDIT_FONT_HEIGHT  MENU_FONT_HEIGHT


#define INFO_FONT_WIDTH   MENU_FONT_WIDTH
#define INFO_FONT_ASCENT  MENU_FONT_ASCENT
#define INFO_FONT_DESCENT MENU_FONT_DESCENT
#define INFO_FONT_HEIGHT  MENU_FONT_HEIGHT
