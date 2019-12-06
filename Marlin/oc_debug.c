//
//  oc_debug.c
//
//  TFT Menu debug module
//
//  2015. 4. 13.
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

#define RGB888_TO_RGB565(r, g, b) \
	( ((((uint16_t)r) >> 3) << 11) | \
	  ((((uint16_t)g) >> 2) << 5) | \
	  (((uint16_t)b) >> 3) )

void otm_background(void);

void color_test(void)
{
	otm_background();

	int i;
	for (i=0; i<255; i++) {
		tft_fillrect(i, 0+20*0, 1, 20, RGB888_TO_RGB565(i, 0, 0));
		tft_fillrect(i, 0+20*1, 1, 20, RGB888_TO_RGB565(0, i, 0));
		tft_fillrect(i, 0+20*2, 1, 20, RGB888_TO_RGB565(0, 0, i));

		tft_fillrect(i, 0+20*3, 1, 20, RGB888_TO_RGB565(i, i, 0));
		tft_fillrect(i, 0+20*4, 1, 20, RGB888_TO_RGB565(0, i, i));
		tft_fillrect(i, 0+20*5, 1, 20, RGB888_TO_RGB565(i, 0, i));

		tft_fillrect(i, 0+20*6, 1, 20, RGB888_TO_RGB565(i, i, i));
	}


	tft_fillrect(10+40*0, 170, 30, 30, OCE_COLOR_TO_RGB565(0xff000000));
	tft_fillrect(10+40*1, 170, 30, 30, OCE_COLOR_TO_RGB565(0xff404000));
	tft_fillrect(10+40*2, 170, 30, 30, OCE_COLOR_TO_RGB565(0xff808000));
	tft_fillrect(10+40*3, 170, 30, 30, OCE_COLOR_TO_RGB565(0xffc0c000));
	tft_fillrect(10+40*4, 170, 30, 30, OCE_COLOR_TO_RGB565(0xfff0f000));
	tft_fillrect(10+40*5, 170, 30, 30, OCE_COLOR_TO_RGB565(0xfffefe00));
	tft_fillrect(10+40*6, 170, 30, 30, OCE_COLOR_TO_RGB565(0xffffff00));


	tft_fillrect(10+40*0, 210, 30, 30, RED);
	tft_fillrect(10+40*1, 210, 30, 30, GREEN);
	tft_fillrect(10+40*2, 210, 30, 30, BLUE);

	tft_fillrect(10+40*4, 210, 30, 30, OCE_COLOR_TO_RGB565(0xffc0c000));

	tft_fillrect(320-10, 240-10, 10, 10, WHITE);

	delay(5000);
}

// in Marlin_main.cpp
extern unsigned long starttime; ///< Print job start time
extern unsigned long stoptime;  ///< Print job stop time

#define LOG2(fmt, ...) kb_log(__FILE__, __LINE__, PSTR(fmt), __VA_ARGS__)

char *otm_current_state_string(char *dst);

#ifdef SHOW_COUNTERS
extern long count_event, count_run, count_update, count_button_update;
#endif

void otm_debug(int mode)
{
	#ifdef USE_DEBUG_OTM
	//
	long mil;
	switch (mode) {
	case 1:
		LOG2("%s", "===== variables =====");
		char tmp[32];
		LOG2("state=%s", otm_current_state_string(tmp));
		#ifdef SHOW_COUNTERS		
		LOG2("count_event=%d",  count_event);
		LOG2("count_run=%d",    count_run);
		LOG2("count_update=%d", count_update);
		LOG2("count_button_update=%d", count_button_update);
		#endif
		LOG2("starttime/1000=%ld", starttime/1000);
		mil = millis();
		LOG2("current_time/1000=%ld", mil/1000);
		LOG2("difftime(current)/1000=%ld", (mil-starttime)/1000);

		LOG2("stoptime/1000=%ld", stoptime/1000);
		LOG2("difftime(finish)/1000=%ld", (stoptime-starttime)/1000);
		break;
	}
	#endif
}

