//
//  octm.h
//
//  OpenCreators TFT Menu module header
//
//  2015. 4. 7.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#ifndef __OCTM_H__
#define __OCTM_H__

#ifdef UI_V2
//#define COLOR_MAIN_BG  OCE_COLOR_TO_RGB565(0xff000000)
//#define COLOR_MAIN_BG  OCE_COLOR_TO_RGB565(0xff000040)
//#define COLOR_CURRENT_BUTTON  OCE_COLOR_TO_RGB565(0xffc0c000)
#else
#define COLOR_MAIN_BG  OCE_COLOR_TO_RGB565(0xffffffff)
#define COLOR_CURRENT_BUTTON  OCE_COLOR_TO_RGB565(0xffc0c000)
#endif

#define COPYRIGHT_H 18

//
// LCD Menu states
//
#define OTM_STATE_NONE       0
#define OTM_STATE_LOGO       1
#define OTM_STATE_MAIN       2
#define OTM_STATE_SETTINGS   3
#define OTM_STATE_PREPARE    4
#define OTM_STATE_JOG        5
#define OTM_STATE_FILEBROWSE 6
#define OTM_STATE_PRINTING   7
#define OTM_STATE_PAUSED     8
#define OTM_STATE_PRINTED    9
#define OTM_STATE_FILEINFO  10
#define OTM_STATE_PSETTINGS 11
#define OTM_STATE_INFO      12

#define OTM_STATE_EDITVALUE 13
#define OTM_STATE_ASKVALUE  14

#define OTM_STATE_ERROR          96
#define OTM_STATE_ERROR_RESOLVED 97
#define OTM_STATE_TESTMENU       98
#define OTM_STATE_HIDDENMENU     99

//
// menu commands
//
#define CMDID_PREPARE    0x01
#define CMDID_JOG        0x02
#define CMDID_SETTINGS   0x03
#define CMDID_BACK       0x04
#define CMDID_FILEBROWSE 0x05
#define CMDID_PSETTINGS  0x06
#define CMDID_INFO       0x07

// printing commands
#define CMDID_PRINT    0x11
#define CMDID_PAUSE    0x12
#define CMDID_RESUME   0x13
#define CMDID_STOP     0x14
#define CMDID_PRINT_SDFILE 0x15
#define CMDID_CHDIR    0x16
#define CMDID_UPDIR    0x17

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
#define CMDID_EXTRUDE  0x33
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
// Constants
//
#define STATUS_X 10
#define STATUS_Y 10

#define DEFAULT_TITLE_COLOR OCE_COLOR_TO_RGB565(0xffffffff)
#define DEFAULT_FG_COLOR OCE_COLOR_TO_RGB565(0xfff0f0f0) // OCE_COLOR_TO_RGB565(0xffc0c0c0)
#define DEFAULT_BG_COLOR OCE_COLOR_TO_RGB565(0xff000000)
#define DEFAULT_PROGRESS_COLOR OCE_COLOR_TO_RGB565(0xff0d91ff)//OCE_COLOR_TO_RGB565(0xff0d91e4)
#define DEFAULT_PROGRESS_MODIFIED_COLOR OCE_COLOR_TO_RGB565(0xffffa000)
#define DEFAULT_COLOR_CURRENT_BUTTON  OCE_COLOR_TO_RGB565(0xffc0c000)
#define DEFAULT_INFO_COLOR OCE_COLOR_TO_RGB565(0xfff0f0f0)

#define SPEED_DISPLAY_TIME_COUNT 10 // 5-sec

typedef struct {
	bool bg_is_dark;
	uint16_t title;
	uint16_t text, background;
	uint16_t progress;
	uint16_t speed_rate, speed_rate_modified;
	uint16_t current_button;
	uint16_t info;
	uint16_t btn_fg, btn_bg, btn_edge;
} colors_t;

#define STATE_STACK_LEN 8
#define MAX_UICOLORSET_MAX 1
typedef struct {
	int state;
	int state_stack[STATE_STACK_LEN+1];
	int state_sp;
	char filename[32], longFilename[64];
	char subdir_depth;
	int sched_state;
	int coords_x, coords_y;
	int temp_x, temp_y;
	int elapsed_time_x, elapsed_time_y;
	int timeleft_x, timeleft_y;
	int progress_x, progress_y;

	int speed_rate, old_speed_rate, oldold_speed_rate;
	int speed_display_count;

	colors_t color;
	int uicolorset_index;
} OTM_t;

#endif // __OCTM_H__

