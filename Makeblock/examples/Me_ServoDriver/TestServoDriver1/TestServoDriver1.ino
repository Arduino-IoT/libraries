/*************************************************************************
* File Name          : TestServoDriver.ino
* Author             : Evan
* Updated            : Evan
* Version            : V1.0.1
* Date               : 5/17/2013
* Description        : Test for Makeblock Electronic modules of Me -Servo 
                       Driver. The module can only be connected to the 
                       port 1, 2 of Me - Base Shield. One module can drive 
                       two servos.
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/
#include <Makeblock.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

/*
Class Me_ServoDriver has most of the functions from class Servo, 
see Me_ServoDriver.h for more details.
*/
MeServo servoDriver(PORT_1,DEV1);//can ONLY be PORT_1,PORT_2

int pos1 = 0;
int pos2 = 180;
void setup()
{
    servoDriver.begin();
    //servoDriver.begin(500,2500); //set time plus of the servo
   //servoDriver.begin(1000,2000);
}

void loop()
{
    servoDriver.begin();
    servoDriver.write(pos1);
    delay(1000);  // Wait for the servo rotation to the set position
    servoDriver.write(pos2);
    delay(1000);  // Wait for the servo rotation to the set position
    // Release the servo, not locked position.next.
    servoDriver.detach();// Next control servo, must call the "begin" function again.
    delay(1000);
    
   
}
