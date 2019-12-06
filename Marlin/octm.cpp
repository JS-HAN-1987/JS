//
//  octm.c
//
//  OpenCreators TFT Menu module
//
//  2015. 4. 7.
//  Created by James Kang
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
#include "octm.h"
#include "octml.h"

#include "oc_tft.h"
#include "oc_lut_but.h"
#include "oc_encoder.h"
#include "oc_tooldetector.h"

#ifdef UI_V2
#include "icons.h"
#else
#include "oc_icon.h"
#endif

#define xFOR_MANUAL
#define xSHOW_UPDATE_EVENT // log on otm_update() periodically every 1 second

extern bool in_dco;
extern bool tft_enable_;
extern bool uio_drawrequest;

void encoder_test(void);
void otm_state_change(int state);

OTM_t otm;

void color_select(int id)
{
	otm.uicolorset_index = id;
	switch (id) {
	case 0:
		otm.color.title      = DEFAULT_TITLE_COLOR;
		otm.color.text       = DEFAULT_FG_COLOR;
		otm.color.background = DEFAULT_BG_COLOR;

		otm.color.current_button = DEFAULT_COLOR_CURRENT_BUTTON;
		
		otm.color.progress   = DEFAULT_PROGRESS_COLOR;
		otm.color.speed_rate = DEFAULT_PROGRESS_COLOR;
		otm.color.speed_rate_modified = DEFAULT_PROGRESS_MODIFIED_COLOR;

		otm.color.info       = DEFAULT_INFO_COLOR;
		otm.color.btn_fg     = WHITE;
		otm.color.btn_bg     = BLACK;
		otm.color.btn_edge   = WHITE;
		otm.color.bg_is_dark = true;
		break;

	case 1:
		otm.color.title      = BLACK;
		otm.color.text       = BLACK;
		otm.color.background = WHITE;

		otm.color.current_button = DEFAULT_PROGRESS_COLOR;
		
		otm.color.progress   = DARK_GREEN;
		otm.color.speed_rate = BLUE;
		otm.color.speed_rate_modified = DEFAULT_PROGRESS_MODIFIED_COLOR;

		otm.color.info       = GRAY;	
		otm.color.btn_fg     = BLACK;
		otm.color.btn_bg     = WHITE;
		otm.color.btn_edge   = BLACK;
		otm.color.bg_is_dark = false;
		break;

	case 2:
		otm.color.title      = YELLOW;
		otm.color.text       = YELLOW;
		otm.color.background = DARK_BLUE;

		otm.color.current_button = DEFAULT_PROGRESS_COLOR;
		
		otm.color.progress   = GREEN;
		otm.color.speed_rate = YELLOW;
		otm.color.speed_rate_modified = DEFAULT_PROGRESS_MODIFIED_COLOR;

		otm.color.info       = GRAY;	
		otm.color.btn_fg     = BLACK;
		otm.color.btn_bg     = OCE_COLOR_TO_RGB565(0xffe0e0e0);
		otm.color.btn_edge   = BLACK;
		otm.color.bg_is_dark = true;
		break;
	}
	OCT_Button_SetCurrentColor(otm.color.current_button);
}

void color_init(void)
{
	color_select(0);
}

void otm_color_changeset(int id)
{
	color_select(id);
	int new_state = otm.state;
	otm.state = -1;
	otm_state_change(new_state);	
}

long oc_millis(void);

char *otm_state_string(char *dst, int state)
{
	switch (state) {
	case OTM_STATE_MAIN:       strncpy_P(dst, PSTR("MAIN"), 15);       break;
	case OTM_STATE_FILEBROWSE: strncpy_P(dst, PSTR("FILEBROWSE"), 15); break;
	case OTM_STATE_PRINTING:   strncpy_P(dst, PSTR("PRINTING"), 15);   break;
	case OTM_STATE_PRINTED:    strncpy_P(dst, PSTR("PRINTED"), 15);    break;
	case OTM_STATE_PAUSED:     strncpy_P(dst, PSTR("PAUSED"), 15);     break;
	case OTM_STATE_SETTINGS:   strncpy_P(dst, PSTR("SETTINGS"), 15);   break;
	case OTM_STATE_PSETTINGS:  strncpy_P(dst, PSTR("PSETTINGS"), 15);  break;
	case OTM_STATE_PREPARE:    strncpy_P(dst, PSTR("PREPARE"), 15);    break;
	case OTM_STATE_JOG:        strncpy_P(dst, PSTR("JOG"), 15);        break;

	case OTM_STATE_HIDDENMENU: strncpy_P(dst, PSTR("HIDDENMENU"), 15); break;
	default: sprintf_P(dst, PSTR("(state=%d)"), state); break;
	}
	return dst;
}

char *otm_current_state_string(char *dst)
{
	return otm_state_string(dst, otm.state);
}

#define _COUNTS_______

