/*************************************************************************
* File Name          : Indicators.ino
* Author             : Evan
* Updated            : Evan
* Version            : V0.4.1
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

void loop()
{
  byte red = 60;
  byte green = 30;
  byte blue = 80;
  byte flSpeed = 1;
  byte count = random(1,60); // generates random numbers
  ledStripDriver.indicators(count, red, green, blue, flSpeed);
  delay(150);
}

