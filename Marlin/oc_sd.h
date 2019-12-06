//
//  oc_sd.h
//
//  OpenCreators Firmware SD header
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//
#ifndef __OC_SD_H__
#define __OC_SD_H__

#define OC_SD_INSERTED 1
#define OC_SD_REMOVED  2

void sd_init(void);
void sd_init0(void);
void sd_term(void);
bool sd_openfile(char *file);
void sd_closefile(void);
int  sd_detection(void);
int  sd_getcount(void);
void sd_chdir(char *dir);
void sd_updir(void);
void sd_getfileinfo(int index, char *filename, char *longFilename, bool *is_dir, int *ptime, int *pweight);
void sd_setpos(long pos);
int  sd_getpos(void);
uint32_t sd_filesize(void);
void sd_read(uint8_t *buffer, int size);
void sd_pause();
void sd_resume();
void sd_stop();
bool sd_isPrinting();
void sd_getFilename(char *buf, int bufsize);
void sd_getLongfilename(char *buf, int bufsize);
int  sd_percent(void);

#endif // __OC_SD_H__