#ifdef SHOW_COUNTERS
long count_event, count_run, count_update, count_button_update;
void show_counters(void)
{
	switch (otm.state) {
	case OTM_STATE_MAIN:
	case OTM_STATE_JOG:
	case OTM_STATE_FILEBROWSE:
	case OTM_STATE_PRINTING:
	case OTM_STATE_PAUSED:
	case OTM_STATE_PRINTED:
	case OTM_STATE_SETTINGS:
	case OTM_STATE_PREPARE:
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
	
	char tmp[32], tt[16];
	sprintf_P(tmp, PSTR("E%02d R%02d U%02d %s"), (uint16_t)count_event%100, (uint16_t)count_run%100, 
		(uint16_t)count_update%100, otm_state_string(tt, otm.state));

	tft_fillrect(0, 240-18, 135, 18, BLACK);
	tft_drawtext(1, 240-17, tmp, WHITE, 1);
}

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

void counting_button_update(void)
{
	count_button_update++;
}

#else
#define counting_event()
#define counting_run()
#define counting_update()
#define counting_button_update()
#endif

#define _UPDATE_FW_STATS_________

void get_current_coords(float *px, float *py, float *pz);

typedef struct {
	int percent;
	char xs[10], ys[10], zs[10];
	char temps[16], srs[10], ets[10], tls[10];
	char xt, yt, zt;
	char file[32];
} stats_t;
stats_t stats;

void stats_clear(void)
{
	stats.percent = -1;

	stats.xs[0] = 0;
	stats.ys[0] = 0;
	stats.zs[0] = 0;

	stats.temps[0] = 0;
	stats.srs[0] = 0;
	stats.ets[0] = 0;
	stats.tls[0] = 0;

	stats.xt = -1;
	stats.yt = -1;
	stats.zt = -1;

	stats.file[0] = 0;
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

void update_all_every_1sec(bool force);

void update_init(void)
{
	stats_clear();
	update_all_every_1sec(true);
}

void update_coords(int cx, int cy)
{
	#ifdef UI_V2
	float x, y, z;
	char buf[32];
	int xx = cx, xy = cy+30*0, yx = cx, yy = cy+30*1, zx = cx, zy = cy+30*2;

	get_current_coords(&x, &y, &z);

	if (otm.state == OTM_STATE_TESTMENU) {
		xx = otm.coords_x+105*0;
		yx = otm.coords_x+105*1;
		zx = otm.coords_x+105*2;
		xy = yy = zy = otm.coords_y;
	}
	uint16_t xcol = RED, ycol = GREEN, zcol = BLUE;

	if (tft_type == TFTTYPE_ILI9325) zcol = OCE_COLOR_TO_RGB565(0xffc0c0ff);

	if (!otm.color.bg_is_dark) {
		xcol = DARK_RED;
		ycol = DARK_GREEN;
		zcol = DARK_BLUE;
	}
	
	tft_printf(xx, xy, 0, xcol, otm.color.background, stats.xs, "X:%s", oc_ftostr31ns(x, buf));
	tft_printf(yx, yy, 0, ycol, otm.color.background, stats.ys, "Y:%s", oc_ftostr31ns(y, buf));
	tft_printf(zx, zy, 0, zcol, otm.color.background, stats.zs, "Z:%s", oc_ftostr31ns(z, buf));

	// display end-stop trigger status
	char xt, yt, zt;
	get_endstop_status(&xt, &yt, &zt);

	int w = tft_stringwidth("*"), h = tft_stringheight("*");
	#define XO 83
	#define YO (0)
	if (stats.xt != xt) {
		tft_fillrect(xx+XO, xy+YO, w, h, otm.color.background);
		if (xt) tft_printf(xx+XO, xy+YO, 0, xcol, otm.color.background, NULL, "*");
		stats.xt = xt;
	}
	if (stats.yt != yt) {
		tft_fillrect(yx+XO, yy+YO, w, h, otm.color.background);
		if (yt) tft_printf(yx+XO, yy+YO, 0, ycol, otm.color.background, NULL, "*");
		stats.yt = yt;
	}
	if (stats.zt != zt) {
		tft_fillrect(zx+XO, zy+YO, w, h, otm.color.background);
		if (zt) tft_printf(zx+XO, zy+YO, 0, zcol, otm.color.background, NULL, "*");
		stats.zt = zt;
	}

	#else
	float x, y, z;
	char buf[32];

	get_current_coords(&x, &y, &z);

	//tft_fillrect(5, STATUS_Y+YG*1, 320, YG, otm.color.background);
	if (otm.coords_y >= 0) {
		tft_printf(otm.coords_x+105*0, otm.coords_y, 0, DARK_RED,   otm.color.background, stats.xs, "X:%s", oc_ftostr31ns(x, buf));
		tft_printf(otm.coords_x+105*1, otm.coords_y, 0, DARK_GREEN, otm.color.background, stats.ys, "Y:%s", oc_ftostr31ns(y, buf));
		tft_printf(otm.coords_x+105*2, otm.coords_y, 0, DARK_BLUE,  otm.color.background, stats.zs, "Z:%s", oc_ftostr31ns(z, buf));
	}
	#endif
}

static uint16_t color4temp(float temp, uint16_t c0, uint16_t c1, uint16_t c2, uint16_t c3, uint16_t c4)
{
	if (temp < 10) return c0;  // BLUE
	if (temp < 30) return c1;  // GREEN
	if (temp < 100) return c2; // YELLOW
	if (temp < 180) return c3; // ORANGE
	return c4; // RED
}

void update_temp(int tx, int ty)
{
	#ifdef UI_V2
	char th[32], tt[32];

	tft_setbitmapcolor(otm.color.text, otm.color.background);
//	if (stats.temps[0] == 0) // first time only
	tft_drawbitmap(tx,  ty-5, 0, icon14x33_temp);
#ifdef FOR_MANUAL
	int c1 = DARK_GREEN, c2 = BLACK;
	if (otm.color.bg_is_dark) { c1 = GREEN; c2 = WHITE; }
	tft_printf(25+16, 100, 0, c1, otm.color.background, NULL, "%s", "17.5`C");
	int w = tft_stringwidth("17.5`C");
	tft_printf(25+16+w+8, 100, 0, c2, otm.color.background, NULL, "%s", "/");
	tft_printf(25+16, 100+25, 0, c2, otm.color.background, NULL, "%s`C", "0");
#else
	int c1, c2, c3;
	if (otm.color.bg_is_dark) {
		c1 = color4temp(degHotend(0), BLUE, GREEN, YELLOW, ORANGE, RED);
		c2 = WHITE;
		c3 = color4temp(degTargetHotend(0), BLUE, GREEN, YELLOW, ORANGE, RED);
	} else {
		c1 = color4temp(degHotend(0), DARK_BLUE, DARK_GREEN, DARK_YELLOW, DARK_ORANGE, DARK_RED);
		c2 = BLACK;
		c3 = color4temp(degTargetHotend(0), DARK_BLUE, DARK_GREEN, DARK_YELLOW, DARK_ORANGE, DARK_RED);
	}
	char tmp[32];
	sprintf(tmp, "%s`C", oc_ftostr31ns(degHotend(0), th));
	tft_printf(tx+16,     ty,    0, c1, otm.color.background, NULL, "%s", tmp);
	int w = tft_stringwidth(tmp);
	tft_printf(tx+16+w+8, ty,    0, c2, otm.color.background, NULL, "%s", "/");
	tft_printf(tx+16,     ty+25, 0, c3, otm.color.background, NULL, "%s`C", oc_ftostr31ns(degTargetHotend(0), tt));
#endif
	#else
	#ifdef OC_TFT
	if (otm.temp_y >= 0) {
		float tHotend = degHotend(0);
		float tTarget = degTargetHotend(0);
		char th[32], tt[32];

		tft_setbitmapcolor(BLACK, otm.color.background);
		if (stats.temps[0] == 0) // first time only
			tft_drawbitmap(otm.temp_x,  otm.temp_y-4, 0, icon_nozzle_temp);
		tft_printf(otm.temp_x+33, otm.temp_y, 0, RED, otm.color.background, stats.temps, "%s/%s`C", oc_ftostr31ns(tHotend, th), oc_ftostr31ns(tTarget, tt));
	}
	#endif
	#endif
}

void update_speedrate(void)
{
	#ifdef UI_V2
	if (otm.speed_display_count) otm.speed_display_count--;
	if (otm.speed_display_count == 0) {
		tft_fillrect(320-25-100, 25, 100+2, 25+2, otm.color.background);
		return;
	}
	#endif
	
	tft_setbitmapcolor(otm.color.text, otm.color.background);
	char s[10];
	sprintf_P(s, PSTR("%d%%"), otm.speed_rate);
	if (strcmp(s, stats.srs) || otm.oldold_speed_rate != otm.old_speed_rate) {
		otm.oldold_speed_rate = otm.old_speed_rate;
		int w = tft_stringwidth(stats.srs), h = tft_stringheight(stats.srs);
		#ifdef UI_V2
		tft_fillrect(320-25-w, 25, w, h, otm.color.background);	
		#else
		tft_fillrect(otm.temp_x+300-w, otm.temp_y, w, h, otm.color.background);	
		#endif
		w = tft_stringwidth(s);

		uint16_t color;
		if (otm.old_speed_rate != otm.speed_rate) color = otm.color.speed_rate_modified; else color = otm.color.speed_rate;
		#ifdef UI_V2
		tft_printf(320-25-w, 25, 0, color, otm.color.background, NULL, "%s", s);
		#else
		tft_printf(otm.temp_x+300-w, otm.temp_y, 0, color, otm.color.background, NULL, "%s", s);
		#endif
		strcpy(stats.srs, s);
	}
}

void get_elapsed_time(char *dst);
void get_timeleft(char *dst, uint32_t a, uint32_t b);

void get_progress(uint32_t *, uint32_t *);

#ifdef BIG_PERCENT
int draw_bignumber_char(int x, int y, int flags, char c, bool draw);

int draw_bignumber(int x, int y, int flags, char *str, bool draw)
{
	int w = 0, i;
	int len = strlen(str);
	for (i=0; i<len; i++) {
		w += draw_bignumber_char(x+w, y, 0, str[i], draw);
	}
	return w;
}

void draw_bigpercent(int x, int y, int p)
{
	char tmp[10];
	sprintf_P(tmp, PSTR("%d%%"), p);
	int w = draw_bignumber(0, 0, 0, tmp, false);
	int ox = x+(154-w)/2;
	tft_setbitmapcolor(otm.color.progress, otm.color.background);
	draw_bignumber(ox, y, 0, tmp, true);
}
#endif

void update_progress(bool forced, bool finished)
{
	#ifdef UI_V2

	uint32_t a, b;
	get_progress(&a, &b);
	int p = sd_percent(), w, h;
	if (finished) { a = b; p = 100; }

	char tmp[10];
	int px = 140, py = 110;

	if (stats.percent != p) {

		// progress bar
		#define PROGRESS_W 154
		#define PROGRESS_H 14
		tft_drawrect(px, py, PROGRESS_W, PROGRESS_H, otm.color.btn_edge);
		w = (PROGRESS_W - 6)*p / 100;
		if (w > 0) tft_fillrect(px+3, py+3, w, PROGRESS_H-6, otm.color.progress);

		#ifdef BIG_PERCENT
		draw_bigpercent(px, py+PROGRESS_H+5, p);
		#else
		sprintf_P(tmp, PSTR("%d%%"), p);
		w = tft_stringwidth(tmp);
		tft_printf(px+(PROGRESS_W-w)/2, py+PROGRESS_H+5, 0, otm.color.progress, otm.color.background, NULL, "%s", tmp);
		#endif
		stats.percent = p;
	}

	// times	
	tft_setbitmapcolor(otm.color.text, otm.color.background);

	get_elapsed_time(tmp);
	if (strcmp(stats.ets, tmp)) {
		w = tft_stringwidth(stats.ets);
		h = tft_stringheight(stats.ets);
		tft_fillrect(px, py-30, w, h, otm.color.background);
		tft_printf(px, py-30, 0, otm.color.text, otm.color.background, NULL, "%s", tmp);
		strcpy(stats.ets, tmp);
	}

	get_timeleft(tmp, a, b);
	if (strcmp(stats.tls, tmp)) {
		w = tft_stringwidth(stats.tls);
		h = tft_stringheight(stats.tls);
		tft_fillrect(px+PROGRESS_W-w, py-30, w, h, otm.color.background);
		w = tft_stringwidth(tmp);
		tft_printf(px+PROGRESS_W-w, py-30, 0, otm.color.text, otm.color.background, NULL, "%s", tmp);
		strcpy(stats.tls, tmp);
	}
	
	#else
	#ifdef OC_TFT
	char tmp[10];

	uint32_t a, b;
	int p;
	get_progress(&a, &b);
	if (finished) {
		a = b;
	}

	tft_setbitmapcolor(BLACK, otm.color.background);
	tft_drawbitmap(otm.elapsed_time_x,  otm.elapsed_time_y-4, 0, icon24x32_elapsedtime);

	get_elapsed_time(tmp);
	tft_printf(otm.elapsed_time_x+25, otm.elapsed_time_y, 0, BLACK, otm.color.background, stats.ets, "%s", tmp);

	tft_drawbitmap(otm.timeleft_x,  otm.timeleft_y-4, 0, icon24x32_timeleft);

	get_timeleft(tmp, a, b);
	tft_printf(otm.timeleft_x+25, otm.timeleft_y, 0, BLACK, otm.color.background, stats.tls, "%s", tmp);

	// progress
	#define PROGRESS_W 300
	#define PROGRESS_H 8
	if (b && a < b) {
		int w = a*PROGRESS_W/b;
		tft_fillrect(otm.progress_x-2, otm.progress_y-2, w, PROGRESS_H, LIGHT_GREEN);
		tft_fillrect(otm.progress_x-2+w, otm.progress_y-2, PROGRESS_W-w, PROGRESS_H, LIGHT_GRAY);
		p = a*100/b;
	} else {
		tft_fillrect(otm.progress_x-2, otm.progress_y-2, PROGRESS_W, PROGRESS_H, LIGHT_GREEN);
		p = 100;
	}

	sprintf(tmp, "%d%%", p);
	int tw = tft_stringwidth(tmp);
	tft_printf(otm.progress_x+(300-tw)/2, otm.progress_y+10, 0, BLUE, otm.color.background, NULL, "%s", tmp); 

	#ifdef EMULATION
	void otm_state_change(int state);
	if (a >= b) otm_state_change(OTM_STATE_PRINTED);
	#endif

	#endif
	#endif
}

void update_filename(void)
{
	#ifdef UI_V2
	int x = 25, w = tft_stringwidth(otm.longFilename);
	if (w < 270) x = 25+(270-w);
	tft_printf(x, 50, 0, otm.color.text, otm.color.background, stats.file, otm.longFilename);
	#else
	tft_printf(STATUS_X, STATUS_Y+125, 0, BLACK, otm.color.background,  stats.file, "%s", otm.longFilename);
	#endif
}

void update_limit_switch_status(void)
{
	char xt, yt, zt;
	get_endstop_status(&xt, &yt, &zt);

	if (stats.xt != xt) {
		tft_fillrect(10+105*0, 10+25, 105, 25, otm.color.background);
		tft_printf(10+105*0, 10+25, 0, RED,   otm.color.background, NULL, "%s", xt ? "Triggrd" : "Open");
		stats.xt = xt;
	}
	if (stats.yt != yt) {
		tft_fillrect(10+105*1, 10+25, 105, 25, otm.color.background);
		tft_printf(10+105*1, 10+25, 0, GREEN, otm.color.background, NULL, "%s", yt ? "Triggrd" : "Open");
		stats.yt = yt;
	}
	if (stats.zt != zt) {
		tft_fillrect(10+105*2, 10+25, 105, 25, otm.color.background);
		tft_printf(10+105*2, 10+25, 0, BLUE,  otm.color.background, NULL, "%s", zt ? "Triggrd" : "Open");
		stats.zt = zt;
	}
}

void update_all_every_1sec(bool force)
{
	static long omillis = -1;
	long mil = oc_millis();
	if (mil < omillis) omillis = mil;
	if (!force && mil < omillis + 500) return;
	omillis = mil;
	/*	
	char tmp[32];
	LOG("update_all_every_1sec(state=%s)", otm_state_string(tmp, otm.state));
	*/
	#ifdef SHOW_COUNTERS
	show_counters();
	#endif

	switch (otm.state) {
	case OTM_STATE_MAIN:
		otm.temp_y   = 45;
		update_temp(25, 100);
		break;

	case OTM_STATE_PRINTED:
		update_filename();
		otm.temp_y   = 45;
		update_temp(25, 100);

		//
		// DO NOT UPDATE PERIODICALLY, JUST ONE TIME AT INIT(otm_printed_init)
		//
		if (force) update_progress(false/*forced*/, true/*finished*/);
		break;

	case OTM_STATE_PRINTING:
		update_speedrate();

	case OTM_STATE_PAUSED:
		update_filename();
		//update_coords();
		otm.temp_y   = 45;
		update_temp(25, 100);

		update_progress(false/*forced*/, false/*finished*/);
		break;

	case OTM_STATE_PREPARE:
		otm.temp_y   = 45;
		update_temp(25, 100);
		break;
		
	case OTM_STATE_HIDDENMENU:
		otm.temp_y   = 45;
		update_temp(15, 20);
		update_coords(15, 80);
		break;

	case OTM_STATE_JOG:
		otm.coords_y = 8;
		update_coords(25, 70);
		break;

	case OTM_STATE_TESTMENU:
		otm.coords_y = 10;
		update_coords(25, 70);
		update_limit_switch_status();
		otm.temp_y   = 45+20;
		update_temp(25, 100);
		break;

	default:
		return;
	}
}



#define _DRAW_ICONS_________

#ifndef UI_V2
static void _combine_icon_6080(OCT_UI_t *pb, const uint8_t *picon, const uint8_t *ptext)
{
	tft_drawbitmap(pb->x+5, pb->y+2, 0, icon_circle);
	tft_drawbitmap(pb->x+5+5, pb->y+2+5, OC_TRANSPARENT_BACK, picon);
	tft_drawbitmap(pb->x+10, pb->y+55, 0, ptext);
}

static void _combine_icon_5050(OCT_UI_t *pb, const uint8_t *p)
{
	tft_drawbitmap(pb->x, pb->y, 0, icon_circle);
	tft_drawbitmap(pb->x+5, pb->y+5, OC_TRANSPARENT_BACK, p);
}
#endif

static void _draw_icon(OCT_UI_t *pb)
{
	const uint8_t *p = (const uint8_t *)pb->icon;
	if (pb->flags & OCT_BF_ALIGNICONCENTER) {
		int16_t x = tft_bitmap_x(p);
		int16_t y = tft_bitmap_y(p);
		int16_t iw = tft_bitmap_width(p);
		int16_t ih = tft_bitmap_height(p);

		tft_drawbitmap(pb->x+(pb->w-iw)/2-x, pb->y+(pb->h-ih)/2-y, 0, p);
	} else
		tft_drawbitmap(pb->x, pb->y, 0, p);
}


void OCT_UIO_DrawIcon(OCT_UI_t *pb)
{
	int id = (int)pb->icon;
	switch (id) {
	case 0: break;
	#ifndef UI_V2
	case ICON_COMBI_BACK_ID:     _combine_icon_6080(pb, icon_back,     text_back); break;
	case ICON_COMBI_PREHEAT_ID:  _combine_icon_6080(pb, icon_preheat,  text_preheat); break;
	case ICON_COMBI_COOLDOWN_ID: _combine_icon_6080(pb, icon_cool,     text_cool); break;
	case ICON_COMBI_EXTRACT_ID:  _combine_icon_6080(pb, icon_extract,  text_extrude); break;
	case ICON_COMBI_RETRACT_ID:  _combine_icon_6080(pb, icon_retract,  text_retract); break;
	case ICON_COMBI_PAUSE_ID:    _combine_icon_6080(pb, icon_pause,    text_pause); break;
	case ICON_COMBI_SETTINGS_ID: _combine_icon_6080(pb, icon_settings, text_settings); break;
	case ICON_COMBI_STOP_ID:     _combine_icon_6080(pb, icon_stop,     text_stop); break;
	case ICON_COMBI_CONTINUE_ID: _combine_icon_6080(pb, icon_start,    text_start); break;
	case ICON_COMBI_JOG_ID:      _combine_icon_6080(pb, icon_jog,      text_jog); break;
	case ICON_COMBI_PREPARE_ID:  _combine_icon_6080(pb, icon_prepare,  text_prepare); break;
	case ICON_COMBI_INFO_ID:     _combine_icon_6080(pb, icon_info,     text_info); break;

	case ICON_COMBI5050_LEFT_ID:  _combine_icon_5050(pb, icon_left); break;
	case ICON_COMBI5050_RIGHT_ID: _combine_icon_5050(pb, icon_right); break;
	case ICON_COMBI5050_UP_ID:    _combine_icon_5050(pb, icon_up); break;
	case ICON_COMBI5050_DOWN_ID:  _combine_icon_5050(pb, icon_down); break;
	case ICON_COMBI5050_HOME_ID:  _combine_icon_5050(pb, icon_home); break;
	case ICON_COMBI5050_G29_ID:   _combine_icon_5050(pb, icon_g29); break;
	case ICON_COMBI5050_NPDM_ID:  _combine_icon_5050(pb, icon_npdm); break;
	case ICON_COMBI5050_MOFF_ID:  _combine_icon_5050(pb, icon_motoroff); break;
	#endif
	default:
		_draw_icon(pb);
		break;
	}
}








#define set_sched_state(state) set_sched_state0(__FILE__, __LINE__, state)

void set_sched_state0(const char *file, int line, int state)
{
	otm.sched_state = state;
	LOG("[%s:%d]sched_state=%d", file, line, state);
}

int get_sched_state(void)
{
	return otm.sched_state;
}

void oc_logo(void)
{
	tft_off();

	#ifdef UI_V2
	tft_fillscreen(BLACK);
	tft_setbitmapcolor(WHITE, BLACK);
	tft_drawbitmap(37, 108,  0, oc_logotext246x24);
	#else
	tft_fillscreen(WHITE);
	tft_setbitmapcolor(WHITE, BLACK);
	#ifdef USE_OCLOGO
	tft_drawbitmap(160-40, 120-40-20, 0, bitmap_logo0);
	tft_drawbitmap(160-80, 120-8+60,  0, bitmap_logo1);
	#endif
	#endif
	
	tft_on();
}

#ifndef UI_V2
void otm_background0(void)
{
	tft_fillrect(0, 240-COPYRIGHT_H, 320, COPYRIGHT_H, BLACK);
	tft_setbitmapcolor(WHITE, BLACK);
	tft_drawbitmap(320-174-10-10, 240-COPYRIGHT_H+1, 0, bitmap_copyrights);
}
#endif

void otm_background(void)
{
	#if 0
	tft_fillrect(0, 0, 320, 240-COPYRIGHT_H, otm.color.background);
	#else
	tft_fillrect(0, 0, 320, 240, otm.color.background);
	#endif
}

void state_push(int state)
{
	if (otm.state_sp < STATE_STACK_LEN) {
		otm.state_stack[otm.state_sp++] = state;
	}
}

int state_pop(void)
{
	if (otm.state_sp == 0) return OTM_STATE_MAIN;
	return otm.state_stack[--otm.state_sp];
}

#define ________________________1
#define _MAIN___________________
#define ________________________2
void otm_main_init(void)
{
	#ifdef UI_V2
	otm_background();
	tft_printf(25, 25, 0, otm.color.title, otm.color.background, NULL, "Mannequin");
	tft_printf(25, 49, 0, otm.color.title, otm.color.background, NULL, "Ready!");

	update_init();
	OCT_UI_Init();

	OCT_ButtonCreate(166,  25, 129, 90, "Print", OCT_BF_ALIGNICONCENTER, CMDID_FILEBROWSE, icon48x70_print, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(166, 125, 129, 90, "Setup", OCT_BF_ALIGNICONCENTER, CMDID_SETTINGS,   icon48x70_settings, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#else
	otm_background();
	
	tft_printf(STATUS_X, STATUS_Y, 0, DARK_GREEN, otm.color.background, NULL, "%s Ready!", OC_MACHINENAME);
	//tft_printf(STATUS_X, STATUS_Y+15, 0, DARK_GREEN, otm.color.background, NULL, "%s", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
//	tft_printf(STATUS_X, STATUS_Y+15, 0, DARK_GREEN, otm.color.background, NULL, "%s", "abcdefghijklmnopqrstuvwxyz");
	//tft_printf(STATUS_X, STATUS_Y+15, 0, DARK_GREEN, otm.color.background, NULL, "%s", "Hi, hi, mike");

	update_init();

	chardraw_M(0, 0);
	
	OCT_UI_Init();

	OCT_ButtonCreate(10+155*0, 80, 142, 130, "Print", OCT_BF_ALIGNICONCENTER, CMDID_FILEBROWSE, menutext_print, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+155*1-6, 80, 142, 130, "Setup", OCT_BF_ALIGNICONCENTER, CMDID_SETTINGS,   menutext_settings, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#endif
}

typedef struct {
	int count, base, pos;
} filelist_t;
filelist_t fl;

void filebrowser_getline(char *buf, int bufsize, int pos, int index, bool *pis_dir)
{
	char gcode_file[32], long_file[64];
	int time, weight;

	if (index == 0)
		strncpy_P(buf, PSTR("Back to Menu"), bufsize);
	else {
		sd_getfileinfo(index-1, gcode_file, long_file, pis_dir, &time, &weight);
		snprintf_P(buf, bufsize, PSTR("%03d %s"), index, long_file);
	}
}

static void filebrowser_draw(void)
{
#define FL_X 14
#define FL_Y  2
#define FL_NY 8
#define FL_W (320-FL_X*2)
#define FL_H 29
	int i;
	bool is_dir;

	OCT_UI_Init();
	for (i=0; i<FL_NY; i++) {
		char buf[25];
		int cmdid, flags, prm;
		uint8_t *picon = NULL;

		if (fl.base+i >= fl.count) {
			tft_fillrect(FL_X-1, FL_Y+FL_H*i-1, FL_W+2, FL_H+3, otm.color.background);
			continue;
		}

		if (fl.base+i == 0) {
			if (otm.subdir_depth == 0) {
				strcpy_P(buf, PSTR("Back to Menu"));
				#ifdef UI_V2
				picon = (uint8_t *)text100x21_to_mainmenu;
				#else
				picon = (uint8_t *)text_to_mainmenu;
				#endif
				cmdid = CMDID_BACK;
				prm = 0;
			} else {
				strcpy_P(buf, PSTR("Previous Folder"));
				cmdid = CMDID_UPDIR;
				prm = 0;
			}
		} else {
			filebrowser_getline(buf, sizeof(buf), i, fl.base+i, &is_dir);
			if (is_dir)
				cmdid = CMDID_CHDIR;
			else
				cmdid = CMDID_PRINT_SDFILE;
			prm = fl.base+i-1;
		}

		// flags
		flags = 0;
		if (fl.base+i == 0) flags |= OTM_UIOF_PREV_END;
		else if (i == 0) flags |= OTM_UIOF_PREV_CMD;
		
		if (fl.base+i == fl.count-1) flags |= OTM_UIOF_NEXT_END;
		else if (i == FL_NY-1) flags |= OTM_UIOF_NEXT_CMD;

		flags |= OCT_BF_ALIGNLEFT;
		
		int bid = OCT_ButtonCreateVAR(FL_X, FL_Y+FL_H*i, FL_W, FL_H+2, buf, flags, cmdid, picon, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
		OCT_UIO_SetParam(bid, prm);
		LOG("i=%d, flags=0x%04x", i, flags);
	}
	OCT_UI_Draw();
}

static void filebrowser_prevpage(void)
{
	if (fl.base == 0) return;
	if (fl.base >= FL_NY) fl.base -= FL_NY;
	filebrowser_draw();
}

static void filebrowser_nextpage(void)
{
	if (fl.base+FL_NY >= fl.count) return;
	fl.base += FL_NY;
	filebrowser_draw();
}

static void filebrowser_init(void)
{
	fl.count = sd_getcount()+1;
	#if 0
	if (fl.count == 1) {
		LOGP("======== sd_init ===========");
		sd_init();
		fl.count = sd_getcount()+1;
	}
	#endif
	fl.base = 0;
	fl.pos = 0;

	filebrowser_draw();
}

void sd_printstart(const char* filename, char* longFilename);
void otm_state_push(int state);

static void change_dir(int index)
{
	int time, weight;
	bool is_dir;

	sd_getfileinfo(index, otm.filename, otm.longFilename, &is_dir, &time, &weight);
	
	sd_chdir(otm.filename);
	otm.subdir_depth++;
	filebrowser_init();
}

static void up_dir(void)
{
	sd_updir();
	otm.subdir_depth--;
	filebrowser_init();
}

static void print_current_uio_file(int index)
{
	int time, weight;
	bool is_dir;

	sd_getfileinfo(index, otm.filename, otm.longFilename, &is_dir, &time, &weight);
//	LOG("Print start: file='%s'", otm.longFilename);
	otm_state_push(OTM_STATE_PRINTING);

	sd_printstart(otm.filename, otm.longFilename);
}

void otm_filebrowser_init(void)
{
	otm_background();
	filebrowser_init();
}

void oc_knob_block_extrude(void *fp_finish);
void oc_knob_block_retract(void *fp_finish);


void otm_prepare_init(void)
{
	#ifdef UI_V2
	otm_background();

	tft_printf(25, 25, 0, otm.color.title, otm.color.background, NULL, "Prepare");

	update_init();

	OCT_UI_Init();

	OCT_ButtonCreate(25+55*0, 165, 50, 50, "Back",     OCT_BF_ALIGNICONCENTER, CMDID_BACK,     icon46x46_back, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*1, 165, 50, 50, "Preheat",  OCT_BF_ALIGNICONCENTER, CMDID_PREHEAT,  icon46x46_preheat, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*2, 165, 50, 50, "Cooldown", OCT_BF_ALIGNICONCENTER, CMDID_COOLDOWN, icon46x46_cooldown, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	int btn_id = OCT_ButtonCreate(25+55*3, 165, 50, 50, "Extrude",  OCT_BF_ALIGNICONCENTER, CMDID_EXTRUDE,  icon46x46_extrude, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_extrude);
	btn_id = OCT_ButtonCreate(25+55*4, 165, 50, 50, "Retract",  OCT_BF_ALIGNICONCENTER, CMDID_RETRACT,  icon46x46_retract, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_retract);

	OCT_UI_Draw();
	#else
	otm_background();
	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, otm.color.background, NULL, "Prepare");

	update_init();

	OCT_UI_Init();

	OCT_ButtonCreate(7+62*0, 240-COPYRIGHT_H-85, 60, 80, "Back",     0, CMDID_BACK,     ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(7+62*1, 240-COPYRIGHT_H-85, 60, 80, "Preheat",  0, CMDID_PREHEAT,  ICON_COMBI_PREHEAT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(7+62*2, 240-COPYRIGHT_H-85, 60, 80, "Cooldown", 0, CMDID_COOLDOWN, ICON_COMBI_COOLDOWN, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	
	int btn_id = OCT_ButtonCreate(7+62*3, 240-COPYRIGHT_H-85, 60, 80, "Extrude",  0, CMDID_EXTRUDE,  ICON_COMBI_EXTRACT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_extrude);
	btn_id = OCT_ButtonCreate(7+62*4, 240-COPYRIGHT_H-85, 60, 80, "Retract",  0, CMDID_RETRACT,  ICON_COMBI_RETRACT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_retract);

	OCT_UI_Draw();
	#endif
}

void otm_printing_init(void)
{
	#ifdef UI_V2
	otm_background();

	tft_printf(25, 25, 0, otm.color.title, otm.color.background,  NULL, "Printing");

	update_init();
	update_filename();

	OCT_UI_Init();
	OCT_ButtonCreate(25, 165, 50, 50, "Pause", OCT_BF_ALIGNICONCENTER, CMDID_PAUSE, icon46x46_pause, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UI_Draw();

	#else
	otm_background();

	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, otm.color.background,  NULL, "Printing");

	update_init();

	OCT_UI_Init();

	/*
	OCT_ButtonCreate(10+65*0, 240-COPYRIGHT_H-85, 60, 80, "Pause",    0, CMDID_PAUSE,     ICON_COMBI_PAUSE, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+65*1, 240-COPYRIGHT_H-85, 60, 80, "Stop",     0, CMDID_STOP,      ICON_COMBI_STOP, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+65*2, 240-COPYRIGHT_H-85, 60, 80, "Settings", 0, CMDID_PSETTINGS, ICON_COMBI_SETTINGS, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	*/
	OCT_ButtonCreate((320-90)/2, 240-COPYRIGHT_H-35-10, 90, 35, "Pause", 0, CMDID_PAUSE,     button_pause, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	
	OCT_UI_Draw();
	#endif
}

void otm_paused_init(void)
{
	#ifdef UI_V2
	otm_background();
	tft_printf(25, 25, 0, otm.color.title, otm.color.background, NULL, "Paused");

	update_init();

	OCT_UI_Init();
	OCT_ButtonCreate(25+60*0, 165, 50, 50, "Resume", OCT_BF_ALIGNICONCENTER, CMDID_RESUME, icon46x46_start, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+60*1, 165, 50, 50, "Stop",   OCT_BF_ALIGNICONCENTER, CMDID_STOP,   icon46x46_stop, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+60*2, 165, 50, 50, "Jog",    OCT_BF_ALIGNICONCENTER, CMDID_JOG,    icon46x46_jog, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UI_Draw();
	#else
	otm_background();

	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, otm.color.background, NULL, "Paused");

	update_init();

	OCT_UI_Init();

	OCT_ButtonCreate(10+65*0, 240-COPYRIGHT_H-85, 60, 80, "Resume", 0, CMDID_RESUME, ICON_COMBI_CONTINUE, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+65*1, 240-COPYRIGHT_H-85, 60, 80, "Stop",   0, CMDID_STOP,   ICON_COMBI_STOP, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+65*2, 240-COPYRIGHT_H-85, 60, 80, "Jog",    0, CMDID_JOG,    ICON_COMBI_JOG, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	
	OCT_UI_Draw();
	#endif
}

void otm_printed_init(void)
{
	#ifdef UI_V2
	otm_background();
	tft_printf(25, 25, 0, otm.color.title, otm.color.background, NULL, "Printed");

	update_init();

	OCT_UI_Init();
	OCT_ButtonCreate(25+55*0, 165, 50, 50, "Back",     OCT_BF_ALIGNICONCENTER, CMDID_BACK,     icon46x46_back, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UI_Draw();
	
	#else
	otm_background();

	tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, otm.color.background,  NULL, "Printed");

	update_init();

	OCT_UI_Init();

	OCT_ButtonCreate(10+65*0, 240-COPYRIGHT_H-85, 60, 80, "Main", 0, CMDID_BACK, ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	
	OCT_UI_Draw();
	#endif
}


int otm_preheat_temp = 185, otm_printing_speed = 100, otm_fan_speed = 100;
float otm_zoffset = -18.20;

float fw_getzoffset(void);
void fw_setzoffset(float zoff);

void otm_settings_init(void)
{
	#ifdef UI_V2
	otm_background();

	OCT_UI_Init();

	OCT_ButtonCreate(25+55*0, 165, 50, 50, "Back",    OCT_BF_ALIGNICONCENTER, CMDID_BACK,    icon46x46_back, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*1, 165, 50, 50, "Prepare", OCT_BF_ALIGNICONCENTER, CMDID_PREPARE, icon46x46_prepare, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*2, 165, 50, 50, "Jog",     OCT_BF_ALIGNICONCENTER, CMDID_JOG,     icon46x46_jog, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*3, 165, 50, 50, "NPDM",    OCT_BF_ALIGNICONCENTER, CMDID_NPDM,    icon46x46_npdm, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*4, 165, 50, 50, "Info",    OCT_BF_ALIGNICONCENTER, CMDID_INFO,    icon46x46_info, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	
	char tt[32], tf[32];
	otm_zoffset = fw_getzoffset();

	sprintf_P(tt, PSTR(": %d `C"), otm_preheat_temp);
	OCT_ButtonCreateVAR(25, 25+30*0, 270, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+0, text78x21_nozzle_temp, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	sprintf_P(tt, PSTR(": %d %%"), otm_printing_speed);
	OCT_ButtonCreateVAR(25, 25+30*1, 270, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+1, text78x21_print_speed, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	sprintf_P(tt, PSTR(": %d %%"), otm_fan_speed);
	OCT_ButtonCreateVAR(25, 25+30*2, 270, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+2, text78x21_fan_speed, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	sprintf_P(tt, PSTR(": %s"), oc_ftostr31ns(otm_zoffset, tf));
	OCT_ButtonCreateVAR(25, 25+30*3, 270, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+3, text78x21_nozzle_offset, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();

	#else
	otm_background();

	//tft_printf(STATUS_X, STATUS_Y+YG*0, 0, DARK_GREEN, otm.color.background, NULL, "%s Ready", OC_MACHINENAME);

	OCT_UI_Init();

	OCT_ButtonCreate(10+65*0, 240-COPYRIGHT_H-85, 60, 80, "Back",    0, CMDID_BACK,    ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+65*1, 240-COPYRIGHT_H-85, 60, 80, "Prepare", 0, CMDID_PREPARE, ICON_COMBI_PREPARE, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+65*2, 240-COPYRIGHT_H-85, 60, 80, "Jog",     0, CMDID_JOG,     ICON_COMBI_JOG, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+65*3, 240-COPYRIGHT_H-85, 60, 80, "Info",    0, CMDID_INFO,    ICON_COMBI_INFO, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	int x = 10, y = 10;
	char tt[32], tf[16];

	otm_zoffset = fw_getzoffset();
	

//	tft_drawbitmap(10, y+0*30+5, 80, 20, 0, );
	sprintf_P(tt, PSTR("        : %d `C"), otm_preheat_temp);
	OCT_ButtonCreateVAR(x, y+0*30, 300, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+0, text_nozzle_temp, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	sprintf_P(tt, PSTR("        : %d %%"), otm_printing_speed);
	OCT_ButtonCreateVAR(x, y+1*30, 300, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+1, text_speed, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	sprintf_P(tt, PSTR("        : %d %%"), otm_fan_speed);
	OCT_ButtonCreateVAR(x, y+2*30, 300, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+2, text_fan_speed, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	sprintf_P(tt, PSTR("        : %s"), oc_ftostr31ns(otm_zoffset, tf));
	OCT_ButtonCreateVAR(x, y+3*30, 300, 30, tt, OCT_BF_SPECIAL_BUTTON1, CMDID_CFGLINE_EEPROM+3, text_nozzle_offset, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#endif
}

int prt_nozzle_temp = 185, prt_printing_speed = 100, prt_fan_speed = 100;
void otm_psettings_init(void)
{
	otm_background();

	//tft_printf(STATUS_X, STATUS_Y+YG*0, 0, DARK_GREEN, otm.color.background, NULL, "%s Ready", OC_MACHINENAME);

	OCT_UI_Init();

	OCT_ButtonCreate(10+65*0, 240-COPYRIGHT_H-85, 60, 80, "Back",    0, CMDID_BACK,    ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	int x = 10, y = 40;
	char tt[32];
	sprintf_P(tt, PSTR("Nozzle: %d `C"), prt_nozzle_temp);
	OCT_ButtonCreateVAR(x, y+0*30, 300, 30, tt, 0, CMDID_CFGLINE_PRINTING+0, NULL, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	sprintf_P(tt, PSTR("Speed: %d %%"), prt_printing_speed);
	OCT_ButtonCreateVAR(x, y+1*30, 300, 30, tt, 0, CMDID_CFGLINE_PRINTING+1, NULL, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	sprintf_P(tt, PSTR("FAN Speed: %d %%"), prt_fan_speed);
	OCT_ButtonCreateVAR(x, y+2*30, 300, 30, tt, 0, CMDID_CFGLINE_PRINTING+2, NULL, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
}

void oc_knob_block_left(void *fp_finish, uint32_t step_max);
void oc_knob_block_right(void *fp_finish, uint32_t step_max);
void oc_knob_block_rear(void *fp_finish, uint32_t step_max);
void oc_knob_block_front(void *fp_finish, uint32_t step_max);
void oc_knob_block_up(void *fp_finish, uint32_t step_max);
void oc_knob_block_down(void *fp_finish, uint32_t step_max);

#ifdef UI_V2
void draw_jogbutton_icon(OCT_UI_t *pb, uint16_t fg_color, uint16_t bg_color, bool do_not_change_fg)
{
	if (!do_not_change_fg) {
		switch (pb->cmdid) {
		case CMDID_LEFT:  case CMDID_RIGHT:
			//if (otm.color.bg_is_dark) fg_color = LIGHT_RED;   else fg_color = DARK_RED; 
			if (bg_color == BLACK) fg_color = RED; else fg_color = DARK_RED;
			//if (pb->cmdid == CMDID_LEFT) fg_color = LIGHT_RED; else fg_color = DARK_RED; 
			break;
		case CMDID_FRONT: case CMDID_REAR:
			//if (otm.color.bg_is_dark) fg_color = LIGHT_GREEN; else fg_color = DARK_GREEN; 
			if (bg_color == BLACK) fg_color = GREEN; else fg_color = DARK_GREEN;
			break;
		case CMDID_UP:    case CMDID_DOWN:
			//if (otm.color.bg_is_dark) fg_color = LIGHT_BLUE;  else fg_color = DARK_BLUE; 
			if (bg_color == BLACK) fg_color = BLUE; else fg_color = DARK_BLUE;
			break;
		}
	}
	tft_setbitmapcolor(fg_color, bg_color);
	switch (pb->cmdid) {
	case CMDID_LEFT:  tft_drawbitmap(pb->x+9, pb->y+34, 0, icon12x16_jogleft); break;
	case CMDID_RIGHT: tft_drawbitmap(pb->x+9, pb->y+30, 0, icon12x16_jogright); break;
	case CMDID_FRONT: tft_drawbitmap(pb->x+39, pb->y+8, 0, icon16x11_jogfront); break;
	case CMDID_REAR:  tft_drawbitmap(pb->x+32, pb->y+10, 0, icon16x11_jogrear); break;
	case CMDID_UP:    tft_drawbitmap(pb->x+22, pb->y+19, 0, icon16x18_jogup); break;
	case CMDID_DOWN:  tft_drawbitmap(pb->x+21, pb->y+18, 0, icon16x18_jogdown); break;
	}
}
#endif

void otm_jog_init(void)
{
	#ifdef UI_V2
	otm_background();

	tft_printf(25, 25, 0, otm.color.title, otm.color.background, NULL, "Jog");

	update_init();

	OCT_UI_Init();

	OCT_ButtonCreate(25+55*0, 165, 50, 50, "Back",     OCT_BF_ALIGNICONCENTER, CMDID_BACK,      icon46x46_back, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*1, 165, 50, 50, "Home",     OCT_BF_ALIGNICONCENTER, CMDID_HOME,      icon46x46_home, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	// special buttons
	int jx=144, jy=25;
	int btn_id;

	btn_id = OCT_ButtonCreate(jx+30, jy+30, 30, 80, "Right",  0, CMDID_RIGHT,  JOG_BUTTON_RIGHT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_right);
	btn_id = OCT_ButtonCreate(jx, jy+20, 30, 80, "Left",  0, CMDID_LEFT,  JOG_BUTTON_LEFT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_left);

	btn_id = OCT_ButtonCreate(jx, jy+10, 90, 30, "Front",  0, CMDID_FRONT,  JOG_BUTTON_FRONT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_front);
	btn_id = OCT_ButtonCreate(jx+30, jy, 90, 30, "Rear",  0, CMDID_REAR,  JOG_BUTTON_REAR, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_rear);

	btn_id = OCT_ButtonCreate(jx+60, jy+20, 60, 55, "Up",  0, CMDID_UP,  JOG_BUTTON_UP, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_up);
	btn_id = OCT_ButtonCreate(jx+60, jy+55, 60, 55, "Down",  0, CMDID_DOWN,  JOG_BUTTON_DOWN, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_down);

	tft_setbitmapcolor(otm.color.text, otm.color.background);
	tft_drawbitmap(jx+6, jy+105, 0, icon22x16_frontmark);

	//
	OCT_ButtonCreate(25+55*3, 165, 50, 50, "G29",      OCT_BF_ALIGNICONCENTER, CMDID_G29,       icon46x46_g29, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(25+55*4, 165, 50, 50, "MOFF",     OCT_BF_ALIGNICONCENTER, CMDID_MOTOROFF,  icon46x46_motoroff, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();

	#else
	otm_background();

	//tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, otm.color.background, NULL, "Jog");

	update_init();

	OCT_UI_Init();

	int w = 50, h = 50, g = 1, jx = 25, jy = 25+8;

	OCT_ButtonCreate(10+65*0, 240-COPYRIGHT_H-85, 60, 80, "Back",    0, CMDID_BACK,    ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	int btn_id;
	OCT_ButtonCreate(jx+(w+g)*1, jy+(h+g)*1, w, h, "Home",  0, CMDID_HOME,  ICON_COMBI5050_HOME, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	btn_id = OCT_ButtonCreate(jx+(w+g)*1, jy+(h+g)*0, w, h, "Rear",  0, CMDID_REAR,  ICON_COMBI5050_UP, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_rear);

	btn_id = OCT_ButtonCreate(jx+(w+g)*1, jy+(h+g)*2, w, h, "Front", 0, CMDID_FRONT, ICON_COMBI5050_DOWN, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_front);

	btn_id = OCT_ButtonCreate(jx+(w+g)*0, jy+(h+g)*1, w, h, "Left",  0, CMDID_LEFT,  ICON_COMBI5050_LEFT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id,  (void *)oc_knob_block_left);

	btn_id = OCT_ButtonCreate(jx+(w+g)*2, jy+(h+g)*1, w, h, "Right", 0, CMDID_RIGHT, ICON_COMBI5050_RIGHT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_right);

	int jzx = 190, jzy = jy;
	btn_id = OCT_ButtonCreate(jzx, jzy,    w, h, "Up",   0, CMDID_UP,   ICON_COMBI5050_UP, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_up);
	btn_id = OCT_ButtonCreate(jzx, jzy+(h+g)*1, w, h, "Down", 0, CMDID_DOWN, ICON_COMBI5050_DOWN, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_down);

	int sx = 260, sy = jy;

	OCT_ButtonCreate(sx, sy+(h+g)*1*0, w, h, "G29",  0, CMDID_G29,      ICON_COMBI5050_G29, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	//OCT_ButtonCreate(sx, sy+55*1, w, h, "NPDM", 0, CMDID_NPDM,     ICON_COMBI5050_NPDM, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(sx, sy+(h+g)*1*1, w, h, "MOFF", 0, CMDID_MOTOROFF, ICON_COMBI5050_MOFF, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#endif
}

bool get_dryrun_mode(void);
void otm_info_init(void)
{
	#ifdef UI_V2
	otm_background();

	#ifdef USE_OCLOGO
	if (otm.color.bg_is_dark) tft_setbitmapcolor(otm.color.background, WHITE);
	else tft_setbitmapcolor(otm.color.background, BLACK);
	tft_drawbitmap(20, 30, 0, bitmap_logo0);
	//tft_drawbitmap(160-80, 120-8+60-50,  0, bitmap_logo1);
	#endif
	#ifdef FOR_MANUAL
	tft_printf(110, 30+25*0, 0, otm.color.title, otm.color.background, NULL, "Mannequin");
	#else
	tft_printf(110, 30+25*0, 0, otm.color.title, otm.color.background, NULL, OC_MACHINENAME);
	#endif
	tft_printf(110, 30+25*1+10, 0, otm.color.info, otm.color.background, NULL, "Firmware");
	tft_printf(110, 30+25*2+10, 0, otm.color.info, otm.color.background, NULL, "ver. " STRING_VERSION);

	if (get_dryrun_mode()) {
		tft_printf(110, 30+25*3+30, 0, otm.color.info, otm.color.background, NULL, "DRY-RUN");	
	}

	update_init();

	OCT_UI_Init();

	//OCT_ButtonCreate(10+62*0, 240-COPYRIGHT_H-85, 60, 80, "Back",     0, CMDID_BACK,     ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate((320-80)/2, 240-COPYRIGHT_H-35-10, 80, 35, "Back",     0, CMDID_BACK,     button_ok, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();

	#else
	otm_background();

	//tft_printf(STATUS_X, STATUS_Y, 0, DARK_YELLOW, otm.color.background, NULL, "Info");

	#ifdef USE_OCLOGO
	tft_setbitmapcolor(WHITE, BLACK);
	tft_drawbitmap(20, 30, 0, bitmap_logo0);
	//tft_drawbitmap(160-80, 120-8+60-50,  0, bitmap_logo1);
	#endif
	tft_printf(110, 30+25*0, 0, BLACK, otm.color.background, NULL, OC_MACHINENAME);
	tft_printf(110, 30+25*1+10, 0, OCE_COLOR_TO_RGB565(0xff606060), otm.color.background, NULL, "Firmware");
	tft_printf(110, 30+25*2+10, 0, OCE_COLOR_TO_RGB565(0xff606060), otm.color.background, NULL, "ver. " STRING_VERSION);

	if (get_dryrun_mode()) {
		tft_printf(110, 30+25*3+30, 0, OCE_COLOR_TO_RGB565(0xff606060), otm.color.background, NULL, "DRY-RUN");	
	}

	update_init();

	OCT_UI_Init();

	//OCT_ButtonCreate(10+62*0, 240-COPYRIGHT_H-85, 60, 80, "Back",     0, CMDID_BACK,     ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate((320-80)/2, 240-COPYRIGHT_H-35-10, 80, 35, "Back",     0, CMDID_BACK,     button_ok, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#endif
}

void otm_hiddenmenu_init(void)
{
	#ifdef UI_V2
	int btn_id;

	otm_background();

	OCT_UI_Init();

	#define ISZ 46
	int by = 175;

	OCT_ButtonCreate(15+(ISZ+2)*0, by, ISZ, ISZ, "Back",     0, CMDID_BACK,     icon46x46_back, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(15+(ISZ+2)*1, by, ISZ, ISZ, "Preheat",  0, CMDID_PREHEAT,  icon46x46_preheat, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(15+(ISZ+2)*2, by, ISZ, ISZ, "Cooldown", 0, CMDID_COOLDOWN, icon46x46_cooldown, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	btn_id = OCT_ButtonCreate(15+(ISZ+2)*3, by, ISZ, ISZ, "Extract",  0, CMDID_EXTRUDE,  icon46x46_extrude, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_extrude);
	btn_id = OCT_ButtonCreate(15+(ISZ+2)*4, by, ISZ, ISZ, "Retract",  0, CMDID_RETRACT,  icon46x46_retract, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_retract);
	
	OCT_ButtonCreate(15+(ISZ+2)*5, by-48*0, ISZ, ISZ, "MOFF", 0, CMDID_MOTOROFF, icon46x46_motoroff, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(15+(ISZ+2)*5, by-48*1, ISZ, ISZ, "Home", 0, CMDID_HOME,     icon46x46_home, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(15+(ISZ+2)*5, by-48*2, ISZ, ISZ, "G29",  0, CMDID_G29,      icon46x46_g29, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(15+(ISZ+2)*5, by-48*3, ISZ, ISZ, "Info", 0, CMDID_INFO,     icon46x46_info, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);


	int jx=174-55, jy=25+15;

	btn_id = OCT_ButtonCreate(jx+30, jy+30, 30, 80, "Right",  0, CMDID_RIGHT,  JOG_BUTTON_RIGHT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_right);
	btn_id = OCT_ButtonCreate(jx, jy+20, 30, 80, "Left",  0, CMDID_LEFT,  JOG_BUTTON_LEFT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_left);

	btn_id = OCT_ButtonCreate(jx, jy+10, 90, 30, "Front",  0, CMDID_FRONT,  JOG_BUTTON_FRONT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_front);
	btn_id = OCT_ButtonCreate(jx+30, jy, 90, 30, "Rear",  0, CMDID_REAR,  JOG_BUTTON_REAR, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_rear);

	btn_id = OCT_ButtonCreate(jx+60, jy+20, 60, 55, "Up",  0, CMDID_UP,  JOG_BUTTON_UP, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_up);
	btn_id = OCT_ButtonCreate(jx+60, jy+55, 60, 55, "Down",  0, CMDID_DOWN,  JOG_BUTTON_DOWN, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_down);

	tft_setbitmapcolor(otm.color.text, otm.color.background);
	tft_drawbitmap(jx+6, jy+105, 0, icon22x16_frontmark);

	OCT_UI_Draw();
	#else
	otm_background();

	OCT_UI_Init();

	OCT_ButtonCreate(10+62*0, 240-COPYRIGHT_H-85, 60, 80, "Back",     0, CMDID_BACK,     ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+62*1, 240-COPYRIGHT_H-85, 60, 80, "Preheat",  0, CMDID_PREHEAT,  ICON_COMBI_PREHEAT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+62*2, 240-COPYRIGHT_H-85, 60, 80, "Cooldown", 0, CMDID_COOLDOWN, ICON_COMBI_COOLDOWN, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(10+62*3, 240-COPYRIGHT_H-85, 60, 80, "Jog",      0, CMDID_JOG,      ICON_COMBI_JOG, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	int btn_id = OCT_ButtonCreate(10+62*2, 240-COPYRIGHT_H-85-85, 60, 80, "Extract",  0, CMDID_EXTRUDE,  ICON_COMBI_EXTRACT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_extrude);
	btn_id = OCT_ButtonCreate(10+62*3, 240-COPYRIGHT_H-85-85, 60, 80, "Retract",  0, CMDID_RETRACT,  ICON_COMBI_RETRACT, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UIO_SetBlockFunction(btn_id, (void *)oc_knob_block_retract);
	
/*
	OCT_ButtonCreate(200, 25+55*0, 50, 50, "Extract",  0, CMDID_EXTRACT,  icon50x50_extract, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(200, 25+55*1, 50, 50, "Retract",  0, CMDID_RETRACT,  icon50x50_retract, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
*/
	//OCT_ButtonCreate(260, 25+55*0, 50, 50, "NPDM", 0, CMDID_NPDM,     ICON_COMBI5050_NPDM, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_ButtonCreate(260, 25+55*1, 50, 50, "MOFF", 0, CMDID_MOTOROFF, ICON_COMBI5050_MOFF, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#endif
}

int tft_init_fail_count = 0;

static char *chip_id_string(void)
{
	switch (tft_type) {
	case TFTTYPE_S6D0154: return "0154";
	case TFTTYPE_HX8347G: return "8347G";
	case TFTTYPE_ILI9325: return "9325";
	case TFTTYPE_C505:    return "C505";
	default: break;
	}
	return "Unknown";
}

void otm_testmenu_init(void)
{
	#ifdef UI_V2
	otm_background();

	tft_drawtextfmt(6, 240-9, 1, otm.color.title, otm.color.background, "%sIFC%d", chip_id_string(), tft_init_fail_count);
	
	update_init();

	OCT_UI_Init();

	OCT_ButtonCreate(25+55*0, 165, 50, 50, "Back",     OCT_BF_ALIGNICONCENTER, CMDID_BACK,      icon46x46_back, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#else
	otm_background();

	update_init();

	OCT_UI_Init();

	OCT_ButtonCreate(10+62*0, 240-COPYRIGHT_H-85, 60, 80, "Back",     0, CMDID_BACK,     ICON_COMBI_BACK, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);

	OCT_UI_Draw();
	#endif
}

void otm_error_init(void)
{
#ifdef USE_PROBE_SAFETY_CHECK
	uint16_t col_bg, col_cap;
	col_bg = OCE_COLOR_TO_RGB565(0xfffff0f0);
	col_cap = OCE_COLOR_TO_RGB565(0xffe00000);

	tft_fillrect(0, 0, 320, 40, col_cap);
	tft_setbitmapcolor(col_cap, WHITE);
	tft_drawbitmap((320-48)/2, (40-26)/2, 0, title_error);

	tft_fillrect(0, 40, 320, 200, col_bg);
	tft_setbitmapcolor(col_bg, BLACK);
	tft_drawbitmap((320-230)/2, 40+(200-26)/2, 0, text1);

	update_init();
	OCT_UI_Init();
	// No button
	OCT_UI_Draw();
#endif
}

void otm_error_resolved_init(void)
{
#ifdef USE_PROBE_SAFETY_CHECK
	uint16_t col_bg, col_cap;
	col_bg = OCE_COLOR_TO_RGB565(0xfffff0ff);
	col_cap = OCE_COLOR_TO_RGB565(0xff0000e0);

	tft_fillrect(0, 0, 320, 40, col_cap);
	tft_setbitmapcolor(WHITE, col_cap);
	tft_drawbitmap((320-104)/2, (40-26)/2, 0, title_continue);

	tft_fillrect(0, 40, 320, 200, col_bg);
	tft_setbitmapcolor(col_bg, BLACK);
	tft_drawbitmap((320-224)/2, 40+(150-40-26)/2, 0, text2);
	tft_drawbitmap((320-196)/2, 40+40+(150-40-26)/2, 0, text3);

	update_init();

	OCT_UI_Init();
	OCT_ButtonCreate((320-80)/2, 240-COPYRIGHT_H-35-10, 80, 35, "Back",     0, CMDID_BACK,     button_ok, otm.color.btn_fg, otm.color.btn_bg, otm.color.btn_edge);
	OCT_UI_Draw();
#endif	
}

void otm_editvalue_init(void);

void otm_state_transit(int state)
{
	switch (state) {
	case OTM_STATE_MAIN:       otm_main_init();        break;
	case OTM_STATE_FILEBROWSE: otm_filebrowser_init(); break;
	case OTM_STATE_PRINTING:   otm_printing_init();    break;
	case OTM_STATE_PRINTED:    otm_printed_init();     break;
	case OTM_STATE_PAUSED:     otm_paused_init();      break;
	case OTM_STATE_SETTINGS:   otm_settings_init();    break;
	case OTM_STATE_PSETTINGS:  otm_psettings_init();   break;
	case OTM_STATE_PREPARE:    otm_prepare_init();     break;
	case OTM_STATE_JOG:        otm_jog_init();         break;
	case OTM_STATE_INFO:       otm_info_init();        break;
	case OTM_STATE_EDITVALUE:  otm_editvalue_init();   break;
	case OTM_STATE_TESTMENU:   otm_testmenu_init();    break;
	case OTM_STATE_HIDDENMENU: otm_hiddenmenu_init();  break;
	case OTM_STATE_ERROR:          otm_error_init(); break;
	case OTM_STATE_ERROR_RESOLVED: otm_error_resolved_init(); break;
	}
}

void otm_state_change(int state)
{
	if (otm.state == state) return;
	otm.state = state;
	otm_state_transit(state);
}

void otm_state_push(int state)
{
	if (otm.state == state) return;

	char prev[16], next[16];
	LOG("#### otm_state_push #### (%s => %s)", otm_state_string(prev, otm.state), otm_state_string(next, state));
	
	state_push(otm.state);
	otm.state = state;
	otm_state_transit(state);
}

void otm_state_pop(void)
{
	//char prev[16], next[16];
	//int old_state = otm.state;
	otm.state = state_pop();
	//LOG("#### otm_state_pop ### (%s => %s)", otm_state_string(prev, old_state), otm_state_string(next, otm.state));
	otm_state_transit(otm.state);
}

void hd_init(void);
void color_test(void);

#ifdef WORKAROUND_FOR_ILI9325
bool tft_init_failed = false;
#endif

void otm_init(void)
{
	LOGP("================");
	LOGP(" otm_init");
	LOGP("================");
	memset(&otm, 0, sizeof(otm));
	otm.sched_state = OTM_STATE_NONE;
	otm.coords_x = 10; otm.coords_y = 10;
	otm.temp_x  = 5; otm.temp_y   = 50;
	otm.elapsed_time_x = 10; otm.elapsed_time_y = 100;
	otm.timeleft_x = 220; otm.timeleft_y = 100;
	otm.progress_x = 10; otm.progress_y = 80;

	otm.speed_rate = 100;
	otm.old_speed_rate = 100;
	otm.oldold_speed_rate = 100;
	otm.subdir_depth = 0;

	color_init();

	sd_init0();
	td_init();

#ifdef WORKAROUND_FOR_ILI9325
tft_reinit:
	tft_init();
	if (tft_init_failed && tft_init_fail_count<5) {
		delay(250);
		tft_init_failed = false;
		tft_init_fail_count++;
		goto tft_reinit;
	}
#else
	tft_init();
#endif
	// color_test(); while (1) ; // !!! for ili9325 gamma test
	oc_logo();

	LOG("%s Ready!", OC_MACHINENAME);

//	otm_state_push(OTM_STATE_MAIN);
	otm.sched_state = OTM_STATE_MAIN;
}

typedef struct {
	int   x, y, w, h;
	bool  is_editing, is_float, zup_on_finished;
	int   *i_ptr, i_min, i_max, i_step, i_old;
	float *f_ptr, f_min, f_max, f_step, f_old;
	char  old_text[10], units[10];
	const uint8_t *ptitle;
} ev_t;
ev_t ev;

bool ev_is_editing(void)
{
	return ev.is_editing;
}

static void _draw_value(void)
{
	char tmp[10];

	if (ev.is_float) {
		oc_ftostr31ns(*ev.f_ptr, tmp);
	} else {
		sprintf(tmp, "%d", *ev.i_ptr);
	}
	if (strcmp(tmp, ev.old_text)) {
		tft_fillrect(ev.x+10+59, ev.y+70, ev.w-20-59, 25, WHITE);
		tft_printf(ev.x+10+59, ev.y+70, 0, BLACK, WHITE, NULL, "%s", tmp);
		strcpy(ev.old_text, tmp);

		int w = tft_stringwidth(tmp);
		tft_printf(ev.x+10+59+w+12, ev.y+70, 0, BLACK, WHITE, NULL, "%s", ev.units);
	}
}

void otm_editvalue_init(void)
{
	ev.x = 40; ev.y = 60;
	ev.w = 320-ev.x*2; ev.h = 250-ev.y*2;
	tft_fillrect(ev.x, ev.y, ev.w, ev.h, WHITE);
	tft_setbitmapcolor(BLACK, WHITE);
	tft_drawbitmap(ev.x+5, ev.y+5, 0, ev.ptitle);
//	tft_printf(ev.x+100, ev.y+5, 0, BLACK, WHITE, NULL, "%s", ev.units);
	
	char tmp[10], tmp2[10];

	if (ev.is_float) {
		oc_ftostr31ns(ev.f_old, tmp);
		tft_printf(ev.x+18, ev.y+40, 0, BLACK, WHITE, NULL, "Old: %s %s", tmp, ev.units);
		tft_printf(ev.x+10, ev.y+70, 0, BLACK, WHITE, NULL, "New:");
		tft_printf(ev.x+10, ev.y+100, 0, BLACK, WHITE, NULL, "(%s ~ %s)", 
			oc_ftostr31ns(ev.f_min, tmp),
			oc_ftostr31ns(ev.f_max, tmp2));
	} else {
		sprintf(tmp, "%d", ev.i_old);
		tft_printf(ev.x+18, ev.y+40, 0, BLACK, WHITE, NULL, "Old: %s %s", tmp, ev.units);
		tft_printf(ev.x+10, ev.y+70, 0, BLACK, WHITE, NULL, "New:");
		tft_printf(ev.x+10, ev.y+100, 0, BLACK, WHITE, NULL, "(%d ~ %d)", ev.i_min, ev.i_max); 
	}
	//otm_background();
	_draw_value();
}

void otm_askvalue_init(void)
{
}

static void _EditValueFloat(float *pvalue, float min, float max, float step, const uint8_t *ptitle, char *units, bool zup)
{
	ev.is_editing = true;
	ev.is_float = true;
	ev.f_ptr = pvalue;
	ev.f_old = *pvalue;
	ev.f_min = min;
	ev.f_max = max;
	ev.f_step = step;
	ev.old_text[0] = 0;
	ev.ptitle = ptitle;
	ev.zup_on_finished = zup;
	strcpy(ev.units, units);
	OCT_UIO_SaveIndex();

	otm_state_push(OTM_STATE_EDITVALUE);
}

static void _EditValueInt(int *pvalue, int min, int max, int step, const uint8_t *ptitle, char *units)
{
	ev.is_editing = true;
	ev.is_float = false;
	ev.i_ptr = pvalue;
	ev.i_old = *pvalue;
	ev.i_min = min;
	ev.i_max = max;
	ev.i_step = step;
	ev.old_text[0] = 0;
	ev.ptitle = ptitle;
	ev.zup_on_finished = false;
	strcpy(ev.units, units);
	OCT_UIO_SaveIndex();

	otm_state_push(OTM_STATE_EDITVALUE);
}

#define OEV_EVENT_CCW    1
#define OEV_EVENT_CW     2
#define OEV_EVENT_CLICK  3

#ifdef USE_REALTIME_ZOFFSET
void oc_block_up_mm_for_zoffset(float mm);
void oc_block_down_mm_for_zoffset(float mm);
#endif
void oc_block_up_mm(float mm);

bool _EditValueEvent(int event)
{
	if (!ev.is_editing) return false;

	#ifdef USE_REALTIME_ZOFFSET
	float old_f;
	if (ev.is_float && ev.f_ptr == &otm_zoffset) old_f = *ev.f_ptr;
	#endif

	// editing events
	switch (event) {
	case OEV_EVENT_CCW:
		if (ev.is_float) {
			if (*ev.f_ptr - ev.f_step >= ev.f_min) (*ev.f_ptr) -= ev.f_step; 
			else *ev.f_ptr = ev.f_min;
		} else {
			if (*ev.i_ptr - ev.i_step >= ev.i_min) (*ev.i_ptr) -= ev.i_step; 
			else *ev.i_ptr = ev.i_min;
		}
		break;
	case OEV_EVENT_CW:
		if (ev.is_float) {
			if (*ev.f_ptr + ev.f_step <= ev.f_max) (*ev.f_ptr) += ev.f_step;
			else *ev.f_ptr = ev.f_max;
		} else {
			if (*ev.i_ptr + ev.i_step <= ev.i_max) (*ev.i_ptr) += ev.i_step;
			else *ev.i_ptr = ev.i_max;
		}
		break;

	case OEV_EVENT_CLICK:
		if (ev.is_float) {
			
			//_ask_apply();
		} else {
			//_ask_apply();
		}
		fw_setzoffset(otm_zoffset);
		ev.is_editing = false;
		OCT_UIO_RestoreIndex();
		otm_state_pop();
		if (ev.zup_on_finished) oc_block_up_mm(5);
		return true;
		break;
	}
	_draw_value();

	#ifdef USE_REALTIME_ZOFFSET
	
	if (ev.is_float && ev.f_ptr == &otm_zoffset) {
		float diff_f = *ev.f_ptr - old_f;

		#if 0 // for test
		#define LOG2(fmt, ...) kb_log(__FILE__, __LINE__, PSTR(fmt), __VA_ARGS__)

		char s[64], tt[16];
		LOG2(" old_zoffset=%s", oc_ftostr31ns(old_f, tt));
		LOG2(" new_zoffset=%s", oc_ftostr31ns(*ev.f_ptr, tt));
		LOG2("diff_zoffset=%s", oc_ftostr31ns(diff_f, tt));
		#else
		if (diff_f > 0) oc_block_down_mm_for_zoffset(diff_f);
		else oc_block_up_mm_for_zoffset(-diff_f);
		#endif
	}
	#endif

	return true;
}

bool g_ignore_update = false, npdm_reserved = false;
void oc_pause();
void oc_resume();
bool oc_is_pausing(void);
bool oc_is_paused(void);
void oc_stop(void);

void npd_setting(void)
{
	char title[32];
	strcpy_P(title, PSTR("Preparing..."));
	int x = 40, y = 60, w = 320-x*2, h = 240-y*2;
	int tw = tft_stringwidth(title), th = tft_stringheight(title);
	tft_fillrect(x, y, w, h, WHITE);
	tft_printf(x + (w-tw)/2, y + (h-th)/2, 0, BLACK, WHITE, NULL, title);
	otm_zoffset = 17.0;
	fw_setzoffset(otm_zoffset);
	fw_npdm_prepare();
	npdm_reserved = true;
}

#ifdef USE_PROBE_SAFETY_CHECK
bool user_confirmed_probe_error = false;
bool otm_user_confirmed_probe_error(void)
{
	return user_confirmed_probe_error;
}
#endif

void otm_command(int cmdid, int param)
{
	static bool in_cmd = false;
	if (in_cmd) return;
	in_cmd = true;
	LOG("cmdid=%d", cmdid);

	switch (cmdid) {
	// return to previous state
	case CMDID_BACK:
		#ifdef USE_PROBE_SAFETY_CHECK
		user_confirmed_probe_error = true;
		#endif
		otm_state_pop();
		break;

	// transit new state
	case CMDID_FILEBROWSE: otm_state_push(OTM_STATE_FILEBROWSE); break;
	case CMDID_PREPARE:    otm_state_push(OTM_STATE_PREPARE);    break;
	case CMDID_SETTINGS:   otm_state_push(OTM_STATE_SETTINGS);   break;
	case CMDID_PSETTINGS:  otm_state_push(OTM_STATE_PSETTINGS);   break;
	case CMDID_JOG:        otm_state_push(OTM_STATE_JOG);   break;
	case CMDID_INFO:       otm_state_push(OTM_STATE_INFO);   break;
	case CMDID_PRINT_SDFILE:
		print_current_uio_file(param);
		break;

	case CMDID_CHDIR:
		change_dir(param);
		break;

	case CMDID_UPDIR:
		up_dir();
		break;

	case CMDID_PAUSE:
		if (oc_is_pausing() || oc_is_paused()) break;
		#if 0
		sd_pause();
		#else
		oc_pause();
		#endif
		otm_state_change(OTM_STATE_PAUSED);
		break;

	case CMDID_RESUME:
		if (!oc_is_paused()) break;
		otm_state_change(OTM_STATE_PRINTING);
		#if 0
		g_ignore_update = true;
		sd_resume();
		g_ignore_update = false;
		#else
		oc_resume();
		#endif
		break;

	case CMDID_STOP:
		sd_stop();
		oc_stop();
		otm_state_pop();
		break;

	// control printer
	case CMDID_HOME: // G28
		fw_home();
		break;
	#if 0
	case CMDID_LEFT:    fw_left();     break;
	case CMDID_RIGHT:   fw_right();    break;
	case CMDID_REAR:    fw_rear();     break;
	case CMDID_FRONT:   fw_front();    break;
	case CMDID_UP:      fw_up();       break;
	case CMDID_DOWN:    fw_down();     break;
	case CMDID_EXTRUDE:
		//OCT_UIO_Disable();
		fw_extrude();
		break;

	case CMDID_RETRACT:
		//OCT_UIO_Disable();
		fw_retract();
		break;

	#endif
	case CMDID_G29:
		OCT_UIO_Disable();
		fw_g29();
		break;

	case CMDID_NPDM:
		npd_setting();
		break;

	case CMDID_MOTOROFF:fw_motoroff(); break;
	case CMDID_PREHEAT: 
		//fw_preheat();  
		fw_settemp(otm_preheat_temp);
		fw_setfanspeed(otm_fan_speed);
		break;

	case CMDID_COOLDOWN:
		fw_cooldown();
		fw_setfanspeed(0);
		break;

	case CMDID_CFGLINE_EEPROM+0: _EditValueInt(&otm_preheat_temp,   0, 250,   1, text78x21_nozzle_temp, "`C"); break;
	case CMDID_CFGLINE_EEPROM+1: _EditValueInt(&otm_printing_speed, 5, 500,   1, text78x21_print_speed, "%"); break;
	case CMDID_CFGLINE_EEPROM+2: _EditValueInt(&otm_fan_speed,      0, 100,   1, text78x21_fan_speed, "%"); break;
	case CMDID_CFGLINE_EEPROM+3: _EditValueFloat(&otm_zoffset,      ZOFFSET_MIN, ZOFFSET_MAX, 0.1, text78x21_nozzle_offset, "mm", false); break;
	}
	in_cmd = false;
}

bool knob_press_process(void)
{
	///////////if (!_EditValueEvent(OEV_EVENT_CLICK))
	return OCT_UIO_Blocked_Process((void *)encoder_released);
}

long speed_changed_millis = 0;

void speed_down(void)
{
	if (otm.speed_rate > 5) {
		otm.speed_rate -= 1;
		speed_changed_millis = millis();
		otm.speed_display_count = SPEED_DISPLAY_TIME_COUNT;
		//LOG("speed_rate=%d%%", speed_rate);
	}
}

void speed_up(void)
{
	if (otm.speed_rate < 500) {
		otm.speed_rate += 1;
		speed_changed_millis = millis();
		otm.speed_display_count = SPEED_DISPLAY_TIME_COUNT;
		//LOG("speed_rate=%d%%", speed_rate);
	}
}

void speed_apply(void)
{
	long mil = oc_millis();
	if (otm.old_speed_rate != otm.speed_rate) {
		if (mil < speed_changed_millis + 1000) return;
		fw_setspeed(otm.speed_rate);
		otm.old_speed_rate = otm.speed_rate;
	}
	speed_changed_millis = mil; // refresh
}

void uicolorset_down(void)
{
	if (otm.uicolorset_index == 0)
		otm_color_changeset(MAX_UICOLORSET_MAX);
	else
		otm_color_changeset(otm.uicolorset_index-1);
}

void uicolorset_up(void)
{
	if (otm.uicolorset_index == MAX_UICOLORSET_MAX)
		otm_color_changeset(0);
	else
		otm_color_changeset(otm.uicolorset_index+1);
}

void knob_ccw(void)
{
	if (!_EditValueEvent(OEV_EVENT_CCW))
	switch (otm.state) {
	case OTM_STATE_PRINTING: speed_down();      break;
	case OTM_STATE_INFO:     uicolorset_down(); break;
	default:                 OCT_UIO_Prev();    break;
	}
}

void knob_cw(void)
{
	if (!_EditValueEvent(OEV_EVENT_CW))
	switch (otm.state) {
	case OTM_STATE_PRINTING: speed_up(); break;
	case OTM_STATE_INFO:     uicolorset_up(); break;
	default:                 OCT_UIO_Next(); break;
	}
}

void refresh_cmd_timeout(void); // in Marlin_main.cpp

bool otm_event(int event, int x, int y)
{
	//LOG("event=%d", event);
	switch (event) {
	case EVENT_G28_STARTED:   
	case EVENT_G29_STARTED:   if (!oc_batch_is_running()) OCT_UIO_Disable(); g_ignore_update = true; break;

	case EVENT_G28_FINISHED:  
	case EVENT_G29_FINISHED: g_ignore_update = false; if (!oc_batch_is_running()) OCT_UIO_Enable(); break;

	case EVENT_BATCH_BEGIN: break;
	case EVENT_BATCH_END:
		if (npdm_reserved) {
			npdm_reserved = false;
			_EditValueFloat(&otm_zoffset, ZOFFSET_MIN, ZOFFSET_MAX, 0.1, text78x21_nozzle_offset, "mm", true);
		}	
		break;

	case EVENT_NPDM_STARTED:  OCT_UIO_Disable(); break;
	case EVENT_NPDM_FINISHED: OCT_UIO_Enable();  break;
	
	case EVENT_UI_ENABLE:  OCT_UIO_Enable();  break;
	case EVENT_UI_DISABLE: OCT_UIO_Disable(); break;

	case EVENT_KNOB_CCW:
		refresh_cmd_timeout();
		knob_ccw();
		break;
	case EVENT_KNOB_CW:
		refresh_cmd_timeout();
		knob_cw();
		break;
	case EVENT_KNOB_CLICK: 
		refresh_cmd_timeout();
		if (!_EditValueEvent(OEV_EVENT_CLICK)) 
			OCT_UIO_Click(); 
		break;

	case EVENT_KNOB_PRESSED: 
		return knob_press_process();
		break;

	case EVENT_COMMAND:   otm_command(x, y); break;

	case EVENT_HIDDEN_MENU:
		if (otm.state == OTM_STATE_INFO)
			otm_state_push(OTM_STATE_TESTMENU);
		#ifdef USE_HIDDENMENU
		else
			otm_state_push(OTM_STATE_HIDDENMENU);
		#endif
		break;

	case EVENT_UIO_PREVPAGE: filebrowser_prevpage(); break;
	case EVENT_UIO_NEXTPAGE: filebrowser_nextpage(); break;

	#ifdef USE_PROBE_SAFETY_CHECK
	case EVENT_PROBE_ERROR: 
		user_confirmed_probe_error = false;
		otm_state_push(OTM_STATE_ERROR);
		break;

	case EVENT_PROBE_ERROR_RESOLVED:
		otm_state_change(OTM_STATE_ERROR_RESOLVED);
		break;
	#endif
	}
	return false;
}

void otm_buttons_update(int but)
{
	encoder_update();
}

void otm_setstatus(const char* message)
{
	//LOG("otm_setstatus: msg=%s", message);
}

void otm_run(int lut);

void hd_detect(void);
bool oc_paused_callback_ready(void);
void oc_pause_post(void);

void encoder_process();

void otm_update(int lut)
{
	if (g_ignore_update) return;

	#ifdef WORKAROUND_FOR_ILI9325
	if (tft_init_failed) {
		tft_init_failed = false;

		tft_init();
		tft_off();
		#ifndef UI_V2
		otm_background0();
		#endif
		otm.state = OTM_STATE_NONE; // BUGFIX: Fixed black-out screen after SD-card inserted
		otm_state_change(OTM_STATE_MAIN);
		tft_on();
	}
	#endif

	encoder_update();
	encoder_process();

	if (oc_paused_callback_ready()) oc_pause_post();
	
	static int cnt=0;
	static long old;
	long mil = millis();
	cnt++;
	if (mil >= old+1000) {
		old = mil;
		#ifdef SHOW_UPDATE_EVENT
		LOG("otm_update(cnt=%d) sched_state=%s", cnt, otm_state_string(tmp, otm.sched_state));
		#endif
		//LOG("otm_update(%s)", loc);
	}
	//
	// BUGFIX: Fixed problem of encoder's double event!
	//
	//otm_buttons_update(loc);

	counting_update();
	if (otm.state == OTM_STATE_PRINTING) speed_apply();
	update_all_every_1sec(false);

	//
	// SD card detection
	//
	int sd_rc = sd_detection();
	if (sd_rc == OC_SD_INSERTED) {
		tft_init();
		tft_off();
		#ifndef UI_V2
		otm_background0();
		#endif
		otm.subdir_depth = 0;
		otm.state_sp = 0;
		otm.state = OTM_STATE_NONE; // BUGFIX: Fixed black-out screen after SD-card inserted
		otm_state_change(OTM_STATE_MAIN);
		tft_on();
	} else if (sd_rc == OC_SD_REMOVED) {
		if (otm.state == OTM_STATE_FILEBROWSE) {
			otm.subdir_depth = 0;
			otm.state_sp = 0;
			otm.state = OTM_STATE_NONE;
			otm_state_change(OTM_STATE_MAIN);
		}
	}

	//
	// Tool Detection
	//
	td_detect();

	otm_run(lut);

	OCT_UI_DrawChangedOnly();
}

void otm_run(int lut)
{
	counting_run();

	if (get_sched_state() != OTM_STATE_NONE) {
		if (lut == LUT_LOOP) {
			//LOG("!!!!! otm_run/otm_transit(sched_state=%s) !!!!!", otm_state_string(tmp, get_sched_state()));
			otm_state_change(get_sched_state());
			set_sched_state(OTM_STATE_NONE);
			return;
		} else {
			//LOGP("!!!!! loc is not loop !!!!!");
			static long old;
			long mil = millis();
			if (mil >= old+1000) {
				old = mil;
				//LOG("loc=%s", loc);
			}
		}
	}
	//encoder_test();
}

bool usb_printing = false;

void otm_sched_finished(void)
{
	usb_printing = false;
	set_sched_state(OTM_STATE_PRINTED);
}

void otm_sched_main(void)
{
	usb_printing = false;
	set_sched_state(OTM_STATE_MAIN);
}

void otm_sched_prepare(void)
{
	set_sched_state(OTM_STATE_PREPARE);
}

void otm_sched_jog(void)
{
	set_sched_state(OTM_STATE_JOG);
}

void otm_sched_settings(void)
{
	set_sched_state(OTM_STATE_SETTINGS);
}

void otm_sched_printing(void)
{
	usb_printing = true;
	set_sched_state(OTM_STATE_PRINTING);

	sd_getFilename(otm.filename, sizeof(otm.filename));
	sd_getLongfilename(otm.longFilename, sizeof(otm.longFilename));
	LOG("file='%s'", otm.longFilename);
//	strncpy(tm.filename, filename, sizeof(tm.filename));
//	strncpy(tm.longFilename, longFilename, sizeof(tm.longFilename));
}

void otm_reset_alert_level()
{
}

void main_update(void)
{
	//update_all_every_1sec();
}

