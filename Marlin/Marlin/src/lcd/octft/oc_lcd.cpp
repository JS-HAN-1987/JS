#ifdef __AVR__
	#include <avr/pgmspace.h>
#endif

#include "pins_arduino.h"
#include "wiring_private.h"



#ifdef SHIELD_TEST
#define LCD_RESET A4 // 47
#define LCD_CS    A3 // 32
#define LCD_CD    A2 // 59(A5)
#define LCD_WR    A1 // 58(A4)
#define LCD_RD    A0 // 40

#define LCD_D0 8
#define LCD_D1 9
#define LCD_D2 2
#define LCD_D3 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

#define RD_ACTIVE PORTF &= 0xfe
#define WR_ACTIVE PORTF &= 0xfd
#define CD_ACTIVE PORTF &= 0xfb
#define CS_ACTIVE PORTF &= 0xf7
#define RST_ACTIVE PORTF &= 0xef

#define RD_IDLE PORTF |= 0x01
#define WR_IDLE PORTF |= 0x02
#define CD_IDLE PORTF |= 0x04
#define CS_IDLE PORTF |= 0x08
#define RST_IDLE PORTF |= 0x10

#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

#else

#define LCD_RESET 47 // A4 // 47
#define LCD_CS 32 // A3 // 32
#define LCD_CD 59 // A2 // 59(A5)
#define LCD_WR 58 // A1 // 58(A4)
#define LCD_RD 40 // A0 // 40

#define LCD_D0 63
#define LCD_D1 64
#define LCD_D2 65
#define LCD_D3 66
#define LCD_D4 45
#define LCD_D5 44
#define LCD_D6 43
#define LCD_D7 42

#define WR_ACTIVE PORTF &= 0xef
#define WR_IDLE   PORTF |= 0x10

#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

#endif

/*
    volatile uint8_t *csPort    , *cdPort    , *wrPort    , *rdPort;
	uint8_t           csPinSet  ,  cdPinSet  ,  wrPinSet  ,  rdPinSet  ,
					  csPinUnset,  cdPinUnset,  wrPinUnset,  rdPinUnset,
					  _reset;

*/

#define LCD_RS LCD_CD


#if 0

void lcdSetWriteDir() {
  pinMode(LCD_D0, OUTPUT);
  pinMode(LCD_D1, OUTPUT);
  pinMode(LCD_D2, OUTPUT);
  pinMode(LCD_D3, OUTPUT);  
  pinMode(LCD_D4, OUTPUT);  
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D7, OUTPUT);  
}

void lcdSetReadDir() {
  pinMode(LCD_D0, INPUT);
  pinMode(LCD_D1, INPUT);
  pinMode(LCD_D2, INPUT);
  pinMode(LCD_D3, INPUT);  
  pinMode(LCD_D4, INPUT);  
  pinMode(LCD_D5, INPUT);
  pinMode(LCD_D6, INPUT);
  pinMode(LCD_D7, INPUT);    
}

void lcdWrite8(uint16_t data) {
	#if 0
	PORTH = (PORTH & B10000111) | ((data & 3) << 5) | ((data & 0xc0) >> 3);
	PORTE = (PORTE & B11000111) | ((data & 0xc) << 2) | ((data & 0x20) >> 2);
	PORTG = (PORTG & B11011111) | ((data & 0x10) << 1);
	#else
  digitalWrite(LCD_D0, data & 1);
  digitalWrite(LCD_D1, (data & 2) >> 1);
  digitalWrite(LCD_D2, (data & 4) >> 2);
  digitalWrite(LCD_D3, (data & 8) >> 3);
  digitalWrite(LCD_D4, (data & 16) >> 4); 
  digitalWrite(LCD_D5, (data & 32) >> 5);
  digitalWrite(LCD_D6, (data & 64) >> 6);
  digitalWrite(LCD_D7, (data & 128) >> 7);  
  #endif
}

