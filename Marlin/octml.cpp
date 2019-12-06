//
//  octml.c
//
//  OpenCreators TFT Menu Library module
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
#include "octml.h"

bool otm_event(int event, int x, int y);


#define MAX_UIO 20
OCT_UI_t uio[MAX_UIO];
int uio_current = -1;
int uio_pressed = -1;
bool uio_disabled = false;

void OCT_Init(void)
{
}

void OCT_Term(void)
{
}

static int uio_index_saved, uio_restore_req = 0;
void OCT_UIO_SaveIndex(void)
{
	uio_index_saved = uio_current;
}

void OCT_UIO_RestoreIndex(void)
{
	uio_restore_req = uio_index_saved;
}

void OCT_UI_Init(void)
{
	int i;
	for (i=0; i<MAX_UIO; i++) {
		uio[i].active = false;
	}
	uio_current = uio_restore_req;
	uio_restore_req = 0;
}

extern void OCT_UIO_DrawIcon(OCT_UI_t *pb);

uint16_t uio_colorcurrent = YELLOW;

void OCT_Button_SetCurrentColor(uint16_t color)
{
	uio_colorcurrent = color;
}

#ifdef UI_V2
void jog_button_frontrear(OCT_UI_t *pb, uint16_t fg_color, uint16_t bg_color, uint16_t edge_color)
{
	int x = pb->x + 30, y = pb->y;
	int i;
	for (i=0; i<10; i++) {
		tft_fillrect(x-i*3, y+i, i*6, 1, bg_color);
	}
	for (i=0; i<10; i++) {
		tft_fillrect(x-30+i*3, y+10+i, 60, 1, bg_color);
	}
	for (i=0; i<10; i++) {
		tft_fillrect(x+i*3, y+20+i, 60-i*6, 1, bg_color);
	}
	tft_drawline(pb->x+30, pb->y, pb->x+90, pb->y+20, edge_color);
	tft_drawline(pb->x+90, pb->y+20, pb->x+60, pb->y+30, edge_color);
	tft_drawline(pb->x+60, pb->y+30, pb->x, pb->y+10, edge_color);
	tft_drawline(pb->x, pb->y+10, pb->x+30, pb->y, edge_color);
}

void jog_button_updown(OCT_UI_t *pb, uint16_t fg_color, uint16_t bg_color, uint16_t edge_color)
{
	int x = pb->x, y = pb->y, w = pb->w, h = pb->h;
	int i;
	for (i=0; i<20; i++) {
		tft_fillrect(x+w-i*3, y+i, i*3, 1, bg_color);
	}
	tft_fillrect(x, y+20, w, h-40, bg_color);
	for (i=0; i<20; i++) {
		tft_fillrect(x, y+20+h-40+i, w-i*3, 1, bg_color);
	}
	tft_drawline(pb->x, pb->y+20, pb->x+w, pb->y, edge_color);
	tft_drawline(pb->x+w, pb->y, pb->x+w, pb->y+20+h-40, edge_color);
	tft_drawline(pb->x+w, pb->y+20+h-40, pb->x, pb->y+h, edge_color);
	tft_drawline(pb->x, pb->y+h, pb->x, pb->y+20, edge_color);
}

void jog_button_leftright(OCT_UI_t *pb, uint16_t fg_color, uint16_t bg_color, uint16_t edge_color)
{
	int x = pb->x, y = pb->y, h = pb->h;
	int i;
	for (i=0; i<10; i++) {
		tft_fillrect(x, y+i, i*3, 1, bg_color);
	}
	tft_fillrect(x, y+10, 30, h-20, bg_color);
	for (i=0; i<10; i++) {
		tft_fillrect(x+i*3, y+h-20+10+i, 30-i*3, 1, bg_color);
	}
	tft_drawline(pb->x, pb->y, pb->x+30, pb->y+10, edge_color);
	tft_drawline(pb->x+30, pb->y+10, pb->x+30, pb->y+h, edge_color);
	tft_drawline(pb->x+30, pb->y+h, pb->x, pb->y+h-10, edge_color);
	tft_drawline(pb->x, pb->y+h-10, pb->x, pb->y, edge_color);
}

