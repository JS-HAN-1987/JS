//
//  oc_events.h
//
//  OpenCreators Firmware events definition
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//

#ifndef __OC_EVENTS_H__
#define __OC_EVENTS_H__

// event types
#define EVENT_KNOB_CCW       4
#define EVENT_KNOB_CW        5
#define EVENT_KNOB_CLICK     6
#define EVENT_KNOB_PRESSED   7

#define EVENT_COMMAND        8
#define EVENT_HIDDEN_MENU    9

#define EVENT_UIO_PREVPAGE   10
#define EVENT_UIO_NEXTPAGE   11

#define EVENT_UI_ENABLE      12
#define EVENT_UI_DISABLE     13

#define EVENT_G28_STARTED   0x1001
#define EVENT_G28_FINISHED  0x1002
#define EVENT_G29_STARTED   0x1003
#define EVENT_G29_FINISHED  0x1004
#define EVENT_NPDM_STARTED  0x1005
#define EVENT_NPDM_FINISHED 0x1006
#define EVENT_BATCH_BEGIN   0x1007
#define EVENT_BATCH_END     0x1008

#ifdef USE_PROBE_SAFETY_CHECK
#define EVENT_PROBE_ERROR          0x8001
#define EVENT_PROBE_ERROR_RESOLVED 0x8002
#endif

#endif // __OC_EVENTS_H__

