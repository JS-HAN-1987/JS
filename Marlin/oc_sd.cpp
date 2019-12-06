//
//  oc_sd.c
//
//  OpenCreators Firmware SD module
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#include "Marlin.h"
#include "oc_firmware.h"

#define _SD_______________
////////////////////////////////////////////////////////////////////////////////
#ifdef SDSUPPORT

#if (SDCARDDETECT > 0)
bool lcd_oldcardstatus;
#endif

#include "cardreader.h"
extern  CardReader card;

void sd_init(void)
{
	card.initsd();
}

void sd_init0(void)
{
#if defined (SDSUPPORT) && defined(SDCARDDETECT) && (SDCARDDETECT > 0)
    pinMode(SDCARDDETECT,INPUT);
    WRITE(SDCARDDETECT, HIGH);
    lcd_oldcardstatus = IS_SD_INSERTED;
	if (lcd_oldcardstatus) {
		sd_init();
	}
#endif//(SDCARDDETECT > 0)
}

void sd_term(void)
{
	card.release();
}

bool sd_openfile(char *file)
{
	card.openFile(file, true, false);
	LOGP("card.openFile->card.isFileOpen");
	return card.isFileOpen();
}

void sd_closefile(void)
{
	card.closefile(false);
}

int sd_detection(void)
{
	int rc = 0;
    #if (SDCARDDETECT > 0)
    if((IS_SD_INSERTED != lcd_oldcardstatus))
    {
        lcd_oldcardstatus = IS_SD_INSERTED;
        //lcd_implementation_init(); // to maybe revive the LCD if static electricity killed it.

        if(lcd_oldcardstatus)
        {
			LOGP("Card inserted!!!!!!!!!!!!!!!");
			sd_init();
			rc = OC_SD_INSERTED;
        }
        else
        {
			LOGP("Card removed!!!!!!!!!!!!!!!!");
			sd_term();
			rc = OC_SD_REMOVED;
        }
    }
    #endif//CARDINSERTED
	return rc;
}

int sd_getcount(void)
{
	return card.getnrfilenames();
}

void sd_chdir(char *dir)
{
	card.chdir(dir);
}

void sd_updir(void)
{
	card.updir();
}

static long cc2long(char c0, char c1)
{
	long n = ((int)(c0-'0'))*10+(c1-'0');
	LOG("cc2int: '%c%c' => %ld", c0, c1, n);
	return n;
}

static long timestr2seconds(char *s)
{
	if (s[2] == ':' && s[5] == ':') {
		return cc2long(s[0], s[1]) * 3600 + cc2long(s[3], s[4])*60 + cc2long(s[6], s[7]);
	}
	return 0;
}

static int lengthstr2weights(char *s)
{
	long l = atol(s);
	if (l) {
		float f = 0.002829 * l;
		l = f;
	}
	return l;
}

void sd_getfileinfo(int index, char *filename, char *longFilename, bool *is_dir, int *ptime, int *pweight)
{
	LOGP("----- sd_getfileinfo --------------");
	int count = card.getnrfilenames();
	LOG("count=%d", count);
	*is_dir = false;
	if (index < count) {

#if 1
		index = count - 1 - index;
		card.getfilename(index);
		if (card.filenameIsDir) {
			sprintf_P(filename, PSTR("%s"), card.filename);
			sprintf_P(longFilename, PSTR("[%s]"), card.filename);
			*is_dir = true;
		} else {
			//strcpy(filename, card.longFilename);
			strcpy(filename, card.filename);
			strcpy(longFilename, card.longFilename);
		}
		*ptime = 0; //1*3600+2*60+3;
		*pweight = 0; //350;
#else
		index = count - 1 - index;
		
		card.getfilename(index);
		LOG("filename=%s", card.filename);
		LOG("isDir=%d", card.filenameIsDir);
		LOG("longFilename=%s", card.longFilename);
		if (card.filenameIsDir) {
			sprintf(filename, "[%s]", card.filename);
			strcpy(longFilename, card.filename);
		} else {
			//strcpy(filename, card.longFilename);
			strcpy(filename, card.filename);
			strcpy(longFilename, card.longFilename);
		}

		LOG("filename='%s'", filename);
		LOG("long name='%s'", longFilename);

		// get time estimation and weight
		//
		if (sd_openfile(filename)) {

			static char buf[60];
			int16_t rlen;
			int i;

			for (i=0; i<9; i++) card.fgets(buf, sizeof(buf));

			// time
			rlen = card.fgets(buf, sizeof(buf));
			LOG("FILE: '%s'", buf);
			char *s = strstr(buf, "Printing time: ");
			if (s) {
				s += 15;
				*ptime = timestr2seconds(s);
				LOG("*ptime=%ld", *ptime);
			} else 
				*ptime = 0;

			// length
			rlen = card.fgets(buf, sizeof(buf));
			LOG("FILE: '%s'", buf);		
			s = strstr(buf, "Filament length: ");
			if (s) {
				s += 17;
				*pweight = lengthstr2weights(s);
			} else 
				*pweight = 0;
			
			sd_closefile();
		} else {
			// time & weight
			*ptime = 0; //1*3600+2*60+3;
			*pweight = 0; //350;
		}
#endif		
	}
}

void sd_setpos(long pos)
{
	card.setIndex(pos);
}

int sd_getpos(void)
{
	return -1;
}

uint32_t sd_filesize(void)
{
	return card.getFilesize();
}

void sd_read(uint8_t *buffer, int size)
{
	while (size--) *buffer++ = card.get();
}

void oc_sdpause(void);
void oc_sdresume(void);

void sd_pause()
{
	card.pauseSDPrint();
	//oc_sdpause();
}

void sd_resume()
{
	//oc_sdresume();
	card.startFileprint();
}

void quickStop();
#include "temperature.h"
extern bool cancel_heatup;
#ifdef ENABLE_AUTO_BED_LEVELING
extern bool stop_g29;
#endif
void sd_stop()
{
#ifdef ENABLE_AUTO_BED_LEVELING
	stop_g29 = true;
#endif
    card.sdprinting = false;
    card.closefile();
    quickStop();
    if(SD_FINISHED_STEPPERRELEASE)
    {
        enquecommands_P(PSTR(SD_FINISHED_RELEASECOMMAND));
    }
    autotempShutdown();

	cancel_heatup = true;
	//lcd_setstatus(MSG_PRINT_ABORTED);

	//
}

bool sd_isPrinting()
{
	return !card.eof();
}

void sd_getFilename(char *buf, int bufsize)
{
	strncpy(buf, card.filename, bufsize);
}

void sd_getLongfilename(char *buf, int bufsize)
{
	strncpy(buf, card.longFilename, bufsize);
}

void get_progress(uint32_t *ppos, uint32_t *psize)
{
	card.get_progress(ppos, psize);
}

int  sd_percent(void)
{
	return (int)card.percentDone();
}


#else // SDSUPPORT

void sd_init() {}
void sd_term() {}
int  sd_detection() {}
int sd_getcount(void) { return 0; }
void sd_getfileinfo(int index, char *filename, char *longFilename, bool *is_dir, int *ptime, int *pweight) {}

#endif // SDSUPPORT