void draw_jogbutton_icon(OCT_UI_t *pb, uint16_t fg_color, uint16_t bg_color, bool do_not_change_fg);

void jog_button(OCT_UI_t *pb, uint16_t fg_color, uint16_t bg_color, uint16_t edge_color, bool do_not_change_fg)
{
	int id = (int)pb->icon;
	switch (id) {
	case JOG_BUTTON_FRONT_ID: 
	case JOG_BUTTON_REAR_ID: jog_button_frontrear(pb, fg_color, bg_color, edge_color); break;

	case JOG_BUTTON_UP_ID: 
	case JOG_BUTTON_DOWN_ID: jog_button_updown(pb, fg_color, bg_color, edge_color); break;

	case JOG_BUTTON_RIGHT_ID: 
	case JOG_BUTTON_LEFT_ID: jog_button_leftright(pb, fg_color, bg_color, edge_color); break;
	}
	draw_jogbutton_icon(pb, fg_color, bg_color, do_not_change_fg);
}

bool ev_is_editing(void);

void OCT_ButtonDraw(OCT_UI_t *pb, bool pressed)
{
	uint16_t fg_color = pb->fcol, bg_color = pb->bcol, edge_color = pb->ecol;
	bool do_not_change_fg = false;

	if (ev_is_editing()) return;

	// setting button style
	if (uio_disabled) {
		edge_color = OCE_COLOR_TO_RGB565(0xff404040);
		bg_color = OCE_COLOR_TO_RGB565(0xff808080);
		fg_color = WHITE;
		do_not_change_fg = true;
	}

	if (uio_current == pb->id) {
		if (pb->ecol == BLACK) edge_color = BLACK;
		else edge_color = OCE_COLOR_TO_RGB565(0xffffff00);
		bg_color = OCE_COLOR_TO_RGB565(0xffffff00);
		fg_color = BLACK;
		do_not_change_fg = true;
	}

	if ((int)pb->icon > 0 && (int)pb->icon < 200) {
		jog_button(pb, fg_color, bg_color, edge_color, do_not_change_fg);
		return;
	}

	// draw button frames
	int n = 2, i;
	for (i=0; i<n; i++) {
		tft_drawrect(pb->x+i, pb->y+i, pb->w-i*2, pb->h-i*2, edge_color);
	}
	tft_fillrect(pb->x+n, pb->y+n, pb->w-n*2, pb->h-n*2, bg_color);

	// draw icon
	if (pb->icon) {
		tft_setbitmapcolor(fg_color, bg_color);

		if (pb->flags & OCT_BF_SPECIAL_BUTTON1) {
			tft_drawbitmap(pb->x+43, pb->y+4, 0, (const uint8_t *)pb->icon);
			tft_printf(pb->x+136, pb->y+4, 0, fg_color, bg_color, NULL, "%s", pb->title);
		} else if (pb->flags & OCT_BF_ALIGNLEFT) {
			tft_drawbitmap(pb->x+5, pb->y+4, 0, (const uint8_t *)pb->icon);
		} else
			OCT_UIO_DrawIcon(pb);
	} else {
		tft_printf(pb->x+2, pb->y+2, 0, fg_color, bg_color, NULL, "%s", pb->title);
	}
}

#else