uint16_t lcdRead8() {
  uint16_t result = digitalRead(LCD_D7);
  result <<= 1;
  result |= digitalRead(LCD_D6);
  result <<= 1;
  result |= digitalRead(LCD_D5);
  result <<= 1; 
  result |= digitalRead(LCD_D4); 
  result <<= 1;
  result |= digitalRead(LCD_D3);
  result <<= 1;
  result |= digitalRead(LCD_D2); 
  result <<= 1;
  result |= digitalRead(LCD_D1);
  result <<= 1;
  result |= digitalRead(LCD_D0); 
  
  return result;
}

void lcdWriteCommand(uint16_t command) {
  lcdSetWriteDir(); 
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);  
  lcdWrite8(command >> 8);
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  lcdWrite8(command);
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_CS, HIGH);    
}


int lcdReadData() {
  uint16_t result;
  lcdSetReadDir();
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);
  
  digitalWrite(LCD_RD, LOW);  
  delayMicroseconds(10);
  result = lcdRead8() << 8;
  digitalWrite(LCD_RD, HIGH);
  
  delayMicroseconds(10);
  
  digitalWrite(LCD_RD, LOW);
  delayMicroseconds(10);
  result |= lcdRead8();
  
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_CS, HIGH);
  
  return result;
}

void lcdWriteData(uint16_t data) {
  
  lcdSetWriteDir();
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);
  
  lcdWrite8(data >> 8);
  
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  
  lcdWrite8(data);
  
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  
  digitalWrite(LCD_CS, HIGH);
}

void lcdWriteRegister(uint16_t addr, uint16_t data) {
  lcdWriteCommand(addr);
  lcdWriteData(data);
}

uint16_t lcdReadRegister(uint16_t reg)
{
	lcdWriteCommand(reg);
	return lcdReadData();
}

uint16_t lcdReadID(void)
{
	//cli();
	uint16_t id = lcdReadRegister(0);
	//sei();
	return id;
}

void lcdFillScreen(uint16_t color) {
  
//  Serial.println("Filling the screen...");
  
  /*lcdWriteRegister(0x0050, 0);
  lcdWriteRegister(0x0051, 219);  
  lcdWriteRegister(0x0052, 0);
  lcdWriteRegister(0x0053, 319);  
  */
  lcdWriteRegister(0x0020, 0);
  lcdWriteRegister(0x0021, 0);
  lcdWriteCommand(0x0022);
  
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);
 
  lcdSetWriteDir();
  
  uint32_t i = 320;
  i *=240;

  while( i-- ) {
    lcdWrite8(color >> 8);
    digitalWrite(LCD_WR, LOW);
    //delayMicroseconds(10);
    digitalWrite(LCD_WR, HIGH); 
    lcdWrite8(color);
    digitalWrite(LCD_WR, LOW);
    //delayMicroseconds(10);
    digitalWrite(LCD_WR, HIGH); 
  }
  
  digitalWrite(LCD_CS, HIGH); 
