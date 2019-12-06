//
// otm.c
//
// OpenCreators TFT Menu module
//
//  Created by James Kang on 2015. 3. 4.
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#ifdef EMULATION
#include "tft_lib2.h"
#else
#include "Marlin.h"
#include "temperature.h"
#include "oc_firmware.h"
#include "oc_sd.h"
#endif
#include "otm.h"

#include "oc_icons.h"

long oc_millis(void);
void tft_init2(void);

extern bool touch_ready;

#define UX_EASY     0
#define UX_ADVANCED 1

typedef struct {
	int  state, prev_state;
	char filename[32], longFilename[64];
	char ux_mode;
	int  coords_x, coords_y;
	int  temp_x, temp_y;
	int  elapsed_time_x, elapsed_time_y;
	int  progress_x, progress_y;
} tftmenu_t;
tftmenu_t tm;

void otm_transit(int state);



long count_event, count_run, count_update;

#ifdef SHOW_COUNTERS
void show_counters(void)
{
	if (!touch_ready) return;

	switch (tm.state) {
	case STATE_MAIN:
	case STATE_JOG:
	case STATE_FILEBROWSE:
	case STATE_PRINTING:
	case STATE_PRINTED:
		break;

	default:
		return;
	}

	#if 1
	static long old=-1;
	long mil = oc_millis();
	if (mil < old+500) return;
	old = mil;
	#endif
	
	char tmp[32], *ss;
	switch (tm.state) {
	case STATE_MAIN:     ss = "MAIN"; break;
	case STATE_PRINTING: ss = "PRINTING"; break;
	case STATE_PRINTED:  ss = "PRINTED"; break;
	case STATE_JOG:      ss = "JOG"; break;
	case STATE_PREPARE:  ss = "PREPARE"; break;
	case STATE_SETTINGS: ss = "SETTINGS"; break;
	case STATE_FILEBROWSE: ss = "FILELIST"; break;
	default: ss = "ERROR"; break;
	}
	sprintf(tmp, "E%02d R%02d U%02d %s", (uint16_t)count_event%100, (uint16_t)count_run%100, (uint16_t)count_update%100, ss);
	tft_fillrect(0, 240-18, 120, 18, BLACK);
	tft_drawtext(1, 240-17, tmp, WHITE, 1);
}
#endif

void counting_event(void)
{
	count_event++;
}

void counting_run(void)
{
	count_run++;
}

void counting_update(void)
{
	count_update++;
}



#define set_sched_state(state) set_sched_state0(__FILE__, __LINE__, state)
static int sched_state = 0; //STATE_NONE;

void set_sched_state0(const char *file, int line, int state)
{
	sched_state = state;

	#ifdef EMULATION
	LOG("[%s:%d]sched_state=%d", file, line, state);
	#else
	SERIAL_PROTOCOL("[");
	SERIAL_PROTOCOL(file);
	SERIAL_PROTOCOL(":");
	SERIAL_PROTOCOL(line);
	SERIAL_PROTOCOL("]");
	SERIAL_PROTOCOLPGM("sched_state=");
	SERIAL_PROTOCOLLN(state);
	#endif
}

int get_sched_state(void)
{
	return sched_state;
}



//int sched_state = STATE_NONE;

#define _BUTTON_____________

#define BUTTON_TYPE_TEXT ((uint8_t *)1)
#define BUTTON_TYPE_FILELINE ((uint8_t *)2)
#define FILELINE_X 8

#define BTN_EDGE_COLOR BLACK
#define BTN_BG_COLOR WHITE
#define BTN_TXT_COLOR BLACK

void button_create_ex(int id, int x, int y, int w, int h, const uint8_t *picon, const uint8_t *ptext, char *title, int cmdid, uint16_t color)
{
	if (id >= 0 && id < MAX_BUTTON) {
		button[id].active = 1;
		button[id].disabled = 0;
		button[id].freezed = 0;
		button[id].invisible = 0;
		button[id].x = x; button[id].y = y; button[id].w = w; button[id].h = h;
		strncpy(button[id].title, title, MAX_BUTTON_TITLE);
		button[id].picon = picon;
		button[id].ptext = ptext;
		button[id].cmdid = cmdid;
		button[id].color = color;
	}
}

void button_create(int id, int x, int y, int w, int h, const uint8_t *picon, const uint8_t *ptext, char *title, int cmdid)
{
	button_create_ex(id, x, y, w, h, picon, ptext, title, cmdid, BLACK);
}

void button_init(void)
{
	int i;
	for (i=0; i<MAX_BUTTON; i++) button[i].active = 0;
//	button_create(0, 10+80*0, 240-16-10-70, 72, 72, icon_continue, text_continue, "");
//	button_create(1, 10+80*1, 240-16-10-70, 72, 72, icon_pause,    text_pause,    "");
//	button_create(2, 10+80*2, 240-16-10-70, 72, 72, icon_cancel,   text_cancel,   "");
}

#if 0
static void draw_button(int x, int y, int w, int h, char *title)
{
	tft_fillrect(x,  y,  w,  h, BTN_BG_COLOR);
	tft_drawrect(x,  y,  w,  h, BTN_EDGE_COLOR);
	int sw = tft_stringwidth(title), sh = tft_stringheight(title);
	tft_printf(x+(w-sw)/2, y+(h-sh)/2, 0, BTN_TXT_COLOR, BTN_BG_COLOR, NULL, title);
}
#endif

void button_draw(button_t *pb, int press)
{
	uint16_t bcolor = COLOR_BTN_BACK, edge_color = COLOR_BTN_EDGE;
	if (pb->cmdid >= CMDID_INPUT0 && pb->cmdid < 0xf0) {
		edge_color = BLACK;
		bcolor = LIGHT_GRAY;
	}
	if (pb->active && !pb->invisible) {
		tft_fillrect(pb->x+0, pb->y+0, pb->w,   pb->h, edge_color);
		if (press == BUTTON_PRESSED) bcolor = COLOR_BTN_BACK_PRESS;
		
		tft_fillrect(pb->x+1, pb->y+1, pb->w-2, pb->h-2, bcolor);
		//tft_setbgcolor(bcolor);

		int color_icon = pb->color/*COLOR_BTN_ICON*/, color_text = COLOR_BTN_TEXT, color_font = COLOR_BTN_FONT;

		if (press) {
			color_icon = COLOR_BTN_FRONT_PRESS;
			color_text = COLOR_BTN_FRONT_PRESS;
			color_font = COLOR_BTN_FRONT_PRESS;
		}

		if (pb->picon == BUTTON_TYPE_TEXT) {
			int sw = tft_stringwidth(pb->title), sh = tft_stringheight(pb->title);
			tft_drawrect(pb->x, pb->y, pb->w, pb->h, BTN_EDGE_COLOR);
			tft_printf(pb->x+(pb->w-sw)/2, pb->y+(pb->h-sh)/2, 0, color_text, bcolor, NULL, "%s", pb->title);
		} else
		if (pb->picon == BUTTON_TYPE_FILELINE) {
			int sh = tft_stringheight(pb->title);
			//tft_drawrect(pb->x, pb->y, pb->w, pb->h, BTN_EDGE_COLOR);
			tft_printf(pb->x+FILELINE_X, pb->y+(pb->h-sh)/2, 0, color_text, bcolor, NULL, "%s", pb->title);
		} else
		if (pb->picon || pb->ptext) {
			//if (press == BUTTON_PRESSED) { r = g = 0; }
			//if (pb->cmdid == CMDID_PREPARE) { r = 1; g = b = 0; }
			if (pb->picon) {
				if (pb->disabled) color_icon = OCE_COLOR_TO_RGB565(0xffa0a0a0);
				tft_setbitmapcolor(color_icon, bcolor);
				tft_drawbitmap(pb->x+1, pb->y+1, 0, pb->picon);
			}
			if (pb->ptext) {
				tft_setbitmapcolor(color_text, bcolor);
				tft_drawbitmap(pb->x+1, pb->y+1, 0, pb->ptext);
			}
		} else {
			tft_setbitmapcolor(color_font, bcolor);
			if (pb->cmdid >= CMDID_INPUT0 && pb->cmdid < 0xf0) {
				int sw = tft_stringwidth(pb->title);
				tft_printf(pb->x+(pb->w-sw)/2, pb->y+10, 0, color_text, bcolor, NULL, "%s", pb->title);
			} else {
				tft_drawtext(pb->x+3, pb->y+3, pb->title, COLOR_BTN_TEXT, 1);
			}
		}
	}
}

