//
//  oc_firmware.c
//
//  OpenCreators Firmware header
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#include "oc_firmware.h"
#include "../../../../Marlin.h"

#define _LOG________________
#include <stdarg.h>

	#define ESC "\x1b"oc_

	#define ANSI_RESET ESC "[0m"
	#define ANSI_OK    ESC "[30;42;1m" ESC "[K"
	#define ANSI_ERR   ESC "[1;41m" ESC "[K"
	#define ANSI_WARN  ESC "[33;1m" ESC "[K"
	#define ANSI_NOTE  ESC "[31;1m" ESC "[K"
	#define ANSI_API   ESC "[37;44m" ESC "[K"
	#define ANSI_API2  ESC "[30;46m" ESC "[K"
	#define ANSI_SVC   ESC "[45;30m" ESC "[K"
	#define ANSI_EVT   ESC "[30;43m" ESC "[K"
	#define ANSI_LOGIC ESC "[45;30m" ESC "[K"

int get_sched_state(void);

#if 0
void kb_log_pgm(const char *file, int line, const char *str)
{
	MYSERIAL.print(get_sched_state());
	
	MYSERIAL.print("[");
	MYSERIAL.print(file);
	MYSERIAL.print(":");
	MYSERIAL.print(line);
	MYSERIAL.print("]");
	char ch=pgm_read_byte(str);
	while(ch)
	{
		MYSERIAL.write(ch);
		ch=pgm_read_byte(++str);
	}
	MYSERIAL.println("");
}
#endif

void kb_log(const char *file, int line, const char *fmt PROGMEM, ...)
{
	static char in_log = 0;
	if (in_log) return;
	in_log = 1;
    int len, noln = 0;
    va_list va;
	char buf[80];
	const char *h = "", *t = "";
 
    sprintf_P(buf, PSTR("[%s:%d]: "), file, line);
 
    va_start(va, fmt);
    len = vsnprintf_P(buf+strlen(buf), sizeof(buf)-strlen(buf), fmt, va);
	va_end(va);
    if (buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = 0;
    if (buf[strlen(buf)-1] == '\a') { buf[strlen(buf)-1] = 0; noln = 1; }
	
//	buf[sizeof(buf)/sizeof(buf[0])-1] = '\0';

	if (strstr(buf, LT_OK))    { h = ANSI_OK;    t = ANSI_RESET; } else
    if (strstr(buf, LT_ERR))   { h = ANSI_ERR;   t = ANSI_RESET; } else
    if (strstr(buf, LT_WARN))  { h = ANSI_WARN;  t = ANSI_RESET; } else
    if (strstr(buf, LT_NOTE))  { h = ANSI_NOTE;  t = ANSI_RESET; } else
    if (strstr(buf, LT_API))   { h = ANSI_API;   t = ANSI_RESET; } else
    if (strstr(buf, LT_API2))  { h = ANSI_API2;  t = ANSI_RESET; } else
    if (strstr(buf, LT_SVC))   { h = ANSI_SVC;   t = ANSI_RESET; } else
    if (strstr(buf, LT_EVT))   { h = ANSI_EVT;   t = ANSI_RESET; } else
    if (strstr(buf, LT_LOGIC)) { h = ANSI_LOGIC; t = ANSI_RESET; }

	MYSERIAL.print(h);
	MYSERIAL.print(buf);
	if (noln) MYSERIAL.print(t);
	else      MYSERIAL.println(t);

	in_log = 0;
}


//
void enquecommands_P(const char* pgcode);

void fw_home(void) { enquecommands_P(PSTR(GCODE_STR_HOME)); }
#if 0
void fw_left(void)     { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_LEFT10));  enquecommands_P(PSTR("G90")); }
void fw_right(void)    { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_RIGHT10)); enquecommands_P(PSTR("G90")); }
void fw_rear(void)     { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_REAR10));  enquecommands_P(PSTR("G90")); }
void fw_front(void)    { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_FRONT10)); enquecommands_P(PSTR("G90")); }
void fw_up(void)       { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_UP5));     enquecommands_P(PSTR("G90")); }
void fw_down(void)     { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_DOWN5));   enquecommands_P(PSTR("G90")); }
void fw_extrude(void) { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_EXTRACT20));  enquecommands_P(PSTR("G90")); }
void fw_retract(void) { enquecommands_P(PSTR("G91")); enquecommands_P(PSTR(GCODE_STR_RETRACT20));  enquecommands_P(PSTR("G90")); }
#endif
void fw_g29(void)      { enquecommands_P(PSTR(GCODE_STR_AUTOLEVEL)); }
void fw_npdm(void)      { enquecommands_P(PSTR(GCODE_STR_NPDM)); }
void fw_preheat(void)  { enquecommands_P(PSTR(GCODE_STR_PREHEAT));}
void fw_cooldown(void) { enquecommands_P(PSTR(GCODE_STR_COOLDOWN));}
void fw_motoroff(void)      { enquecommands_P(PSTR(GCODE_STR_MOTOROFF)); }