//  Serial.println("Done filling...");
  
}
void lcdRegisterOther() {
  //Serial.println("Loading LCD registers...");
  lcdWriteRegister(0x00e5,0x8000);
  lcdWriteRegister(0x0000,0x0001);
  lcdWriteRegister(0x0001,0x0100);
  lcdWriteRegister(0x0002,0x0700);
  lcdWriteRegister(0x0003,0x1030);
  lcdWriteRegister(0x0004,0x0000);
  lcdWriteRegister(0x0008,0x0202);
  lcdWriteRegister(0x0009,0x0000);
  lcdWriteRegister(0x000a,0x0000);
  lcdWriteRegister(0x000c,0x0000);
  lcdWriteRegister(0x000d,0x0000);
  lcdWriteRegister(0x000f,0x0000);
  lcdWriteRegister(0x0010,0x0000);
  lcdWriteRegister(0x0011,0x0000);
  lcdWriteRegister(0x0012,0x0000);
  lcdWriteRegister(0x0013,0x0000);
  lcdWriteRegister(0x0010,0x17b0);
  lcdWriteRegister(0x0011,0x0037);
  lcdWriteRegister(0x0012,0x0138);
  lcdWriteRegister(0x0013,0x1700);
  lcdWriteRegister(0x0029,0x000d);
  lcdWriteRegister(0x0020,0x0000);
  lcdWriteRegister(0x0021,0x0000);
  lcdWriteRegister(0x0030,0x0001);
  lcdWriteRegister(0x0031,0x0606);
  lcdWriteRegister(0x0032,0x0304);
  lcdWriteRegister(0x0033,0x0202);
  lcdWriteRegister(0x0034,0x0202);
  lcdWriteRegister(0x0035,0x0103);
  lcdWriteRegister(0x0036,0x011d);
  lcdWriteRegister(0x0037,0x0404);
  lcdWriteRegister(0x0038,0x0404);
  lcdWriteRegister(0x0039,0x0404);
  lcdWriteRegister(0x003c,0x0700);
  lcdWriteRegister(0x003d,0x0a1f);
  lcdWriteRegister(0x0050,0x0000);
  lcdWriteRegister(0x0051,0x00ef);
  lcdWriteRegister(0x0052,0x0000);
  lcdWriteRegister(0x0053,0x013f);
  lcdWriteRegister(0x0060,0x2700);
  lcdWriteRegister(0x0061,0x0001);
  lcdWriteRegister(0x006a,0x0000);
  lcdWriteRegister(0x0090,0x0010);
  lcdWriteRegister(0x0092,0x0000);
  lcdWriteRegister(0x0093,0x0003);
  lcdWriteRegister(0x0095,0x0101);
  lcdWriteRegister(0x0097,0x0000);
  lcdWriteRegister(0x0098,0x0000);
  lcdWriteRegister(0x0007,0x0021);
  lcdWriteRegister(0x0007,0x0031);
  lcdWriteRegister(0x0007,0x0173);
}
void lcdReset(void)
{
	digitalWrite(LCD_CS, HIGH);
	digitalWrite(LCD_WR, HIGH);
	digitalWrite(LCD_RD, HIGH);

    digitalWrite(LCD_RESET, LOW);
    delay(2);
    digitalWrite(LCD_RESET, HIGH);

	digitalWrite(LCD_CS, LOW);
	digitalWrite(LCD_CD, LOW);
	lcdWrite8(0x00);
	
	for(uint8_t i=0; i<3; i++) {
		//WR_STROBE; // Three extra 0x00s
		digitalWrite(LCD_WR, LOW);
		digitalWrite(LCD_WR, HIGH);
	}
	digitalWrite(LCD_CS, HIGH);
}

#else
void lcdWrite8(uint16_t data) {
	#if 0
	#ifdef SHIELD_TEST
	PORTH = (PORTH & B10000111) | ((data & 3) << 5) | ((data & 0xc0) >> 3);
	PORTE = (PORTE & B11000111) | ((data & 0xc) << 2) | ((data & 0x20) >> 2);
	PORTG = (PORTG & B11011111) | ((data & 0x10) << 1);
	#else
	PORTL = (PORTL & B00001111)|((data&B11110000)<<0);
	PORTK = (PORTK & B11100001)|((data&B00001111) << 1);
	#endif
	#else
  digitalWrite(LCD_D0, data & 1);
  digitalWrite(LCD_D1, (data & 2) >> 1);
  digitalWrite(LCD_D2, (data & 4) >> 2);
  digitalWrite(LCD_D3, (data & 8) >> 3);
  digitalWrite(LCD_D4, (data & 16) >> 4); 
  digitalWrite(LCD_D5, (data & 32) >> 5);
  digitalWrite(LCD_D6, (data & 64) >> 6);
  digitalWrite(LCD_D7, (data & 128) >> 7);  
  #endif
}