void button_drawall(void)
{
	int i;
	for (i=0; i<MAX_BUTTON; i++) button_draw(&button[i], BUTTON_NOT_PRESSED);
}

#define XY_IN_BUTTON(x, y, b) (x > (b).x && x < (b).x+(b).w && y > (b).y && y < (b).y + (b).h)

int button_find_by_xy(int x, int y)
{
	int i;
	for (i=0; i<MAX_BUTTON; i++) {
		if (button[i].active && !button[i].disabled && !button[i].freezed && XY_IN_BUTTON(x, y, button[i])) return i;
	}
	return -1;
}

int target_button_idx = -1;
extern bool in_g29;
extern bool touch_enable_;
void main_update_1sec();

int button_event(int event, int x, int y)
{
	static int old_idx = -1;
	int idx, cmdid = 0;
	static long press_time, release_time;

	main_update_1sec();
	if (in_g29) return 0;
	if (!touch_enable_) return 0;
	#ifdef TOUCH_DISABLE
	return 0;
	#endif

#if 0
	#ifdef MQ_S3
	LOG("button_event(%d, %d, %d)", event, x, y);
	switch (event) {
	case EVENT_TOUCH_PRESSED:  tft_fillrect(x-4, y-4, 9, 9, RED); break;
	case EVENT_TOUCH_RELEASED: tft_fillrect(x-2, y-2, 5, 5, BLUE); break;
	}
	#endif
#endif	

	if (target_button_idx == -1) {
		#if 1
		if (event == EVENT_TOUCH_PRESSED) {
			press_time = millis();
			idx = button_find_by_xy(x, y);
			#ifdef DISABLE_TOUCH_LOG
			#else
			LOG(LT_EVT "button_event(EVENT_TOUCH_PRESSED, x=%d, y=%d) => idx=%d", x, y, idx);
			#endif
			if (idx != -1) {
				old_idx = idx;
				target_button_idx = idx;
				button_draw(&button[idx], BUTTON_PRESSED);
			} else {
				touch_reset();
			}
		}
		#endif
	} else {
		switch (event) {
		case EVENT_TOUCH_MOVED:
			#if 1
			idx = button_find_by_xy(x, y);
			if (idx != old_idx) {
				if (idx == target_button_idx) {
					button_draw(&button[target_button_idx], BUTTON_PRESSED);
				} else {
					if (old_idx == target_button_idx)
						button_draw(&button[target_button_idx], BUTTON_NOT_PRESSED);
				}
				old_idx = idx;
			}
			#endif
			break;

		case EVENT_TOUCH_RELEASED:
			
			release_time = millis();

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// To ignore wrong events caused touch noise(?)
			// The button action which has an interval between two events(pressed and released) will be ignored.
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			#ifndef EMULATION
			if (release_time - press_time < 150) {
				LOGP("!!! invalid button press time, ignored event!!!");
				LOG("    '%s', %d ms", button[idx].title, release_time - press_time);
				button_draw(&button[target_button_idx], BUTTON_NOT_PRESSED);
				target_button_idx = -1;
				release_time = press_time = 0;
				return 0;
			}
			#endif

			idx = button_find_by_xy(x, y);
			#ifdef DISABLE_TOUCH_LOG
			#else
			LOG(LT_EVT "button_event(EVENT_TOUCH_RELEASED, x=%d, y=%d) => idx=%d", x, y, idx);
			#endif
			button_draw(&button[target_button_idx], BUTTON_NOT_PRESSED);
			if (idx == target_button_idx) {
				int pt = release_time - press_time;
				LOG("'%s' selected(pressed_time=%d msec)!", button[idx].title, pt);
				#if 0
				if (!strcmp(button[idx].title, "NPDM")) {
					extern char dbg_buf[80];
					LOG(LT_EVT "%s", dbg_buf);
				}
				#endif
				cmdid = button[idx].cmdid;
			}
			target_button_idx = -1;
			break;
		}
	}
	return cmdid;
}

#define _LOGO_________________
void oc_logo(void)
{
	tft_fillscreen(WHITE);
	tft_setbitmapcolor(WHITE, BLACK);
	/*
	tft_drawbitmap(160-40, 120-40-20, bitmap_logo0);
	tft_drawbitmap(160-80, 120-8+60, bitmap_logo1);
	*/
}

#define _COMMON_________________
void otm_background0(void)
{
	tft_fillrect(0, 240-COPYRIGHT_H, 320, COPYRIGHT_H, BLACK);
	tft_setbitmapcolor(WHITE, BLACK);
	tft_drawbitmap(320-174-10, 240-COPYRIGHT_H+1, 0, bitmap_copyrights);
}

void otm_background(void)
{
//	tft_fillrect(0, 0, 320, 16, WHITE);
	tft_fillrect(0, 0, 320, 240-COPYRIGHT_H, COLOR_MAIN_BG);
//	tft_setbgcolor(COLOR_MAIN_BG);
}

#define _MENU_________________

#include <stdlib.h>

/*
tHotend = int(degHotend(1) + 0.5);
    tTarget = int(degTargetHotend(1) + 0.5);
	ftostr3(current_position[X_AXIS])
		itostr3(feedmultiply) '%'


		if(starttime != 0)
    {
        uint16_t time = millis()/60000 - starttime/60000;
        lcd.print(itostr2(time/60));
        lcd.print(':');
        lcd.print(itostr2(time%60));
    }else{
        lcd_printPGM(PSTR("--:--"));
    }
		*/
#define YG 30

typedef struct {
	char xs[10], ys[10], zs[10];
	char temps[16];
	char times[10];
} stats_t;
stats_t stats;

void stats_clear(void)
{
	stats.xs[0] = 0;
	stats.ys[0] = 0;
	stats.zs[0] = 0;

	stats.temps[0] = 0;
	stats.times[0] = 0;
}

char *oc_ftostr31ns(float x, char *buf)
{
  int xx=x*10;
  //conv[0]=(xx>=0)?'+':'-';
  xx=abs(xx);
  buf[0]=(xx/1000)%10+'0';
  buf[1]=(xx/100)%10+'0';
  buf[2]=(xx/10)%10+'0';
  buf[3]='.';
  buf[4]=(xx)%10+'0';
  buf[5]=0;
  return buf;
}

void get_current_coords(float *px, float *py, float *pz);

void main_update_coords(void)
{
	float x, y, z;
	char buf[32];

	get_current_coords(&x, &y, &z);

	//tft_fillrect(5, STATUS_Y+YG*1, 320, YG, COLOR_MAIN_BG);
	if (tm.coords_y >= 0) {
		tft_printf(tm.coords_x+105*0, tm.coords_y, 0, DARK_RED,   COLOR_MAIN_BG, stats.xs, "X:%s", oc_ftostr31ns(x, buf));
		tft_printf(tm.coords_x+105*1, tm.coords_y, 0, DARK_GREEN, COLOR_MAIN_BG, stats.ys, "Y:%s", oc_ftostr31ns(y, buf));
		tft_printf(tm.coords_x+105*2, tm.coords_y, 0, DARK_BLUE,  COLOR_MAIN_BG, stats.zs, "Z:%s", oc_ftostr31ns(z, buf));
	}
}

