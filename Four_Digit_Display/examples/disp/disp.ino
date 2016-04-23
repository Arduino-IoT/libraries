#include <Wire.h>
#include <SoftwareSerial.h>
#include <Suli.h>
#include <Four_Digit_Display_Arduino.h>


Four_Digit_Display_Arduino    disp;


void setup()
{
    disp.begin(2,3);
    disp.display(1, 5);
}


void loop()
{

}