uint16_t lcdRead8() {
  uint16_t result = digitalRead(LCD_D7);
  result <<= 1;
  result |= digitalRead(LCD_D6);
  result <<= 1;
  result |= digitalRead(LCD_D5);
  result <<= 1; 
  result |= digitalRead(LCD_D4); 
  result <<= 1;
  result |= digitalRead(LCD_D3);
  result <<= 1;
  result |= digitalRead(LCD_D2); 
  result <<= 1;
  result |= digitalRead(LCD_D1);
  result <<= 1;
  result |= digitalRead(LCD_D0); 
  
  return result;
}

void lcdSetWriteDir() {
  pinMode(LCD_D0, OUTPUT);
  pinMode(LCD_D1, OUTPUT);
  pinMode(LCD_D2, OUTPUT);
  pinMode(LCD_D3, OUTPUT);  
  pinMode(LCD_D4, OUTPUT);  
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D7, OUTPUT);  
}


void lcdSetReadDir() {
  pinMode(LCD_D0, INPUT);
  pinMode(LCD_D1, INPUT);
  pinMode(LCD_D2, INPUT);
  pinMode(LCD_D3, INPUT);  
  pinMode(LCD_D4, INPUT);  
  pinMode(LCD_D5, INPUT);
  pinMode(LCD_D6, INPUT);
  pinMode(LCD_D7, INPUT);    
}

void lcdWriteData(uint16_t data) {
  
  lcdSetWriteDir();
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);
  
  lcdWrite8(data >> 8);
  
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  
  lcdWrite8(data);
  
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  
  digitalWrite(LCD_CS, HIGH);  
}


void lcdWriteCommand(uint16_t command) {
  lcdSetWriteDir(); 
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);  
  lcdWrite8(command >> 8);
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  lcdWrite8(command);
  digitalWrite(LCD_WR, LOW);
  delayMicroseconds(10);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_CS, HIGH);    
}


int lcdReadData() {
  uint16_t result;
  lcdSetReadDir();
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);
  
  digitalWrite(LCD_RD, LOW);  
  delayMicroseconds(10);
  result = lcdRead8() << 8;
  digitalWrite(LCD_RD, HIGH);
  
  delayMicroseconds(10);
  
  digitalWrite(LCD_RD, LOW);
  delayMicroseconds(10);
  result |= lcdRead8();
  
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_CS, HIGH);
  
  return result;
}


void lcdWriteRegister(uint16_t addr, uint16_t data) {
  lcdWriteCommand(addr);
  lcdWriteData(data);
}

uint16_t lcdReadRegister(uint16_t reg) {
  lcdWriteCommand(reg);
  return lcdReadData();
}

uint16_t lcdReadID() {
  return lcdReadRegister(0x00);
}


void lcdFillScreen(uint16_t color) {
  
  //Serial.println("Filling the screen...");
  
  /*lcdWriteRegister(0x0050, 0);
  lcdWriteRegister(0x0051, 219);  
  lcdWriteRegister(0x0052, 0);
  lcdWriteRegister(0x0053, 319);  
  */
  lcdWriteRegister(0x0020, 0);
  lcdWriteRegister(0x0021, 0);
  lcdWriteCommand(0x0022);
  
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_WR, HIGH);
 
  lcdSetWriteDir();
  
  uint32_t i = 320;
  i *=240;

  while( i-- ) {
    lcdWrite8(color >> 8);
	WR_STROBE;
	/*
    digitalWrite(LCD_WR, LOW);
    //delayMicroseconds(10);
    digitalWrite(LCD_WR, HIGH); 
	*/
    lcdWrite8(color);
	WR_STROBE;
	/*
    digitalWrite(LCD_WR, LOW);
    //delayMicroseconds(10);
    digitalWrite(LCD_WR, HIGH); 
    */
  }
  
  digitalWrite(LCD_CS, HIGH); 
 // Serial.println("Done filling...");
  
}

void lcdReset(void)
{
  digitalWrite(LCD_RESET, LOW);
  delay(2); 
  digitalWrite(LCD_RESET, HIGH);
  lcdWriteData(0);
  lcdWriteData(0);
  lcdWriteData(0);
  lcdWriteData(0);
}