void main_update_temp(void)
{
	if (tm.temp_y >= 0) {
		float tHotend = degHotend(0);
		float tTarget = degTargetHotend(0);
		char th[32], tt[32];

		tft_setbitmapcolor(BLACK, COLOR_MAIN_BG);
		tft_drawbitmap(tm.temp_x,  tm.temp_y-4, 0, icon32x32_nozzle_temp);
		tft_printf(tm.temp_x+33, tm.temp_y, 0, RED, COLOR_MAIN_BG, stats.temps, "%s/%s`C", oc_ftostr31ns(tHotend, th), oc_ftostr31ns(tTarget, tt));
	}
}

void get_elapsed_time(char *dst);
void get_progress(uint32_t *, uint32_t *);

void main_update_progress(bool forced, bool finished)
{
	char tmp[10];

	static long old_mil;
	long mil = millis();
	if (!forced && (mil < old_mil+1000)) return;
	old_mil = mil;

	tft_setbitmapcolor(BLACK, COLOR_MAIN_BG);
	tft_drawbitmap(tm.elapsed_time_x,  tm.elapsed_time_y-4, 0, icon32x32_clock);

	get_elapsed_time(tmp);
	tft_printf(tm.elapsed_time_x+33, tm.elapsed_time_y, 0, BLUE, COLOR_MAIN_BG, stats.times, "%s", tmp);

	uint32_t a, b;
	int p;
	get_progress(&a, &b);
	if (finished) {
		a = b;
	}

	// progress
	#define PROGRESS_W 300
	if (b && a < b) {
		int w = a*PROGRESS_W/b;
		tft_fillrect(tm.progress_x-2, tm.progress_y-2, w, 28, LIGHT_GREEN);
		tft_fillrect(tm.progress_x-2+w, tm.progress_y-2, PROGRESS_W-w, 28, LIGHT_GRAY);
		p = a*100/b;
	} else {
		tft_fillrect(tm.progress_x-2, tm.progress_y-2, PROGRESS_W, 28, LIGHT_GREEN);
		p = 100;
	}
	
	tft_printf(tm.progress_x, tm.progress_y, OC_TRANSPARENT_BACK, BLUE, COLOR_MAIN_BG, NULL, "%d%% (%ld/%ld)", p, a, b); 

	#ifdef EMULATION
	if (a >= b) otm_transit(STATE_PRINTED);
	#endif
}

void main_update(void)
{
	switch (tm.state) {
	case STATE_MAIN:
		if (tm.ux_mode != UX_EASY) main_update_coords();
		if (tm.ux_mode != UX_EASY) main_update_temp();
		break;

	case STATE_PRINTED:
		main_update_temp();
		break;

	case STATE_PRINTING:
		main_update_coords();
		main_update_temp();
		main_update_progress(false/*forced*/, false/*finished*/);
		break;

	case STATE_PREPARE:
		main_update_temp();
		break;

	case STATE_JOG:
		main_update_coords();
		break;

	default:
		return;
	}
}

void main_update_1sec(void)
{
	static long omillis = -1;
	long mil = oc_millis();
	if (mil < omillis) omillis = mil;
	if (mil < omillis + 500) return;
	omillis = mil;

	#ifdef SHOW_COUNTERS
	show_counters();
	#endif

	main_update();
}

void main_init(void)
{
	otm_background();

	tft_printf(STATUS_X, STATUS_Y+YG*0, 0, DARK_GREEN, COLOR_MAIN_BG, NULL, "%s Ready", OC_MACHINENAME);

	button_init();

	if (tm.ux_mode == UX_EASY) {

		button_create(0, 10,     60, 145, 150, BUTTON_TYPE_TEXT, NULL, "Print", CMDID_FILEBROWSE);
		button_create(1, 10+155, 60, 145, 150, BUTTON_TYPE_TEXT, NULL, "Setup", CMDID_SETTINGS);

	} else {

		stats_clear();
		tm.coords_y = 40;
		main_update();


		int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
		button_create(0, x+(w+g)*0, y, w, h, icon60x80_file,      NULL,  "File",    CMDID_FILEBROWSE);
		button_create(1, x+(w+g)*1, y, w, h, icon60x80_prepare,   NULL,  "Prepare",  CMDID_PREPARE);
		button_create(2, x+(w+g)*2, y, w, h, icon60x80_jog,       NULL,  "Jog",      CMDID_JOG);
		button_create(3, x+(w+g)*3, y, w, h, icon60x80_settings,  NULL,  "Settings", CMDID_SETTINGS);

	}
	
	button_drawall();
	LOG("%s Ready!", OC_MACHINENAME);
	//tft_on();
}

#define _FILEBROWSE__________
void sd_init(void);
void sd_term(void);

#ifdef USE_OLD_FILEBROWSER
typedef struct {
	int file_count, file_index;
} sd_t;
static sd_t sd;

void filebrowse_drawfile(void)
{
	otm_background();

	button_init();

	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_back,  NULL, "Back",  CMDID_BACK);


	if (sd.file_index < sd.file_count) {

		char gcode_file[32], long_file[64];
		int time, weight;
		sd_getfileinfo(sd.file_index, gcode_file, long_file, &time, &weight);

		//tft_printf(STATUS_X, STATUS_Y, DARK_YELLOW, COLOR_MAIN_BG, "Browse");
		//tft_printf(5, 45, DARK_GREEN,   COLOR_MAIN_BG, "%s %%", "25");

		button_create(1, x+(w+g)*4, y, w, h, icon60x80_print, NULL, "Print", CMDID_PRINT);

		#if 1
		button_create(2,   7, 30+52*0, 50, 50, icon50x50_rewind, NULL,  "Rewind", CMDID_FLREWIND);
		button[2].disabled = 1;
		button[2].freezed = 1;
		#else
		button_create(2,   7, 30+52*0, 50, 50, icon50x50_next,   NULL,  "Next",   CMDID_FLNEXT);
		#endif
		button_create(3,   7, 30+52*1, 50, 50, icon50x50_prev,   NULL,  "Prev",   CMDID_FLPREV);
		button_create(4, 263, 30+52*0, 50, 50, icon50x50_ffwd,   NULL,  "FFwd",   CMDID_FLFFWD);
		button[4].disabled = 1;
		button_create(5, 263, 30+52*1, 50, 50, icon50x50_next,   NULL,  "Next",   CMDID_FLNEXT);

		button_drawall();

		// draw thumbnail
		if (gcode_file[0] == '[') {
			tft_printf(STATUS_X, STATUS_Y, 0, BLACK, COLOR_MAIN_BG, NULL, "%d/%d %s", sd.file_index+1, sd.file_count, gcode_file);
			tft_fillrect(80, 30, 160, 120, DARK_YELLOW);
			tm.filename[0] = 0;
		} else {
			tft_printf(STATUS_X, STATUS_Y, 0, BLACK, COLOR_MAIN_BG, NULL, "%d/%d %s", sd.file_index+1, sd.file_count, long_file);
			tft_drawbitmap_from_gcode(80, 30, gcode_file);
			strcpy(tm.filename, gcode_file);
			strcpy(tm.longFilename, long_file);

			tft_setbitmapcolor(BLUE, COLOR_MAIN_BG);
			tft_drawbitmap(80, 155+2, icon20x20_time);
			tft_drawbitmap(80, 155+25+2, icon20x20_weight);
			tft_printf(100, 155,    0, BLACK, COLOR_MAIN_BG, NULL, "%02d:%02d:%02d", time/3600, (time/60)%60, time % 60);
			tft_printf(100, 155+25, 0, BLACK, COLOR_MAIN_BG, NULL, "%d g", weight);
		}

	} else {
		tft_printf(STATUS_X, STATUS_Y, 0, BLACK, COLOR_MAIN_BG, NULL, "[Error count=%d]", sd.file_count);
	}

}

