/*************************************************************************
* File Name          : BackAndForth.ino
* Author             : Evan
* Updated            : Evan
* Version            : V0.5.0
* Date               : 10/08/2013
* Description        : Test for Makeblock Electronic modules of Me_Stepper 
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

MeStepperMotor stepperDriver(PORT_1);

void setup()
{
  stepperDriver.begin(); // initialize stepper driver.
  stepperDriver.enable(); // enable stepper driver, Keep the micro step current position.
  stepperDriver.setMicroStep(STP_QUARTER); // set micro step. STP_FULL, STP_HALF, STP_QUARTER, STP_EIGHTH, STP_SIXTEENTH
  stepperDriver.setMaxSpeed(5000);         // set max speed. 1-10000
  stepperDriver.setAcceleration(120000);   // set acceleration. 1-200000
  stepperDriver.run(); // output pulse
}

void loop()
{
  long stp = stepperDriver.currentPosition(); // get the current position
  if(stp > 3000) // set the limit
    stepperDriver.moveTo(0); // Set the target position. Negative is anticlockwise from the 0 position.
  if(stp < 1) // set the limit
    stepperDriver.moveTo(3001); // set the target position.
}

