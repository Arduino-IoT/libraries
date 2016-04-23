/*************************************************************************
* File Name          : WhiteBreathLight.ino
* Author             : Evan
* Updated            : Evan
* Version            : V0.2.1
* Date               : 10/09/2013
* Description        : Test for Makeblock Electronic modules of Me_LedStrip 
                       Driver. The module can only be connected to the 
                       PORT_1, PORT_2 of Me - Base Shield.. 
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/
#include <Makeblock.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

MeLedStrip ledStripDriver(PORT_1);

void setup()
{
  // initialize ledStrip Driver and set the led quantity.
  ledStripDriver.begin(60);
}

int bri = 0, st = 0;
void loop()
{
  if(bri==100) st=1;
  if(bri==1) st=0;
  
  if(st)bri--;
  else bri++;
  
  ledStripDriver.setPixelColor(LS_ALL_PIXEL, bri, bri, bri, LS_ONCE_FLASH); // parameter description: led number, red, green, blue, flash mode
}