void filebrowse_init(void)
{
	//sd_init();
	sd.file_count = sd_getcount();
	sd.file_index = 0;

	filebrowse_drawfile();
}

void filebrowse_prev(void)
{
	if (sd.file_index > 0) {
		sd.file_index--;
		filebrowse_drawfile();
	}
}

void filebrowse_next(void)
{
	if (sd.file_index < sd.file_count-1) {
		sd.file_index++;
		filebrowse_drawfile();
	}
}
#else
typedef struct {
	int file_count;
	int page, lpos;
} sd_t;
static sd_t sd;

void filebrowse_getline(char *buf, int bufsize, int pos, int index)
{
	char gcode_file[32], long_file[64];
	int time, weight;
	sd_getfileinfo(index, gcode_file, long_file, &time, &weight);
	
	snprintf(buf, bufsize, "%03d %s", index, long_file);
}

void filebrowse_drawline(int pos, int index, bool selected)
{
	#if 0
	char gcode_file[32], long_file[64];
	int time, weight;
	sd_getfileinfo(index, gcode_file, long_file, &time, &weight);

	if (selected) {
		tft_fillrect(0, 10+pos*25, 320, 25, BLUE);
		tft_printf(8, 10+pos*25, 0, WHITE, BLUE, NULL, "%03d %s", index, long_file);
	} else {
		tft_fillrect(0, 10+pos*25, 320, 25, WHITE);
		tft_printf(8, 10+pos*25, 0, BLACK, WHITE, NULL, "%03d %s", index, long_file);
	}
	#else
	char buf[32];
	filebrowse_getline(buf, sizeof(buf), pos, index);
	if (selected) {
		tft_fillrect(0, 10+pos*25, 320, 25, BLUE);
		tft_printf(FILELINE_X, 10+pos*25, 0, WHITE, BLUE, NULL, "%s", buf);
	} else {
		tft_fillrect(0, 10+pos*25, 320, 25, WHITE);
		tft_printf(FILELINE_X, 10+pos*25, 0, BLACK, WHITE, NULL, "%s", buf);
	}
	#endif
}

void filebrowse_drawpage(int page)
{
	#if 0
	tft_fillrect(0, 0, 320, 140, WHITE);

	int i;
	for (i=0; i<5 && page*5+i < sd.file_count; i++) {
		LOG("i=%d", i);
		filebrowse_drawline(i, page*5+i, sd.lpos == i);
		LOG("i=%d done", i);
	}
	#else
	int i;
	for (i=0; i<5 && page*5+i < sd.file_count; i++) {
		if (sd.lpos == i) filebrowse_drawline(i, page*5+i, sd.lpos == i);
	}
	#endif

	tft_fillrect(110, 140, 320-220, 26, WHITE);
	tft_printf(110, 140, 0, BLACK, WHITE, NULL, "%03d/%03d", sd.page+1, sd.file_count/5+1);
}

void fb_button_changestatus(int index, bool enable, bool draw)
{
	if (button[index].disabled != !enable) {
		button[index].disabled = !enable;
		if (draw) {
			button_draw(&button[index], BUTTON_NOT_PRESSED);
		}
	}
}

void fb_button_setstatus(bool draw)
{
	fb_button_changestatus(2, sd.page > 0, draw);
	fb_button_changestatus(3, sd.page*5+sd.lpos > 0, draw);
	fb_button_changestatus(4, sd.page*5+sd.lpos < sd.file_count-1, draw);
	fb_button_changestatus(5, sd.page*5+4 < sd.file_count, draw);
}

void filebrowse_reinit(void)
{
	otm_background();

	button_init();
	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_back,  NULL, "Back",  CMDID_BACK);
	button_create(1, x+(w+g)*4, y, w, h, icon60x80_print, NULL, "Print", CMDID_PRINT);

	button_create(2, x+(w+g)*1-6+50*0, y+25, 50, 50, icon50x50_rewind, NULL,  "Rewind", CMDID_FLREWIND);
	button_create(3, x+(w+g)*1-6+50*1, y+25, 50, 50, icon50x50_prev,   NULL,  "Prev",   CMDID_FLPREV);
	button_create(4, x+(w+g)*1-6+50*2, y+25, 50, 50, icon50x50_next,   NULL,  "Next",   CMDID_FLNEXT);
	button_create(5, x+(w+g)*1-6+50*3, y+25, 50, 50, icon50x50_ffwd,   NULL,  "FFwd",   CMDID_FLFFWD);

	int i;
	for (i=0; i<5 && sd.page*5+i < sd.file_count; i++) {
		char buf[32];
		filebrowse_getline(buf, sizeof(buf), i, sd.page*5+i);
		button_create(6+i, 0, 10+i*25, 320, 25, BUTTON_TYPE_FILELINE,   NULL,  buf,   CMDID_LINEPOS+i);
		//button[6+i].invisible = 1;
	}

	fb_button_setstatus(false);
	button_drawall();

	filebrowse_drawpage(sd.page);
}

void filebrowse_init(void)
{
	sd.file_count = sd_getcount();
	sd.page = 0;
	sd.lpos = 0;

	filebrowse_reinit();
}

void filebrowse_prevpage(bool rotate_lpos)
{
	if (sd.page > 0) {
		sd.page--;
		if (rotate_lpos) sd.lpos = 4;
		filebrowse_reinit();
		filebrowse_drawpage(sd.page);

		fb_button_setstatus(true);
	}
}

void filebrowse_nextpage(bool rotate_lpos)
{
	if (sd.page*5+4 < sd.file_count) {
		sd.page++;
		if (rotate_lpos) sd.lpos = 0;
		else {
			while (sd.lpos && sd.page*5+sd.lpos > sd.file_count-1) sd.lpos--;
		}
		filebrowse_reinit();
		filebrowse_drawpage(sd.page);

		fb_button_setstatus(true);
	}
}

void filebrowse_prev(void)
{
	if (sd.lpos > 0) {
		filebrowse_drawline(sd.lpos, sd.page*5+sd.lpos, false);
		sd.lpos--;
		filebrowse_drawline(sd.lpos, sd.page*5+sd.lpos, true);
		fb_button_setstatus(true);
	} else {
		filebrowse_prevpage(true);
	}
}

void filebrowse_next(void)
{
	if (sd.lpos < 4) {
		filebrowse_drawline(sd.lpos, sd.page*5+sd.lpos, false);
		sd.lpos++;
		filebrowse_drawline(sd.lpos, sd.page*5+sd.lpos, true);
		
		fb_button_setstatus(true);
	} else {
		filebrowse_nextpage(true);	
	}
}

void filebrowser_select_line(int line)
{
	if (line != sd.lpos) {
		filebrowse_drawline(sd.lpos, sd.page*5+sd.lpos, false);
		sd.lpos = line;
		filebrowse_drawline(sd.lpos, sd.page*5+sd.lpos, true);
		
		fb_button_setstatus(true);
	} else {
		otm_transit(STATE_FILEINFO);
	}
}

#endif

#define _FILEINFO_______________