//

void fw_npdm_prepare(void)
{
	enquecommands_P(PSTR("M885"));
	enquecommands_P(PSTR(GCODE_STR_HOME));
	enquecommands_P(PSTR("G0 Z0 F300"));
	enquecommands_P(PSTR("M886"));
	//oc_wait_for_batch();
}

void fw_settemp(int temp)
{
	char cmd[32];
	snprintf_P(cmd, 31, PSTR("M104 S%d"), temp);
	enquecommand((const char *)cmd);
}

void fw_setspeed(int speed)
{
	char cmd[32];
	snprintf_P(cmd, 31, PSTR("M220 S%d"), speed);
	enquecommand((const char *)cmd);
}

void fw_setfanspeed(int speed)
{
	char cmd[32];
	snprintf_P(cmd, 31, PSTR("M106 S%d"), speed*255/100);
	enquecommand((const char *)cmd);
}

extern float zprobe_zoffset;
void Config_StoreSettings();

float fw_getzoffset(void)
{
	return zprobe_zoffset;
}

void fw_setzoffset(float zoff)
{
	zprobe_zoffset = zoff;
	Config_StoreSettings();     // !!! Always save config for all variable
}


void get_current_coords(float *px, float *py, float *pz)
{
	//*px = current_position[X_AXIS];
	//*py = current_position[Y_AXIS];
	//*pz = current_position[Z_AXIS];
}

void get_elapsed_time(char *dst)
{
	//if (starttime) {
	//	uint16_t time = millis() / 60000 - starttime / 60000;
	//	sprintf_P(dst, PSTR("%02d:%02d"), time / 60, time % 60);
	//} else {
	//	strcpy_P(dst, PSTR("--:--"));
	//}
}

void get_timeleft(char *dst, uint32_t a, uint32_t b)
{
	//if (a == b) {
	//	strcpy_P(dst, PSTR("00:00"));
	//} else if (starttime && a) {
	//	uint16_t et = millis() / 60000 - starttime / 60000;
	//	uint16_t tt = et * b / a;
	//	uint16_t tl = tt - et;
	//	sprintf_P(dst, PSTR("%02d:%02d"), tl / 60, tl % 60);
	//} else {
	//	strcpy_P(dst, PSTR("--:--"));
	//}
}

void get_endstop_status(char *pxt, char *pyt, char *pzt)
{
	#if defined(X_MIN_PIN) && X_MIN_PIN > -1
	*pxt = ((READ(X_MIN_PIN)^X_MIN_ENDSTOP_INVERTING)?1:0);
	#endif

#ifdef NP_RAMPS
	#if defined(Y_MIN_PIN) && Y_MIN_PIN > -1
	*pyt = ((READ(Y_MIN_PIN)^Y_MIN_ENDSTOP_INVERTING)?1:0);
	#endif
#else
	#if defined(Y_MAX_PIN) && Y_MAX_PIN > -1
	*pyt = ((READ(Y_MAX_PIN)^Y_MAX_ENDSTOP_INVERTING)?1:0);
	#endif
#endif

	#if defined(Z_MIN_PIN) && Z_MIN_PIN > -1
	*pzt = ((READ(Z_MIN_PIN)^Z_MIN_ENDSTOP_INVERTING)?1:0);
	#endif
}

#ifdef OC_SUPPORT_DRYRUN
extern int oc_dryrun;
#endif

bool get_dryrun_mode(void)
{
#ifdef OC_SUPPORT_DRYRUN
	return oc_dryrun != 0;
#else
	return false;
#endif
}

long oc_millis(void)
{
	return millis();
}

void sd_printstart(const char* filename, char* longFilename)
{
    char cmd[30];
    char* c;
    sprintf_P(cmd, PSTR("M23 %s"), filename);
    for(c = &cmd[4]; *c; c++)
        *c = tolower(*c);
    enquecommand(cmd);
    enquecommands_P(PSTR("M24"));
    //lcd_return_to_status();
}

