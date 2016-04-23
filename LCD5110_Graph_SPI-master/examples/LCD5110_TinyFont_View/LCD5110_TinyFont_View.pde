// LCD5110_TinyFont_View (C)2013 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// Modified by Alexey Stepanov (Hardware SPI Support)
//
// This program is a demo of the tiniest font.
//
// This program requires a Nokia 5110 LCD module.
//
// It is assumed that the LCD module is connected to
// the following pins using a levelshifter to get the
// correct voltage to the module.
//      SCK  - Pin 13
//      MOSI - Pin 11
//      DC   - Pin 5
//      RST  - Pin 6
//      CS   - Pin 3
//
#include <SPI.h>
#include <LCD5110_Graph_SPI.h>

LCD5110 myGLCD(5,6,3);

extern uint8_t TinyFont[];

void setup()
{
  myGLCD.InitLCD();
  myGLCD.setFont(TinyFont);
}

void loop()
{
  myGLCD.print(" !\"#$%&'()*+,-./", CENTER, 0);
  myGLCD.print("0123456789:;<=>?", CENTER, 6);
  myGLCD.print("@ABCDEFGHIJKLMNO", CENTER, 12);
  myGLCD.print("PQRSTUVWXYZ[\\]^_", CENTER, 18);
  myGLCD.print("`abcdefghijklmno", CENTER, 24);
  myGLCD.print("pqrstuvwxyz{|}~ ", CENTER, 30);
  myGLCD.update();
  
  while (1);
}