void fileinfo_init(void)
{
	otm_background();

	//tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG,  NULL, "File Info.");

	button_init();

	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_back,  NULL,  "Main", CMDID_BACK);
	button_create(1, x+(w+g)*1, y, w, h, icon60x80_print, NULL, "Print", CMDID_PRINT);

	button_drawall();

	char gcode_file[32], long_file[64];
	int time, weight;
	sd_getfileinfo(sd.page*5+sd.lpos, gcode_file, long_file, &time, &weight);

	// draw thumbnail
	if (gcode_file[0] == '[') {
		tft_printf(15, 10, 0, BLACK, COLOR_MAIN_BG, NULL, "%d/%d %s", sd.page*5+sd.lpos+1, sd.file_count, gcode_file);
		tft_fillrect(80, 30, 160, 120, DARK_YELLOW);
		tm.filename[0] = 0;
	} else {
		tft_printf(15, 10, 0, BLACK, COLOR_MAIN_BG, NULL, "%s", long_file);

		// bitmap
		x = 150; y = 50;
		tft_drawbitmap_from_gcode(x, y, gcode_file);
		strcpy(tm.filename, gcode_file);
		strcpy(tm.longFilename, long_file);

		// info
		x = 15; y = 60;
		tft_setbitmapcolor(BLUE, COLOR_MAIN_BG);
		tft_drawbitmap(x, y+25*1, 0, icon20x20_time);
		tft_drawbitmap(x, y+25*2, 0, icon20x20_weight);

		tft_printf(x,     y+25*0, 0, BLACK, COLOR_MAIN_BG, NULL, "%d/%d", sd.page*5+sd.lpos+1, sd.file_count);
		tft_printf(x+20,  y+25*1, 0, BLACK, COLOR_MAIN_BG, NULL, "%02d:%02d:%02d", time/3600, (time/60)%60, time % 60);
		tft_printf(x+20,  y+25*2, 0, BLACK, COLOR_MAIN_BG, NULL, "%d g", weight);
	}
}



#define _PRINTING_______________

void printing_init(void)
{
	otm_background();

	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG,  NULL, "Printing");
	tft_printf(STATUS_X, STATUS_Y+30, 0, BLACK, COLOR_MAIN_BG,  NULL, "%s", tm.longFilename);

	stats_clear();
	tm.temp_y = 70;
	if (tm.ux_mode == UX_EASY) {
		tm.coords_y = -1; //COORDS_Y1;
	} else {
		tm.coords_y = 40; //COORDS_Y1;
	}
	main_update();

	button_init();

	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_pause,    NULL,  "Pause", CMDID_PAUSE);
	button_create(1, x+(w+g)*1, y, w, h, icon60x80_stop,     NULL,  "Stop",  CMDID_STOP);
	button_create(2, x+(w+g)*2, y, w, h, icon60x80_settings, NULL,  "Settings", CMDID_PSETTINGS);

	button_drawall();
}

#define _PRINTED_______________

void printed_init(void)
{
	otm_background();

	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG,  NULL, "Printed");
	tft_printf(STATUS_X, STATUS_Y+30, 0, BLACK, COLOR_MAIN_BG,  NULL, "%s", tm.longFilename);

	stats_clear();
	tm.temp_y = 70;
	main_update_temp();
	main_update_progress(true/*forced*/, true/*finished*/);

	button_init();

	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_back,  NULL,  "Main", CMDID_BACK);

	button_drawall();
}

#define _PAUSE_______________

void pause_init(void)
{
	otm_background();

	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG, NULL, "Paused");
	tft_printf(STATUS_X, STATUS_Y+30, 0, BLACK, COLOR_MAIN_BG,  NULL, "%s", tm.longFilename);

	stats_clear();
	main_update_temp();
	main_update_progress(true/*forced*/, false/*finished*/);

	button_init();

	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_continue, NULL,  "Resume", CMDID_RESUME);
	button_create(1, x+(w+g)*1, y, w, h, icon60x80_stop,     NULL,  "Stop",   CMDID_STOP);
	button_create(2, x+(w+g)*2, y, w, h, icon60x80_jog,       NULL,  "Jog",      CMDID_JOG);

	button_drawall();
}

#define _PREPARE_______________

void prepare_init(void)
{
	otm_background();

	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG, NULL, "Prepare");

	stats_clear();
//	tm.coords_y = 25;
	tm.temp_y = 40;
	main_update();

	button_init();

	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_back,      NULL,  "Back",     CMDID_BACK);
	button_create(1, x+(w+g)*1, y, w, h, icon60x80_preheat,   NULL,  "Preheat",  CMDID_PREHEAT);
	button_create(2, x+(w+g)*2, y, w, h, icon60x80_cooldown,  NULL,  "Cooldown", CMDID_COOLDOWN);
	button_create(3, x+(w+g)*3+5,    y, 50, 50, icon50x50_down,    NULL,  "Extract",  CMDID_EXTRACT);
	button_create(4, x+(w+g)*3+5+55, y, 50, 50, icon50x50_up,      NULL,  "Retract",  CMDID_RETRACT);

	button_drawall();
}

#define _JOG_______________

void jog_init(void)
{
	otm_background();

	//tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG, NULL, "Jog");

	stats_clear();
	tm.coords_y = 10;
	main_update();

	button_init();

	int w = 50, h = 50, g = 5, jx = 20, jy = 35;

	button_create_ex(0, jx+(w+g)*0, jy+(h+g)*1, w, h, icon50x50_left,      NULL,  "Left",    CMDID_LEFT, DARK_RED);
	button_create_ex(1, jx+(w+g)*1, jy+(h+g)*0, w, h, icon50x50_up,        NULL,  "Rear",    CMDID_REAR, DARK_GREEN);
	button_create(2,    jx+(w+g)*1, jy+(h+g)*1, w, h, icon50x50_home,      NULL,  "Home",    CMDID_HOME);
	button_create_ex(3, jx+(w+g)*1, jy+(h+g)*2, w, h, icon50x50_down,      NULL,  "Front",   CMDID_FRONT, DARK_GREEN);
	button_create_ex(4, jx+(w+g)*2, jy+(h+g)*1, w, h, icon50x50_right,     NULL,  "Right",   CMDID_RIGHT, DARK_RED);

	int jzx = 190, jzy = 35;
	button_create_ex(5, jzx, jzy, w, h, icon50x50_up,        NULL,  "Up",    CMDID_UP, DARK_BLUE);
	button_create_ex(6, jzx, jzy+55, w, h, icon50x50_down,      NULL,  "Down",  CMDID_DOWN, DARK_BLUE);

	int sx = 260, sy = 35;

	button_create(7, sx, sy+55*0, w, h, icon50x50_g29,        NULL,  "G29",    CMDID_G29);
	button_create(8, sx, sy+55*1, w, h, icon50x50_npdm,       NULL,  "NPDM",   CMDID_NPDM);
	button_create(9, sx, sy+55*2, w, h, icon50x50_motoroff,   NULL,  "MOFF",   CMDID_MOTOROFF);

	button_create(10, 2+(w+g)*0, 240-18-82, 60, 80, icon60x80_back,  NULL,  "Back",  CMDID_BACK);

	button_drawall();
}


#define _SETTINGS______________

int otm_preheat_temp = 185, otm_printing_speed = 100, otm_fan_speed = 100;

void settings_init(void)
{
	#if 1
	otm_background();
	//tft_fixDir();
	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG, NULL, "Settings");

	button_init();
	//button_create(0, 10+80*0, 240-16-10-70, 72, 72, icon_cancel,  text_cancel,  "Back",  CMDID_BACK);
	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_back,  NULL,  "Back",  CMDID_BACK);
	button_create(1, x+(w+g)*1, y, w, h, icon60x80_prepare,   NULL,  "Prepare",  CMDID_PREPARE);
	button_create(2, x+(w+g)*2, y, w, h, icon60x80_jog,       NULL,  "Jog",      CMDID_JOG);


	button_create(6, x+(w+g)*4, y, w, h, icon60x80_jog,       NULL,  "Jog",      0);


