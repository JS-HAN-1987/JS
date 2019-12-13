

#define OC_TFT
#define WORKAROUND_FOR_ILI9325
#define UI_V2

#include "oc_tft.h"
#include "oc_icons.h"



void setup() {
  // put your setup code here, to run once:

  tft_init();
  _delay_ms(1000);  // wait 1sec to display the splash screen
  tft_on();
  
  tft_fillrect(0, 0, 320, 240, BLACK);
  
  //tft_fillscreen(BLACK);
  //tft_printf(int x, int y, int flags, int fcolor, int bcolor, char *oldstr, char *fmt, ...)
  #define START_X 20
  #define START_Y 10
  #define Y_GAP   30


  tft_setbitmapcolor(WHITE, BLACK);
  tft_drawbitmap(0, 0, 0, icon12x16_jogright);

  
  tft_printf(START_X, START_Y, 0, WHITE, BLACK, (char *)0, (char *)"1234");
  tft_printf(START_X, START_Y+Y_GAP*1, 0, WHITE, BLACK, (char *)0, (char *)"5678");
}

void loop() {
  // put your main code here, to run repeatedly:

}
