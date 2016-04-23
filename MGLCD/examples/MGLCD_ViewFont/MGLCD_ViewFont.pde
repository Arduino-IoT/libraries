// MGLCD_ViewFont (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of the included fonts.
//
// This program requires a compatible Monochrome GLCD module.
//

#include <MGLCD.h>

MGLCD myGLCD(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

extern uint8_t SmallFont[];
extern uint8_t WideFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

void setup()
{
  myGLCD.initLCD();
}

void loop()
{
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print("SmallFont:", LEFT, 0);
  myGLCD.print(" !\"#$%&'()*+,-./", CENTER, 16);
  myGLCD.print("0123456789:;<=>?", CENTER, 24);
  myGLCD.print("@ABCDEFGHIJKLMNO", CENTER, 32);
  myGLCD.print("PQRSTUVWXYZ[\\]^_", CENTER, 40);
  myGLCD.print("`abcdefghijklmno", CENTER, 48);
  myGLCD.print("pqrstuvwxyz{|}~ ", CENTER, 56);
  delay (5000);
  
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print("WideFont:", LEFT, 0);
  myGLCD.setFont(WideFont);
  myGLCD.print(" !\"#$%&'()*+,-./", CENTER, 16);
  myGLCD.print("0123456789:;<=>?", CENTER, 24);
  myGLCD.print("@ABCDEFGHIJKLMNO", CENTER, 32);
  myGLCD.print("PQRSTUVWXYZ[\\]^_", CENTER, 40);
  myGLCD.print("`abcdefghijklmno", CENTER, 48);
  myGLCD.print("pqrstuvwxyz{|}~ ", CENTER, 56);
  delay (5000);
  
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print("MediumNumbers:", LEFT, 0);
  myGLCD.setFont(MediumNumbers);
  myGLCD.print("0123456789", CENTER, 32);
  delay (5000);
  
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print("BigNumbers:", LEFT, 0);
  myGLCD.setFont(BigNumbers);
  myGLCD.print("01234", CENTER, 16);
  myGLCD.print("56789", CENTER, 40);
  delay (5000);
}