//	sd_init();


	x = 10; y = 40;
	char tt[32], tmp[32];
	sprintf(tt, "Nozzle: %d `C", otm_preheat_temp);
	button_create(3, 10, y+0*30, 300, 30, BUTTON_TYPE_TEXT, NULL, tt, CMDID_CFGLINE_EEPROM+0);
	sprintf(tt, "Speed: %d %%", otm_printing_speed);
	button_create(4, 10, y+1*30, 300, 30, BUTTON_TYPE_TEXT, NULL, tt, CMDID_CFGLINE_EEPROM+1);
	sprintf(tt, "FAN Speed: %d %%", otm_fan_speed);
	button_create(5, 10, y+2*30, 300, 30, BUTTON_TYPE_TEXT, NULL, tt, CMDID_CFGLINE_EEPROM+2);


	button_drawall();


	#else
	
	#endif
	
}

#define _PSETTINGS______________
int prt_nozzle_temp = 185, prt_printing_speed = 100, prt_fan_speed = 100;
void printing_settings_init(void)
{
	otm_background();
	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, COLOR_MAIN_BG, NULL, "Settings");

	button_init();
	int g = 4, w=60, h = 80, x = 2, y = 240-16-h-2;
	button_create(0, x+(w+g)*0, y, w, h, icon60x80_back,  NULL,  "Back",  CMDID_BACK);

	x = 10; y = 40;
	char tt[32], tmp[32];
	sprintf(tt, "Nozzle: %d `C", prt_nozzle_temp);
	button_create(1, 10, y+0*30, 300, 30, BUTTON_TYPE_TEXT, NULL, tt, CMDID_CFGLINE_PRINTING+0);
	sprintf(tt, "Speed: %d %%", prt_printing_speed);
	button_create(2, 10, y+1*30, 300, 30, BUTTON_TYPE_TEXT, NULL, tt, CMDID_CFGLINE_PRINTING+1);
	sprintf(tt, "FAN Speed: %d %%", prt_fan_speed);
	button_create(3, 10, y+2*30, 300, 30, BUTTON_TYPE_TEXT, NULL, tt, CMDID_CFGLINE_PRINTING+2);

	button_drawall();
}

#define _INPUT______________

#define VT_NOZZLE_TEMP 1
#define VT_SPEED       2
#define VT_FAN_SPEED   3
#define VALUE_W 100
int current_vt;
char value[16];

void draw_value(void)
{
	int sw = tft_stringwidth(value);
	tft_fillrect(10+1, 60+1, VALUE_W-2, 35-2, WHITE);
	tft_printf(14, 66, 0, BLACK, WHITE, NULL, "%s", value);
	tft_fillrect(14+sw, 64, 3, 27, BLACK);
}

void input_value(int type, int *pval)
{
	char tt[16];

	current_vt = type;
	otm_background();

	button_init();
	tft_fillrect(0, 0, 320, 30, BLUE);

	switch (type) {
	case VT_NOZZLE_TEMP:
		tft_printf(10, 2, 0, WHITE, BLUE, NULL, "Nozzle Temp.: %d `C", *pval);
		//tft_printf(10, 35, 0, BLACK, COLOR_MAIN_BG, NULL, "Crnt:%s", oc_ftostr31ns(cfg.nozzle_temp, tt));
		break;

	case VT_SPEED:
		tft_printf(10, 2, 0, WHITE, BLUE, NULL, "Print Speed: %d %%", *pval);
		//tft_printf(10, 35, 0, BLACK, COLOR_MAIN_BG, NULL, "Crnt:%d", cfg.speed);
		break;

	case VT_FAN_SPEED:
		tft_printf(10, 2, 0, WHITE, BLUE, NULL, "FAN Speed: %d %%", *pval);
		//tft_printf(10, 35, 0, BLACK, COLOR_MAIN_BG, NULL, "Crnt:%d", cfg.fan_speed);
		break;
	}

	tft_drawrect(10, 60, VALUE_W, 35, BLACK);

	int g = 4, w=40, h = 40, x = 140, y = 40;
	button_create(0, x+(w+g)*0, y+(h+g)*3, w*2+g, h, NULL,  NULL,  "0",  CMDID_INPUT0+0);
	button_create(1, x+(w+g)*0, y+(h+g)*2, w, h, NULL,  NULL,  "1",  CMDID_INPUT0+1);
	button_create(2, x+(w+g)*1, y+(h+g)*2, w, h, NULL,  NULL,  "2",  CMDID_INPUT0+2);
	button_create(3, x+(w+g)*2, y+(h+g)*2, w, h, NULL,  NULL,  "3",  CMDID_INPUT0+3);
	button_create(4, x+(w+g)*0, y+(h+g)*1, w, h, NULL,  NULL,  "4",  CMDID_INPUT0+4);
	button_create(5, x+(w+g)*1, y+(h+g)*1, w, h, NULL,  NULL,  "5",  CMDID_INPUT0+5);
	button_create(6, x+(w+g)*2, y+(h+g)*1, w, h, NULL,  NULL,  "6",  CMDID_INPUT0+6);
	button_create(7, x+(w+g)*0, y+(h+g)*0, w, h, NULL,  NULL,  "7",  CMDID_INPUT0+7);
	button_create(8, x+(w+g)*1, y+(h+g)*0, w, h, NULL,  NULL,  "8",  CMDID_INPUT0+8);
	button_create(9, x+(w+g)*2, y+(h+g)*0, w, h, NULL,  NULL,  "9",  CMDID_INPUT0+9);

	button_create(10, x+(w+g)*2, y+(h+g)*3, w, h, NULL,  NULL,  ".",  CMDID_INPUT0+10);
	button_create(11, x+(w+g)*3, y+(h+g)*0, w, h, NULL,  NULL,  "<",  CMDID_INPUT0+11);
	button_create(12, x+(w+g)*3, y+(h+g)*1, w, h, NULL,  NULL,  "O",  CMDID_INPUT0+12);
	button_create(13, x+(w+g)*3, y+(h+g)*2, w, h, NULL,  NULL,  "X",  CMDID_INPUT0+13);

	button_drawall();

	value[0] = 0;
	draw_value();
	
}

void value_add(char c)
{
	int len = strlen(value);
	if (len < 15) {
		value[len] = c;
		value[len+1] = 0;
		draw_value();
	}
}

void value_delete(void)
{
	int len = strlen(value);
	if (len) {
		value[len-1] = 0;
		draw_value();
	}
}

#define _MAIN_______________

void otm_init(void)
{
	// initialize global structure 'tm'
	tm.ux_mode = UX_EASY;    // need to read from the EEPROM
	tm.state = STATE_LOGO;
	tm.coords_x = 5; tm.coords_y = 40;
	tm.temp_x   = 5; tm.temp_y   = 70;
	tm.elapsed_time_x = 210; tm.elapsed_time_y = 70;
	tm.progress_x = 10; tm.progress_y = 100;

	sd_init0();
	
	tft_init();
	tft_off();
	oc_logo();
	tft_on();

	delay(LOGO_DELAY);

	tft_off();
	tft_fillscreen(COLOR_MAIN_BG);
	otm_background0();
	tft_on();

	touch_ready = true;
	otm_transit(STATE_MAIN);
}

void sd_printstart(const char* filename, char* longFilename);

bool usb_printing = false;

void otm_sched_finished(void)
{
	usb_printing = false;
	set_sched_state(STATE_PRINTED);
	LOGP("sched_state = STATE_PRINTED");
}

void otm_sched_main(void)
{
	usb_printing = false;
	set_sched_state(STATE_MAIN);
	LOGP("sched_state = STATE_MAIN");
}

void otm_sched_prepare(void)
{
	set_sched_state(STATE_PREPARE);
	LOGP("sched_state = STATE_PREPARE");
}

void otm_sched_jog(void)
{
	set_sched_state(STATE_JOG);
	LOGP("sched_state = STATE_JOG");
}