void OCT_ButtonDraw(OCT_UI_t *pb, bool pressed)
{
	#ifdef OC_TFT
	int sw = tft_stringwidth(pb->title), sh = tft_stringheight(pb->title);
	int tx = pb->x+(pb->w-sw)/2, ty = pb->y+(pb->h-sh)/2;

	if (pb->flags & OCT_BF_ALIGNLEFT) tx = pb->x + 5;

	int16_t title_color = BLACK, bg_color = WHITE, edge_color = BLACK, edge2_color = WHITE;
	if (!uio_disabled) {
		if (uio_current == pb->id) {
			title_color = BLUE;
			bg_color = uio_colorcurrent;// OCE_COLOR_TO_RGB565(0xffa0a0a0);
			//edge_color = BLACK;
			edge2_color = BLUE;
		}
	}

	tft_drawrect(pb->x-1, pb->y-1, pb->w+2, pb->h+2, edge2_color);
	tft_drawrect(pb->x, pb->y, pb->w, pb->h, edge_color);

	if (pressed) bg_color = LIGHT_GREEN;

	tft_fillrect(pb->x+1, pb->y+1, pb->w-2, pb->h-2, bg_color);

	if (pb->flags & OCT_BF_SPECIAL_BUTTON1) {
		const uint8_t *p = (const uint8_t *)pb->icon;
		tft_setbitmapcolor(BLACK, bg_color);
		tft_drawbitmap(pb->x+40, pb->y+1, 0, p);
		tft_printf(tx, ty, 0, title_color, bg_color, NULL, "%s", pb->title);
	} else {
		if (pb->icon) {
			#if 0
			const uint8_t *p = (const uint8_t *)pb->icon;
			tft_setbitmapcolor(BLACK, bg_color);

			if (pb->flags & OCT_BF_ALIGNICONCENTER) {
				int16_t iw = tft_bitmap_width(p);
				int16_t ih = tft_bitmap_height(p);
				
				tft_drawbitmap(pb->x+(pb->w-iw)/2, pb->y+(pb->h-ih)/2, 0, p);
			} else 
				tft_drawbitmap(pb->x+1, pb->y+1, 0, p);
			#else
			tft_setbitmapcolor(BLACK, bg_color);
			OCT_UIO_DrawIcon(pb);
			#endif
		} else 
			tft_printf(tx, ty, 0, title_color, bg_color, NULL, "%s", pb->title);
	}
	#endif
}
#endif

#define UIO_VISIBLE(o) (o.active)

void OCT_UIO_Draw(int index, bool pressed)
{
	if (index < 0 || index >= MAX_UIO) return;
	if (UIO_VISIBLE(uio[index]))
	{
		switch (uio[index].type) {
		case UIO_TYPE_BUTTON: OCT_ButtonDraw(&uio[index], pressed); break;
		}
	}	
}

void OCT_UI_Draw(void)
{
	int i;
	for (i=0; i<MAX_UIO; i++) OCT_UIO_Draw(i, false);
}

bool s_drawn = false;
int s_last = -1;
bool in_dco = false;

void OCT_UI_DrawChangedOnly(void)
{
	if (s_last == uio_current) return;
	if (in_dco) {
		//LOGP("in_dco!!!!!");
		return;
	}
	in_dco = true;
	OCT_UIO_Draw(s_last, false);
	OCT_UIO_Draw(uio_current, false);
	s_last = uio_current;
	s_drawn = true;
	//LOGP("s_drawn=true");
	in_dco = false;
}

int OCT_ButtonCreateVAR(int x, int y, int w, int h, char *title, int flags, int cmdid, const uint8_t *icon, uint16_t fcol, uint16_t bcol, uint16_t ecol)
{
	int i;
	for (i=0; i<MAX_UIO; i++) if (!uio[i].active) break;
	if (i == MAX_UIO) return -1;

	OCT_UI_t *po = &uio[i];

	po->active = true;
	po->id = i;
	po->type = UIO_TYPE_BUTTON;
	po->x = x;
	po->y = y;
	po->w = w;
	po->h = h;
	po->flags = flags;
	po->cmdid = cmdid;
	po->icon = (void *)icon;
	po->fcol = fcol;
	po->bcol = bcol;
	po->ecol = ecol;
	po->fp_block = NULL;
	strncpy(po->title, title, UIO_TITLE_MAXLEN);

	return i;
}

