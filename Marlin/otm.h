//
// otm.h
//
// OpenCreators TFT Menu module header
//
//  Created by James Kang on 2015. 3. 4.
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#ifndef __OTM_H__
#define __OTM_H__

#define COPYRIGHT_H 18
#define STATUS_X 10
#define STATUS_Y 10

#define LOGO_DELAY 500


#define STATE_NONE     0
#define STATE_LOGO     1
#define STATE_MAIN     2
#define STATE_SETTINGS 3
#define STATE_PREPARE  4
#define STATE_JOG      5
#define STATE_FILEBROWSE 6
#define STATE_PRINTING 7
#define STATE_PAUSED   8
#define STATE_PRINTED  9
#define STATE_FILEINFO 10
#define STATE_PSETTINGS 11

#define MAX_BUTTON 16
#define MAX_BUTTON_TITLE 32
typedef struct {
	char active, disabled, freezed, invisible;
	int16_t x, y, w, h, color;
	char title[MAX_BUTTON_TITLE+1];
	const uint8_t *picon, *ptext;
	int cmdid;
} button_t;
button_t button[MAX_BUTTON];

#define COLOR_MAIN_BG  OCE_COLOR_TO_RGB565(0xffffffff)
#define COLOR_BTN_BACK COLOR_MAIN_BG //BLUE
#define COLOR_BTN_EDGE COLOR_MAIN_BG
#define COLOR_BTN_ICON BLACK //YELLOW
#define COLOR_BTN_TEXT BLACK //YELLOW
#define COLOR_BTN_FONT BLACK //YELLOW
#define COLOR_BTN_FRONT_PRESS YELLOW
#define COLOR_BTN_BACK_PRESS BLUE //YELLOW

#define BUTTON_NOT_PRESSED 0
#define BUTTON_PRESSED     1

// menu commands
#define CMDID_PREPARE  0x01
#define CMDID_JOG      0x02
#define CMDID_SETTINGS 0x03
#define CMDID_BACK     0x04
#define CMDID_FILEBROWSE 0x05
#define CMDID_PSETTINGS 0x06

// printing commands
#define CMDID_PRINT    0x11
#define CMDID_PAUSE    0x12
#define CMDID_RESUME   0x13
#define CMDID_STOP     0x14

// jogging commands
#define CMDID_LEFT     0x21
#define CMDID_RIGHT    0x22
#define CMDID_FRONT    0x23
#define CMDID_REAR     0x24
#define CMDID_UP       0x25
#define CMDID_DOWN     0x26
#define CMDID_HOME     0x27
#define CMDID_NPDM     0x28
#define CMDID_G29      0x29
#define CMDID_MOTOROFF 0x2a

// preparation commands
#define CMDID_PREHEAT  0x31
#define CMDID_COOLDOWN 0x32
#define CMDID_EXTRACT  0x33
#define CMDID_RETRACT  0x34

// file list
#define CMDID_FLPREV   0x41
#define CMDID_FLNEXT   0x42
#define CMDID_FLREWIND 0x43
#define CMDID_FLFFWD   0x44
#define CMDID_FLHOME   0x45
#define CMDID_FLEND    0x46

#define CMDID_LINEPOS  0x50

#define CMDID_INPUT0   0x80

#define CMDID_CFGLINE_EEPROM   0xA0
#define CMDID_CFGLINE_PRINTING 0xB0

//
#define CMDID_TEST     0xf9

#endif // __OTM_H__