void otm_sched_settings(void)
{
	set_sched_state(STATE_SETTINGS);
	LOGP("sched_state = STATE_SETTINGS");
}

void otm_sched_printing(void)
{
	usb_printing = true;
	set_sched_state(STATE_PRINTING);
	LOGP("sched_state = STATE_PRINTING");

	sd_getFilename(tm.filename, sizeof(tm.filename));
	sd_getLongfilename(tm.longFilename, sizeof(tm.longFilename));
//	strncpy(tm.filename, filename, sizeof(tm.filename));
//	strncpy(tm.longFilename, longFilename, sizeof(tm.longFilename));
}


static void otm_print_start(const char* filename, char* longFilename)
{
	LOGP("######## otm_print_start ########");
	LOG("file='%s'", longFilename);
	sd_printstart(filename, longFilename);
}

#define TLQ_SIZE 100
char tlq[TLQ_SIZE+1];
int tlq_pos = 0;
bool in_tlq_out = false;

void __tft_log(char c)
{
	if (in_tlq_out) return;
	if (tlq_pos < TLQ_SIZE) {
		tlq[tlq_pos++] = c;
	}
}

void __tlq_out()
{
	return;
	static uint16_t t_color = YELLOW;
	in_tlq_out = 1;
	static int tlq_x = 0, tlq_y = 0;
	int i;
	for (i=0; i<tlq_pos; i++) {
		char tmp[2];
		tmp[0] = tlq[i]; tmp[1] = 0;
		tft_fillrect(tlq_x, 100+tlq_y, 6, 10, BLACK);
		tft_drawtext(tlq_x, 100+tlq_y, tmp, t_color, 1);
		tlq_x += 6;
		if (tlq_x >= 320) {
			tlq_x = 0;
			tlq_y += 10;
			if (tlq_y >= 40) {
				tlq_y = 0;
				t_color ^= (YELLOW ^ GREEN);
			}
		}
	}
	tlq_pos = 0;
	in_tlq_out = 0;
}

void otm_test(void)
{
	tft_init();
	otm_transit(STATE_MAIN);
}

void Config_StoreSettings(void);

void touch_event(int event, int x, int y)
{
	counting_event();
	__tlq_out();

	#if 0
	if (sched_state != STATE_NONE) {
		LOGP("!!!!! otm_transit(sched_state) !!!!!");
		#if 0
		if (sched_state == STATE_PRINTED) {
			tft_init();
			tft_init2();
		}
		#endif
		otm_transit(sched_state);
		sched_state = STATE_NONE;
		return;
	}
	#endif
	int cmdid;
	if (event == EVENT_TOUCH_PRESSED && y < 40 && x > 320-40) cmdid = CMDID_TEST; else
	cmdid = button_event(event, x, y);
	//LOG("touch_event: cmdid=%d", cmdid);
	switch (cmdid) {
	case CMDID_TEST: otm_test(); break;
	case CMDID_FILEBROWSE: otm_transit(STATE_FILEBROWSE); break;
	case CMDID_PRINT:
		{
			int time, weight;
			sd_getfileinfo(sd.page*5+sd.lpos, tm.filename, tm.longFilename, &time, &weight);			
		}
		otm_print_start(tm.filename, tm.longFilename);
		//otm_transit(STATE_PRINTING);
		break;
	case CMDID_PAUSE:
		sd_pause();
		set_sched_state(STATE_PAUSED);
		//otm_transit(STATE_PAUSED);
		break;
	case CMDID_RESUME:
		otm_transit(STATE_PRINTING);
		sd_resume();
		break;
	case CMDID_PREPARE: otm_transit(STATE_PREPARE);  break;
	case CMDID_JOG:
		tm.prev_state = tm.state;
		otm_transit(STATE_JOG);
		break;
	case CMDID_SETTINGS:otm_transit(STATE_SETTINGS); break;
	case CMDID_PSETTINGS:
		tm.prev_state = tm.state;
		set_sched_state(STATE_PSETTINGS);
		//otm_transit(STATE_PSETTINGS);
		break;
	//case CMDID_TEST:	tftmenu_transit(STATE_TEST);     break;

	case CMDID_HOME:    tftcmd_home();     break;
	case CMDID_LEFT:    tftcmd_left();     break;
	case CMDID_RIGHT:   tftcmd_right();    break;
	case CMDID_REAR:    tftcmd_rear();     break;
	case CMDID_FRONT:   tftcmd_front();    break;
	case CMDID_UP:      tftcmd_up();       break;
	case CMDID_DOWN:    tftcmd_down();     break;
	case CMDID_G29:     tftcmd_g29();      break;
	case CMDID_NPDM:    tftcmd_npdm();     break;
	case CMDID_MOTOROFF:tftcmd_motoroff(); break;
	case CMDID_PREHEAT: 
		//tftcmd_preheat();  
		fw_settemp(otm_preheat_temp);
		fw_setfanspeed(otm_fan_speed);
		break;
	case CMDID_COOLDOWN:
		tftcmd_cooldown();
		fw_setfanspeed(0);
		break;

	case CMDID_FLPREV:  filebrowse_prev(); break;
	case CMDID_FLNEXT:  filebrowse_next(); break;
	case CMDID_FLREWIND:filebrowse_prevpage(false); break; 
	case CMDID_FLFFWD:  filebrowse_nextpage(false); break;

	case CMDID_LINEPOS+0:
	case CMDID_LINEPOS+1:
	case CMDID_LINEPOS+2:
	case CMDID_LINEPOS+3:
	case CMDID_LINEPOS+4: filebrowser_select_line(cmdid - CMDID_LINEPOS); break;

	case CMDID_CFGLINE_EEPROM+0: input_value(1, &otm_preheat_temp); break;
	case CMDID_CFGLINE_EEPROM+1: input_value(2, &otm_printing_speed); break;
	case CMDID_CFGLINE_EEPROM+2: input_value(3, &otm_fan_speed); break;

	case CMDID_CFGLINE_PRINTING+0: input_value(1, &prt_nozzle_temp); break;
	case CMDID_CFGLINE_PRINTING+1: input_value(2, &prt_printing_speed); break;
	case CMDID_CFGLINE_PRINTING+2: input_value(3, &prt_fan_speed); break;


	case CMDID_INPUT0+0:
	case CMDID_INPUT0+1:
	case CMDID_INPUT0+2:
	case CMDID_INPUT0+3:
	case CMDID_INPUT0+4:
	case CMDID_INPUT0+5:
	case CMDID_INPUT0+6:
	case CMDID_INPUT0+7:
	case CMDID_INPUT0+8:
	case CMDID_INPUT0+9: value_add(cmdid-CMDID_INPUT0+'0'); break;
	case CMDID_INPUT0+10: value_add('.'); break;
	case CMDID_INPUT0+11: value_delete(); break;

	case CMDID_INPUT0+12: // OK
		if (tm.state == STATE_SETTINGS) {
			switch (current_vt) {
			case VT_NOZZLE_TEMP: otm_preheat_temp   = atoi(value); Config_StoreSettings(); break;
			case VT_SPEED:       otm_printing_speed = atoi(value); Config_StoreSettings(); break;
			case VT_FAN_SPEED:   otm_fan_speed      = atoi(value); Config_StoreSettings(); break;
			}
		} else {
			switch (current_vt) {
			case VT_NOZZLE_TEMP: prt_nozzle_temp    = atoi(value); fw_settemp(prt_nozzle_temp); break;
			case VT_SPEED:       prt_printing_speed = atoi(value); fw_setspeed(prt_printing_speed); break;
			case VT_FAN_SPEED:   prt_fan_speed      = atoi(value); fw_setfanspeed(prt_fan_speed); break;
			}
		}
		settings_init();
		break;

	case CMDID_INPUT0+13: // Cancel
		settings_init();
		break;

	case CMDID_STOP:
		sd_stop();
		set_sched_state(STATE_MAIN);
		//otm_transit(STATE_MAIN);
		break;
	case CMDID_BACK:
		if (tm.prev_state) {
			otm_transit(tm.prev_state);
			tm.prev_state = 0;
		} else
		switch (tm.state) {
		case STATE_PREPARE:
		case STATE_JOG:
			otm_transit(STATE_SETTINGS); break;

		case STATE_FILEBROWSE:
		case STATE_PRINTING:
		case STATE_PRINTED:
		//case STATE_TEST:
		case STATE_SETTINGS: otm_transit(STATE_MAIN); break;

		case STATE_PSETTINGS: otm_transit(STATE_PRINTING); break;

		case STATE_FILEINFO: 
			tm.state = STATE_FILEBROWSE;
			filebrowse_reinit();
			break;

		case STATE_PAUSED:
			otm_transit(STATE_PRINTING);
			break;
		}
		break;
	}
}

