#include <LCD12864.h>
#include <LCDSprite.h>

void setup()
{
  LCDA.Initialise(); // Init the LCD screen, clears the garbage on top and bottom screen, immediately sets it to graphics mode 
  delay(100);
  LCDA.Render(); // Call this after Initialise() to start the rendering process 
  delay(100);
}
void loop()
{
  LCDA.DumpScreenBuffer(); //Dump the whole screen  
  //LCDA.DumpScreenBuffer(0,0,1,32); //Dump a cropped rectangle of the screen buffer
  LCDA.DrawPixel(0,128);  //(0 <= x <=128 , 128 <= y <=160)
  LCDA.DrawBrLine(0,148,20,148); //(x0, y0, x1, y1)
  LCDA.DrawCircle(40,144,15);    //(x, y, r)
  LCDA.DrawRectangle(60,128,20,31);     //(x, y, width, height)
  LCDA.FillRectangle(100,128,20,32);    //(x, y, width, height)
  //LCDA.DrawSprite(2,1);
  LCDA.RenderScreenBuffer(1); //Render the whole screen,screen (1 = top screen, 2 = bottom screen) 
  //LCDA.RenderScreenBuffer(0,0,1,32); //Render a cropped rectangle of the screen buffer
  LCDA.Duplicate(true); //Can duplicate what is shown in the first screen, onto the second screen, default is false 
  LCDA.RenderScreenBuffer(2);
  while(1);
}




