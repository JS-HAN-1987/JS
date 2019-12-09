//
//  oc_tooldetector.c
//
//  Tool detector module
//
//  2015. 4. 15.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#include "oc_firmware.h"

#ifdef SHIELD_TEST
#define TOOL_DETECT_PIN -1
#else
#define TOOL_DETECT_PIN A3
#endif

void td_init(void)
{
#if defined(TOOL_DETECT_PIN) && (TOOL_DETECT_PIN != -1)
	//pinMode(TOOL_DETECT_PIN, INPUT);
#endif
}

#define TT_NONE 0
#define TT_330  1
#define TT_1K   2
#define TT_4_7K 3
#define TT_UNKNOWN 4

int reg2tooltype(int x)
{
	#if 0
	static int min_x = 9999, max_x = -9999;

	if (x > max_x) max_x = x;
	if (x < min_x) min_x = x;

	static long old;
	long mil = millis();
	if (mil > old+1000) {
		old = mil;
		LOG("x=%d, min=%d, max=%d", x, min_x, max_x);
	}
	#endif
	
#define RVM 30
	#if 1 // analogRead() return values
	if (x > 600) return TT_NONE;
	if (x > 520-RVM && x < 520+RVM) return TT_4_7K;
	if (x > 330-RVM && x < 330+RVM) return TT_1K;
	if (x > 172-RVM && x < 172+RVM) return TT_330;
	#endif

	return TT_UNKNOWN;
}

extern bool uio_disabled;

void td_detect(void)
{
#ifdef DISABLE_NOZZLE_DETECTION
	return;
#endif
	if (uio_disabled) return; // To prohibit detection while g28, g29

	cli();
	int x = analogRead(TOOL_DETECT_PIN);
	sei();

	int tt = reg2tooltype(x);
	static int old_tt = -1;

	if (tt != old_tt) {
		old_tt = tt;
		switch (tt) {
		case TT_NONE: LOGP("Tool removed"); break;
		case TT_330:  LOGP("Tool inserted(330 ohm)"); break;
		case TT_1K:   LOGP("Tool inserted(1K ohm)"); break;
		case TT_4_7K: LOGP("Tool inserted(4.7K ohm)"); break;
		case TT_UNKNOWN: LOG("Unknown tool inserted(x=%d)", x); break;
		}
	}
}