int OCT_ButtonCreatePSTR(int x, int y, int w, int h, const char *title, int flags, int cmdid, const uint8_t *icon, uint16_t fcol, uint16_t bcol, uint16_t ecol)
{
	int i;
	for (i=0; i<MAX_UIO; i++) if (!uio[i].active) break;
	if (i == MAX_UIO) return -1;

	OCT_UI_t *po = &uio[i];

	po->active = true;
	po->id = i;
	po->type = UIO_TYPE_BUTTON;
	po->x = x;
	po->y = y;
	po->w = w;
	po->h = h;
	po->flags = flags;
	po->cmdid = cmdid;
	po->icon = (void *)icon;
	po->fcol = fcol;
	po->bcol = bcol;
	po->ecol = ecol;
	po->fp_block = NULL;
	strncpy_P(po->title, title, UIO_TITLE_MAXLEN);

	return i;
}

void OCT_UIO_SetParam(int index, int prm)
{
	uio[index].param = prm;
}

void OCT_UIO_SetBlockFunction(int index, void *fp)
{
	uio[index].fp_block = fp;
}

#define UIO_SELECTABLE(o) (o.active)

void uio_change_current(int idx)
{
	if (idx != uio_current) {
		uio_current = idx;
	}
}

void OCT_UIO_Prev(void)
{
	int i = uio_current;
	if (uio[i].flags & OTM_UIOF_PREV_END) return;
	do {
		if (UIO_SELECTABLE(uio[i]) && uio[i].flags & OTM_UIOF_PREV_CMD) {
			otm_event(EVENT_UIO_PREVPAGE, 0, 0);
		}
		i = (i == 0) ? MAX_UIO-1 : i-1;
	} while (i != uio_current && !UIO_SELECTABLE(uio[i]));
	LOG("OCT_UIO_Prev(%d)", i);
	uio_change_current(i);
}

void OCT_UIO_Next(void)
{
	int i = uio_current;
	if (uio[i].flags & OTM_UIOF_NEXT_END) return;
	do {
		if (UIO_SELECTABLE(uio[i]) && uio[i].flags & OTM_UIOF_NEXT_CMD) {
			otm_event(EVENT_UIO_NEXTPAGE, 0, 0);
		}
		i = (i+1)%MAX_UIO;
	} while (i != uio_current && !UIO_SELECTABLE(uio[i]));
	LOG("OCT_UIO_Next(%d)", i);
	uio_change_current(i);
}

void OCT_UIO_Enable(void)
{
	uio_disabled = false;
	OCT_UIO_Draw(uio_current, false);
}

void OCT_UIO_Disable(void)
{
	uio_disabled = true;
	OCT_UIO_Draw(uio_current, false);
}

void OCT_UIO_Click(void)
{
	otm_event(EVENT_COMMAND, uio[uio_current].cmdid, uio[uio_current].param);
}

bool OCT_UIO_Blocked_Process(void *fp_finish)
{
	void (*fp_block)(void *, uint32_t) = (void (*)(void *, uint32_t))uio[uio_current].fp_block;
	if (fp_block) {
		(*fp_block)(fp_finish, 0);
		return true;
	}
	return false;
}

int OCT_UIO_CurrentIndex(void)
{
	if (uio_current == -1) return 0;
	return uio[uio_current].id;
}

#define XY_IN_BUTTON(x, y, po) (x > po->x && x < po->x + po->w && y > po->y && y < po->y + po->h)

static int find_uio_by_xy(int x, int y)
{
	int i;
	for (i=0; i<MAX_UIO; i++) {
		OCT_UI_t *po = &uio[i];
		if (po->active 
			#if 0
			&& !button[i].disabled && !button[i].freezed 
			#endif
			&& XY_IN_BUTTON(x, y, po)) return i;
	}
	return -1;
}

