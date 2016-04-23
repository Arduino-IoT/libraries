// LCD5110_NumberFonts (C)2013 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of the included number-fonts,
// and how to use them.
//
// This program requires a Nokia 5110 LCD module.
//
// It is assumed that the LCD module is connected to
// the following pins.
//      SCK  - Pin 8
//      MOSI - Pin 9
//      DC   - Pin 10
//      RST  - Pin 11
//      CS   - Pin 12
//

#include <LCD5110_Basic.h>

LCD5110 myGLCD(8,9,10,11,12);
extern unsigned char SmallFont[];
extern unsigned char MediumNumbers[];
extern unsigned char BigNumbers[];

void setup()
{
  myGLCD.InitLCD();
}

void loop()
{
  for (int i=0; i<=10000; i++)
  {
    myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(float(i)/3, 2, RIGHT, 0);
    myGLCD.setFont(BigNumbers);
    myGLCD.printNumI(i, RIGHT, 24);
  }
  
  myGLCD.setFont(SmallFont);
  myGLCD.print("|            |", CENTER, 16);
  delay(500);
  for (int i=0; i<12; i++)
  {
    myGLCD.print("\\", 6+(i*6), 16);
    delay(500);
  }
  myGLCD.clrScr();
}
