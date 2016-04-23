// MGLCD_Demo (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of most of the functions
// in the library.
//
// This program requires a compatible Monochrome GLCD module.
//

#include <MGLCD.h>

MGLCD myGLCD(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

extern uint8_t SmallFont[];
extern uint8_t WideFont[];
extern uint8_t arduino_logo[];
extern uint8_t The_End[];
extern uint8_t pacman1[];
extern uint8_t pacman2[];
extern uint8_t pacman3[];
extern uint8_t pill[];

float y;
uint8_t* bm;
int pacy;

void setup()
{
  myGLCD.initLCD();
  randomSeed(analogRead(7));
}

void loop()
{
  myGLCD.clrScr();
  myGLCD.drawBitmap(22, 8, arduino_logo, 84, 48);

  delay(2000);
  
  myGLCD.clrScr();
  myGLCD.setFont(WideFont);
  myGLCD.print("MGLCD", CENTER, 0);
  myGLCD.print("DEMO", CENTER, 20);
  myGLCD.drawRect(45, 18, 81, 28);
  for (int i=0; i<6; i++)
  {
    myGLCD.drawLine(82, 18+(i*2), 127-(i*3), 18+(i*2));
    myGLCD.drawLine((i*3), 28-(i*2), 45, 28-(i*2));
  }
  myGLCD.setFont(SmallFont);
  myGLCD.print("(C)2011", CENTER, 48);
  myGLCD.print("Henning Karlsen", CENTER, 56);

  delay(2000);
  
  myGLCD.clrScr();
  for (int i=0; i<64; i+=2)
  {
    myGLCD.drawLine(0, i, 127, 64-i);
  }
  for (int i=127; i>=0; i-=2)
  {
    myGLCD.drawLine(i, 0, 127-i, 63);
  }

  delay(2000);
  
  myGLCD.clrScr();
  myGLCD.drawRect(0, 0, 127, 63);
  for (int i=0; i<64; i+=4)
  {
    myGLCD.drawLine(0, i, i*2, 63);
  }
  for (int i=0; i<64; i+=4)
  {
    myGLCD.drawLine(127, 64-i, 127-(i*2), 0);
  }

  delay(2000);
  
  myGLCD.clrScr();
  for (int i=0; i<10; i++)
  {
    myGLCD.drawRoundRect(i*3, i*3, 127-(i*3), 63-(i*3));
  }

  delay(2000);
  
  myGLCD.clrScr();
  for (int i=0; i<24; i++)
  {
    myGLCD.drawCircle(63, 31, i*3);
  }

  delay(2000);
  
  myGLCD.clrScr();
  myGLCD.drawRect(0, 0, 127, 63);
  myGLCD.drawLine(0, 31, 127, 31);
  myGLCD.drawLine(63, 0, 63, 63);
  for (int c=0; c<4; c++)
  {
    for (int i=0; i<128; i++)
    {
      y=i*0.017453292519943295769236907684886;
      myGLCD.invPixel(i, (sin(y*6)*28)+31);
      delay(20);
    }
  }

  delay(2000);

  myGLCD.clrScr();
  for (int pc=0; pc<3; pc++)
  {
    pacy=random(0, 44);
  
    for (int p=7; p>0; p--)
      myGLCD.drawBitmap(p*20-8, pacy+7, pill, 5, 5);
  
    for (int i=-20; i<128; i++)
    {
      switch(((i+20)/3) % 4)
      {
        case 0: bm=pacman1;
                break;
        case 1: bm=pacman2;
                break;
        case 2: bm=pacman3;
                break;
        case 3: bm=pacman2;
                break;
      }
      myGLCD.drawBitmap(i, pacy, bm, 20, 20);
      if (i>0)
        for (int c=0; c<6; c++)
          myGLCD.clrPixel(i-1, pacy+c+7);
      delay(50);
    }
    for (int c=0; c<6; c++)
      myGLCD.clrPixel(127, pacy+c+7);
  }

  myGLCD.clrScr();
  for (int i=0; i<33; i++)
  {
    myGLCD.drawBitmap(22, i-24, The_End, 84, 24);
    delay(100);
  }
  myGLCD.print("Runtime (ms):", CENTER, 48);
  myGLCD.printNumI(millis(), CENTER, 56);
  for (int i=0; i<5; i++)
  {
    myGLCD.invert(true);
    delay(1000);
    myGLCD.invert(false);
    delay(1000);
  }
}