#define TTTTTTTTTTTTTTTTTTTTTTTTTTT
void otm_transit(int state)
{
	if (tm.state == state) return;

	tm.state = state;
	switch (state) {
	case STATE_MAIN:
		LOGP("##### otm_transit(MAIN)#####");
		main_init();
		break;

	case STATE_PREPARE:
		prepare_init();
		break;

	case STATE_FILEBROWSE:
		filebrowse_init();
		break;

	case STATE_PRINTING:
		LOGP("##### otm_transit(PRINTING)#####");
		printing_init();
		break;

	case STATE_PRINTED:
		LOGP("##### otm_transit(PRINTED)#####");
		printed_init();
		break;

	case STATE_PAUSED:
		pause_init();
		break;

	case STATE_JOG:
		jog_init();
		break;

	case STATE_SETTINGS:
		settings_init();
		break;

	case STATE_PSETTINGS:
		printing_settings_init();
		break;

	case STATE_FILEINFO:
		fileinfo_init();
		break;
	}
}
#define TTTTTTTTTTTTTTTTTTTTTTTTTTTT

bool otm_isPrinting(void)
{
	return sd_isPrinting() || usb_printing;
}

void otm_run(char *loc)
{
	counting_run();
	if (get_sched_state() != STATE_NONE) {
		if (strstr(loc, "loop")) {
			LOGP("!!!!! otm_run/otm_transit(sched_state) !!!!!");
			otm_transit(get_sched_state());
			set_sched_state(STATE_NONE);
			return;
		} else {
			LOGP("!!!!! loc is not loop !!!!!");
			#ifndef EMULATION
			MYSERIAL.println(loc);
			#endif
		}
	}

	switch (tm.state) {
	case STATE_PRINTING:
		if (!otm_isPrinting()) {
			#if 0
			LOG("###### STATE_PRINTING => STATE_PRINTED");
			//otm_transit(STATE_MAIN);
			//otm_sched_finished();
			tm.state = STATE_PRINTED; // DO NOT DRAW
			return;
			break;
			#else
			set_sched_state(STATE_PRINTED);
			#endif
		} else {
			bool bk = touch_enable_;
			touch_enable_ = true;
			touch_run(tm.state, "loop");
			touch_enable_ = bk;
			return;
		}
		break;

	case STATE_PRINTED:
		//if (sched_state == STATE_NONE) // only once
		//	otm_sched_main();
		break;
		
	case STATE_MAIN:
		//main_update_1sec();
		break;

	case STATE_FILEBROWSE:
	case STATE_PAUSED:
	case STATE_PREPARE:
	case STATE_JOG:
	case STATE_SETTINGS:
	//case STATE_TEST:
		//tftmenu_update();
		break;
	}
	touch_run(tm.state, loc);
}

void otm_run2(void)
{
	switch (tm.state) {
	case STATE_PRINTING:
		break;
		
	case STATE_MAIN:
		//main_update_1sec();
		break;

	case STATE_FILEBROWSE:
	case STATE_PAUSED:
	case STATE_PREPARE:
	case STATE_JOG:
	case STATE_SETTINGS:
	//case STATE_TEST:
		//touch_run();
		//tftmenu_update();
		break;
	}
}

void otm_update(char *loc)
{
	counting_update();
	//LOG("--------------- octft_update() ---------------");

	//
	// SD card detection
	//
	int sd_rc = sd_detection();
	if (sd_rc == OC_SD_INSERTED) {
		tft_init();
		tft_off();
		otm_background0();
		otm_transit(STATE_MAIN);	
		tft_on();
	}

	//
	// Touch event
	//
	otm_run(loc);

	//
	//
	//
	#if 0
	static int a = 0;
	switch (a) {
	case 0: otm_drawtemp(0.0, 185.01); break;
	case 1: otm_drawtemp(10.0, 185.01); break;
	case 2: otm_drawtemp(100.0, 185.01); break;
	case 3: otm_drawtemp(180.0, 185.01); break;
	}
	a++;
	if (a == 4) a = 0;
	#endif
	main_update_1sec();
//	static int cnt;
	//tft_counter_inc(0, 240-17, &cnt, 999);
}

void otm_setstatus(const char* message)
{
	LOG("otm_setstatus: msg=%s", message);
}

#if 1
#define BLEN_A 0
#define BLEN_B 1
#define BLEN_C 2

    #define EN_C (1<<BLEN_C)
    #define EN_B (1<<BLEN_B)
    #define EN_A (1<<BLEN_A)

#define BTN_EN1 31
#define BTN_EN2 33
#define BTN_ENC 35

uint32_t blocking_enc;
uint8_t lastEncoderBits;
int8_t encoderDiff; /* encoderDiff is updated from interrupt context and added to encoderPosition every LCD update */

    #define encrot0 0
    #define encrot1 2
    #define encrot2 3
    #define encrot3 1

volatile uint8_t buttons;//Contains the bits of the currently pressed buttons.

void otm_buttons_update()
{
	#ifdef EMULATION
	#else
    uint8_t newbutton=0;
    if(digitalRead(BTN_EN1)==0)  newbutton|=EN_A;
    if(digitalRead(BTN_EN2)==0)  newbutton|=EN_B;
  #if BTN_ENC > 0
    if((blocking_enc<millis()) && (digitalRead(BTN_ENC)==0))
        newbutton |= EN_C;
  #endif
  	if (buttons != newbutton) {
		SERIAL_PROTOCOLPGM("buttons=");
		SERIAL_PROTOCOLLN(buttons);
  	}
    buttons = newbutton;

    //manage encoder rotation
    uint8_t enc=0;
    if(buttons&EN_A)
        enc|=(1<<0);
    if(buttons&EN_B)
        enc|=(1<<1);
    if(enc != lastEncoderBits)
    {
		SERIAL_PROTOCOLPGM("enc=");
		SERIAL_PROTOCOLLN(enc);

        switch(enc)
        {
        case encrot0:
            if(lastEncoderBits==encrot3)
                encoderDiff++;
            else if(lastEncoderBits==encrot1)
                encoderDiff--;
            break;
        case encrot1:
            if(lastEncoderBits==encrot0)
                encoderDiff++;
            else if(lastEncoderBits==encrot2)
                encoderDiff--;
            break;
        case encrot2:
            if(lastEncoderBits==encrot1)
                encoderDiff++;
            else if(lastEncoderBits==encrot3)
                encoderDiff--;
            break;
        case encrot3:
            if(lastEncoderBits==encrot2)
                encoderDiff++;
            else if(lastEncoderBits==encrot0)
                encoderDiff--;
            break;
        }
    }
    lastEncoderBits = enc;
	#endif
}
#endif

void otm_reset_alert_level()
{
}