void lcdRegisterOther() {
  //Serial.println("Loading LCD registers...");
  lcdWriteRegister(0x00e5,0x8000);
  lcdWriteRegister(0x0000,0x0001);
  lcdWriteRegister(0x0001,0x0100);
  lcdWriteRegister(0x0002,0x0700);
  lcdWriteRegister(0x0003,0x1030);
  lcdWriteRegister(0x0004,0x0000);
  lcdWriteRegister(0x0008,0x0202);
  lcdWriteRegister(0x0009,0x0000);
  lcdWriteRegister(0x000a,0x0000);
  lcdWriteRegister(0x000c,0x0000);
  lcdWriteRegister(0x000d,0x0000);
  lcdWriteRegister(0x000f,0x0000);
  lcdWriteRegister(0x0010,0x0000);
  lcdWriteRegister(0x0011,0x0000);
  lcdWriteRegister(0x0012,0x0000);
  lcdWriteRegister(0x0013,0x0000);
  lcdWriteRegister(0x0010,0x17b0);
  lcdWriteRegister(0x0011,0x0037);
  lcdWriteRegister(0x0012,0x0138);
  lcdWriteRegister(0x0013,0x1700);
  lcdWriteRegister(0x0029,0x000d);
  lcdWriteRegister(0x0020,0x0000);
  lcdWriteRegister(0x0021,0x0000);
  lcdWriteRegister(0x0030,0x0001);
  lcdWriteRegister(0x0031,0x0606);
  lcdWriteRegister(0x0032,0x0304);
  lcdWriteRegister(0x0033,0x0202);
  lcdWriteRegister(0x0034,0x0202);
  lcdWriteRegister(0x0035,0x0103);
  lcdWriteRegister(0x0036,0x011d);
  lcdWriteRegister(0x0037,0x0404);
  lcdWriteRegister(0x0038,0x0404);
  lcdWriteRegister(0x0039,0x0404);
  lcdWriteRegister(0x003c,0x0700);
  lcdWriteRegister(0x003d,0x0a1f);
  lcdWriteRegister(0x0050,0x0000);
  lcdWriteRegister(0x0051,0x00ef);
  lcdWriteRegister(0x0052,0x0000);
  lcdWriteRegister(0x0053,0x013f);
  lcdWriteRegister(0x0060,0x2700);
  lcdWriteRegister(0x0061,0x0001);
  lcdWriteRegister(0x006a,0x0000);
  lcdWriteRegister(0x0090,0x0010);
  lcdWriteRegister(0x0092,0x0000);
  lcdWriteRegister(0x0093,0x0003);
  lcdWriteRegister(0x0095,0x0101);
  lcdWriteRegister(0x0097,0x0000);
  lcdWriteRegister(0x0098,0x0000);
  lcdWriteRegister(0x0007,0x0021);
  lcdWriteRegister(0x0007,0x0031);
  lcdWriteRegister(0x0007,0x0173);
}

void lcdInit(void)
{
  pinMode(LCD_CS, OUTPUT);
  digitalWrite(LCD_CS, HIGH);
  pinMode(LCD_RS, OUTPUT);
  digitalWrite(LCD_RS, HIGH);
  pinMode(LCD_WR, OUTPUT);
  digitalWrite(LCD_WR, HIGH);
  pinMode(LCD_RD, OUTPUT);
  digitalWrite(LCD_RD, HIGH);
  pinMode(LCD_RESET, OUTPUT);
  digitalWrite(LCD_RESET, HIGH);  
}

#endif


void lcdTest(void)
{
	delay(500);
	lcdInit();
	lcdReset();
	delay(500);


	uint16_t id = lcdReadID();
//  sprintf(hexString, "0x%0.4X", id); 

	lcdRegisterOther();
	lcdFillScreen(0x0000);
}


void lcdTestLoop(void)
{
	lcdFillScreen(0xf800); delay(1000);
	lcdFillScreen(0x07e0); delay(1000);
	lcdFillScreen(0x001f); delay(1000);
}

