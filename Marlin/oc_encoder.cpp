//
//  oc_encoder.c
//
//  Encoder module
//
//  2015. 4. 15.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#include "oc_firmware.h"
#include "oc_encoder.h"

#define BLEN_A 0
#define BLEN_B 1
#define BLEN_C 2

#define EN_C (1<<BLEN_C)
#define EN_B (1<<BLEN_B)
#define EN_A (1<<BLEN_A)

#define BTN_EN1 OC_EN1
#define BTN_EN2 OC_EN2
#define BTN_ENC OC_ENC

#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1

uint32_t blocking_enc;
uint8_t lastEncoderBits;
int8_t encoderDiff; /* encoderDiff is updated from interrupt context and added to encoderPosition every LCD update */

volatile uint8_t buttons;//Contains the bits of the currently pressed buttons.

int lcdDrawUpdate;
int encoderPosition;
int timeoutToStatus;

#define LCD_CLICKED (buttons & EN_C)

bool otm_event(int event, int x, int y);

long by_g_temp_state;

void encoder_init(void)
{
    pinMode(OC_EN1,INPUT);
    pinMode(OC_EN2,INPUT);
    WRITE(OC_EN1,HIGH);
    WRITE(OC_EN2,HIGH);
	#if OC_ENC > 0
    pinMode(OC_ENC,INPUT);
    WRITE(OC_ENC,HIGH);
	#endif
}

void encoder_update(void)
{
	uint8_t newbutton=0;
	if(digitalRead(BTN_EN1)==0)  newbutton|=EN_A;
	if(digitalRead(BTN_EN2)==0)  newbutton|=EN_B;
	#if BTN_ENC > 0
	if((blocking_enc<millis()) && (digitalRead(BTN_ENC)==0)) newbutton |= EN_C;
	#endif

	#ifdef SHOW_ENCODER_EVENT  	
	if (buttons != newbutton) {
		LOG("buttons=%d", (int)buttons);
	}
	#endif
	buttons = newbutton;

    //manage encoder rotation
    uint8_t enc=0;
    if(buttons&EN_A)
        enc|=(1<<0);
    if(buttons&EN_B)
        enc|=(1<<1);
    if(enc != lastEncoderBits)
    {
		#ifdef SHOW_ENCODER_EVENT  	
		LOG("enc=%d", (int)enc);
		#endif

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

}

void encoder_process(void)
{
#define ENCODER_PULSES_PER_STEP 1
#define LCD_TIMEOUT_TO_STATUS 15000

	static int diff4=0;

	diff4 += encoderDiff;
	if (diff4 > 3) {
		otm_event(EVENT_KNOB_CCW, 0, 0);
		diff4 = 0;
	} else if (diff4 < -3) {
		otm_event(EVENT_KNOB_CW, 0, 0);
		diff4 = 0;
	}

	static bool knob_clicked = false;
	static long knob_click_mil;
	static bool ignore_knob = false;
	if (LCD_CLICKED) {
		if (otm_event(EVENT_KNOB_PRESSED, 0, 0)) ignore_knob = true;
		if (!ignore_knob) {
			if (!knob_clicked) {
				knob_clicked = true;
				knob_click_mil = millis();
			} else {
				if (millis() > knob_click_mil + 3000) {
					knob_clicked = false;
					ignore_knob = true;

					// hidden menu
					otm_event(EVENT_HIDDEN_MENU, 0, 0);
				}
			}
		}
	} else {
		ignore_knob = false;
		if (knob_clicked) {
			long mil = millis();
			long diff = mil - knob_click_mil;
			if (diff > 3000) {
				// ignore more than 3-secs. event
			} else if (diff > 50) {
				// normal click
				otm_event(EVENT_KNOB_CLICK, 0, 0);
			} else {
				// too short
			}
			knob_clicked = false;

		}
	}


        if (abs(encoderDiff) >= ENCODER_PULSES_PER_STEP)
        {
            lcdDrawUpdate = 1;
            encoderPosition += encoderDiff / ENCODER_PULSES_PER_STEP;
			#ifdef SHOW_ENCODER_EVENT  	
			LOG("pos=%d, diff=%d", encoderPosition, encoderDiff);
			#endif
            encoderDiff = 0;
            timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;

			
        }
        if (LCD_CLICKED)
            timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;

}

bool encoder_released(void)
{
	return (digitalRead(BTN_ENC) != 0);
}

