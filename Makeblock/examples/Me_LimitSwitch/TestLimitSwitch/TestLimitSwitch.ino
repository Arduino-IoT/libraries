/*************************************************************************
* File Name          : TestLimitSwitch.ino
* Author             : Evan
* Updated            : Evan
* Version            : V1.0.0
* Date               : 5/27/2013
* Description        : Example for Makeblock Electronic modules of 
                       Me-LimitSwitch. The module can only be connected to 
                       the PORT_3, PORT_4, PORT_5, and PORT_6 of Me - 
                       Base Shield.
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/
#include <Makeblock.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

MeLimitSwitch limitSwitch(PORT_4); //Me_LimitSwitch module can only be connected to PORT_3, PORT_4, PORT_5, PORT_6 of base shield.

void setup()
{
    Serial.begin(9600);
    Serial.println("Start.");
}

void loop()
{
   if(limitSwitch.touched()==1) //If the limit switch is up, the "readUpPin" return value is true.
   {
     Serial.println("State: UP.");
     delay(5);  //We have designed the hardware disappears shakes circuit, add software disappears shakes to improve reliability.
     //Delay time is usually 2 to 10 ms.
     while(limitSwitch.touched()==1); //Repeat check the switch state, until released.
   }
   if(limitSwitch.touched()==0) //If the limit switch is down, the "readDownPin" return value is true.
   {
     Serial.println("State: DOWN.");
     delay(5);
     while(limitSwitch.touched()==0);
   }
   
}

