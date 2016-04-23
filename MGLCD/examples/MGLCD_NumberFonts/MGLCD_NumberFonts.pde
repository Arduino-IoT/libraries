// MGLCD_NumberFonts (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of the included number-fonts,
// and how to use them.
//
// This program requires a compatible Monochrome GLCD module.
//

#include <MGLCD.h>

MGLCD myGLCD(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

extern unsigned char SmallFont[];
extern unsigned char MediumNumbers[];
extern unsigned char BigNumbers[];

void setup()
{
  myGLCD.initLCD();
}

void loop()
{
  for (int i=0; i<=10000; i++)
  {
    myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(float(i)/3, 2, RIGHT, 0);
    myGLCD.setFont(BigNumbers);
    myGLCD.invertText(true);
    myGLCD.printNumI(i, RIGHT, 40);
    myGLCD.setFont(SmallFont);
    myGLCD.printNumI(i, LEFT, 52);
  }
  
  myGLCD.setFont(SmallFont);
  myGLCD.print("|            |", CENTER, 24);
  delay(500);
  for (int i=0; i<12; i++)
  {
    myGLCD.print("\\", 28+(i*6), 24);
    delay(500);
  }
  myGLCD.clrScr();
}
