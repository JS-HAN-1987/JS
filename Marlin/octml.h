//
//  octml.h
//
//  OpenCreators TFT Menu Library module header
//
//  2015. 4. 7.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#ifndef __OCTML_H__
#define __OCTML_H__

#define UIO_TYPE_BUTTON 1

#define UIO_TITLE_MAXLEN 24
typedef struct {
	bool active;
	char type;
	int id, x, y, w, h, flags;
	char title[UIO_TITLE_MAXLEN+1];
	int cmdid, param;
	void *icon;
	uint16_t fcol, bcol, ecol;
	void *fp_block;
} OCT_UI_t;

#define OCT_BF_ALIGNLEFT       0x0001
#define OCT_BF_SPECIAL_BUTTON1 0x0002
#define OCT_BF_ALIGNICONCENTER 0x0004

#define OTM_UIOF_PREV_CMD 0x0100
#define OTM_UIOF_NEXT_CMD 0x0200
#define OTM_UIOF_PREV_END 0x1000
#define OTM_UIOF_NEXT_END 0x2000

#define ICON_COMBI_BACK_ID     1
#define ICON_COMBI_PREHEAT_ID  2
#define ICON_COMBI_COOLDOWN_ID 3
#define ICON_COMBI_EXTRACT_ID  4
#define ICON_COMBI_RETRACT_ID  5
#define ICON_COMBI_PAUSE_ID    6
#define ICON_COMBI_SETTINGS_ID 7
#define ICON_COMBI_STOP_ID     8
#define ICON_COMBI_CONTINUE_ID 9
#define ICON_COMBI_JOG_ID      10
#define ICON_COMBI_PREPARE_ID  11
#define ICON_COMBI_INFO_ID     12

#define ICON_COMBI_BACK     (uint8_t *)ICON_COMBI_BACK_ID
#define ICON_COMBI_PREHEAT  (uint8_t *)ICON_COMBI_PREHEAT_ID
#define ICON_COMBI_COOLDOWN (uint8_t *)ICON_COMBI_COOLDOWN_ID
#define ICON_COMBI_EXTRACT  (uint8_t *)ICON_COMBI_EXTRACT_ID
#define ICON_COMBI_RETRACT  (uint8_t *)ICON_COMBI_RETRACT_ID
#define ICON_COMBI_PAUSE    (uint8_t *)ICON_COMBI_PAUSE_ID
#define ICON_COMBI_SETTINGS (uint8_t *)ICON_COMBI_SETTINGS_ID
#define ICON_COMBI_STOP     (uint8_t *)ICON_COMBI_STOP_ID
#define ICON_COMBI_CONTINUE (uint8_t *)ICON_COMBI_CONTINUE_ID
#define ICON_COMBI_JOG      (uint8_t *)ICON_COMBI_JOG_ID
#define ICON_COMBI_PREPARE  (uint8_t *)ICON_COMBI_PREPARE_ID
#define ICON_COMBI_INFO     (uint8_t *)ICON_COMBI_INFO_ID

#define ICON_COMBI5050_LEFT_ID  101
#define ICON_COMBI5050_RIGHT_ID 102
#define ICON_COMBI5050_UP_ID    103
#define ICON_COMBI5050_DOWN_ID  104
#define ICON_COMBI5050_HOME_ID  105
#define ICON_COMBI5050_G29_ID   106
#define ICON_COMBI5050_NPDM_ID  107
#define ICON_COMBI5050_MOFF_ID  108

#define ICON_COMBI5050_LEFT  (uint8_t *)ICON_COMBI5050_LEFT_ID
#define ICON_COMBI5050_RIGHT (uint8_t *)ICON_COMBI5050_RIGHT_ID
#define ICON_COMBI5050_UP    (uint8_t *)ICON_COMBI5050_UP_ID
#define ICON_COMBI5050_DOWN  (uint8_t *)ICON_COMBI5050_DOWN_ID
#define ICON_COMBI5050_HOME  (uint8_t *)ICON_COMBI5050_HOME_ID
#define ICON_COMBI5050_G29   (uint8_t *)ICON_COMBI5050_G29_ID
#define ICON_COMBI5050_NPDM  (uint8_t *)ICON_COMBI5050_NPDM_ID
#define ICON_COMBI5050_MOFF  (uint8_t *)ICON_COMBI5050_MOFF_ID

#ifdef UI_V2
#define JOG_BUTTON_FRONT_ID 101
#define JOG_BUTTON_REAR_ID  102
#define JOG_BUTTON_UP_ID    103
#define JOG_BUTTON_DOWN_ID  104
#define JOG_BUTTON_RIGHT_ID 105
#define JOG_BUTTON_LEFT_ID  106
#define JOG_BUTTON_FRONT (uint8_t *)JOG_BUTTON_FRONT_ID
#define JOG_BUTTON_REAR  (uint8_t *)JOG_BUTTON_REAR_ID
#define JOG_BUTTON_UP    (uint8_t *)JOG_BUTTON_UP_ID
#define JOG_BUTTON_DOWN  (uint8_t *)JOG_BUTTON_DOWN_ID
#define JOG_BUTTON_RIGHT (uint8_t *)JOG_BUTTON_RIGHT_ID
#define JOG_BUTTON_LEFT  (uint8_t *)JOG_BUTTON_LEFT_ID
#endif


void OCT_UI_Init(void);
void OCT_UI_Draw(void);
void OCT_UI_DrawChangedOnly(void);
void OCT_UIO_Enable(void);
void OCT_UIO_Disable(void);
void OCT_UIO_Prev(void);
void OCT_UIO_Next(void);
void OCT_UIO_Click(void);
int  OCT_UIO_CurrentIndex(void);
void OCT_UIO_SetParam(int index, int prm);
void OCT_UIO_SetBlockFunction(int index, void *fp);
bool OCT_UIO_Blocked_Process(void *fp_finish);

int  OCT_ButtonCreateVAR(int x, int y, int w, int h, char *title, int flags, int cmdid, const uint8_t *icon, uint16_t fcol, uint16_t bcol, uint16_t ecol);
int  OCT_ButtonCreatePSTR(int x, int y, int w, int h, const char *title, int flags, int cmdid, const uint8_t *icon, uint16_t fcol, uint16_t bcol, uint16_t ecol);
#define OCT_ButtonCreate(x, y, w, h, title, flags, cmdid, icon, fcol, bcol, ecol) OCT_ButtonCreatePSTR(x, y, w, h, PSTR(title), flags, cmdid, icon, fcol, bcol, ecol)
void OCT_Button_SetCurrentColor(uint16_t color);

void OCT_UIO_SaveIndex(void);
void OCT_UIO_RestoreIndex(void);

#endif // __OCTML_H__

