#include <Servo.h>
#include "pitches.h" 
#include "IO_pins.h"

// define constants
byte LRscalefactor=6;
byte UDscalefactor=6;
int distancemax=200;
int bestdistance=550;
int neckLRcenter=1490;
int neckUDcenter=1300;
int leftmotorstop=1470;
int rightmotorstop=1455;
int LRmax=neckLRcenter+700;
int LRmin=neckLRcenter-700;
int UDmax=neckUDcenter+700;
int UDmin=neckUDcenter-200;

// define global variables
long time;
byte boredom;
int pan=neckLRcenter;
int tilt=neckUDcenter;
int panscale;
int tiltscale;
int leftspeed=leftmotorstop;
int rightspeed=rightmotorstop;
byte lightchase;
byte edge;
byte edgesensors;
int distance;
int temp;
int updown;
int leftright;
int leftIRvalue;
int rightIRvalue;
int upIRvalue;
int downIRvalue;
int safedistance=200;
int leftfrontsen;
int leftrearsen;
int rightrearsen;
int rightfrontsen;

Servo leftmotor;
Servo rightmotor;
Servo neckleftright;
Servo neckupdown;


void setup()
{
  // initialize servos and configure pins
  leftmotor.attach(leftmotorpin);
  leftmotor.writeMicroseconds(leftmotorstop);
  rightmotor.attach(rightmotorpin);
  rightmotor.writeMicroseconds(rightmotorstop);
  neckleftright.attach(neckleftrightpin);
  neckleftright.writeMicroseconds(neckLRcenter);
  neckupdown.attach(neckupdownpin);
  neckupdown.writeMicroseconds(neckUDcenter);

  pinMode (IRleds,OUTPUT);
  pinMode (Speaker,OUTPUT);
  pinMode (leftfrontLED,OUTPUT);
  pinMode (leftrearLED,OUTPUT);
  pinMode (rightrearLED,OUTPUT);
  pinMode (rightfrontLED,OUTPUT);

  // play tune on powerup / reset
  int melody[] = {NOTE_C4,NOTE_G3,NOTE_G3,NOTE_A3,NOTE_G3,0,NOTE_B3,NOTE_C4};
  int noteDurations[] = {4,8,8,4,4,4,4,4};
  for (byte Note = 0; Note < 8; Note++)
  {
    int noteDuration = 1000/noteDurations[Note];
    tone(Speaker,melody[Note],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }
}

void loop()
{
  if (millis()-time>249)                                      // has 250mS elapsed (1/4 of a second)
  {
    time=millis();                                            // update time
    lightchase=lightchase+1-4*(lightchase>3);                 // change LED pattern every 250mS
  }
  leftmotor.writeMicroseconds(leftspeed);                     // update the speed of the left motor
  rightmotor.writeMicroseconds(rightspeed);                   // update the speed of the right motor
  neckleftright.writeMicroseconds(pan);                       // update the position of the pan servo
  neckupdown.writeMicroseconds(tilt);                         // update the position of the tilt servo

  IReye();                                                    // read the eye sensors
  IRfollow();                                                 // move the head and body to follow an object
  ObjectDetection();                                          // read corner sensors, turn on corner LEDs and prevent collisions
}

byte noise;                                                   
void IReye()//===============================================================Read IR compound eye================================================
{

  digitalWrite(IRleds,HIGH);                                  // turn on IR LEDs to read TOTAL IR LIGHT (ambient + reflected)
  delay(2);                                                   // Allow time for phototransistors to respond. (may not be needed)
  leftIRvalue=analogRead(IRleft);                             // TOTAL IR = AMBIENT IR + LED IR REFLECTED FROM OBJECT
  rightIRvalue=analogRead(IRright);                           // TOTAL IR = AMBIENT IR + LED IR REFLECTED FROM OBJECT
  upIRvalue=analogRead(IRup);                                 // TOTAL IR = AMBIENT IR + LED IR REFLECTED FROM OBJECT
  downIRvalue=analogRead(IRdown);                             // TOTAL IR = AMBIENT IR + LED IR REFLECTED FROM OBJECT

  digitalWrite(IRleds,LOW);                                   // turn off IR LEDs to read AMBIENT IR LIGHT (IR from indoor lighting and sunlight)
  delay(2);                                                   // Allow time for phototransistors to respond. (may not be needed)
  leftIRvalue=leftIRvalue-analogRead(IRleft);                 // REFLECTED IR = TOTAL IR - AMBIENT IR
  rightIRvalue=rightIRvalue-analogRead(IRright);              // REFLECTED IR = TOTAL IR - AMBIENT IR
  upIRvalue=upIRvalue-analogRead(IRup);                       // REFLECTED IR = TOTAL IR - AMBIENT IR
  downIRvalue=downIRvalue-analogRead(IRdown);                 // REFLECTED IR = TOTAL IR - AMBIENT IR

  distance=(leftIRvalue+rightIRvalue+upIRvalue+downIRvalue)/4;// distance of object is average of reflected IR

  noise++;                                                    // count program loops
  if(noise>7)
  {
    tone(Speaker,distance*5+100,5);                           // produce sound every eighth loop - high pitch = close distance
    noise=0;
  }
}

void IRfollow ()//==============================================Track object in range================================================================
{

  leftspeed=leftmotorstop;                                    // start with motors set to a speed of 0
  rightspeed=rightmotorstop;

  if (distance<distancemax)                                   // if there is no object in range 
  {
    if (pan>neckLRcenter)pan=pan-5;
    if (pan<neckLRcenter)pan=pan+5;
    if (tilt>neckUDcenter)tilt=tilt-5;
    if (tilt<neckUDcenter)tilt=tilt+5;
  }
  else
  {
    //-------------------------------------------------------------Track object with head------------------------------------------------
    panscale=(leftIRvalue+rightIRvalue)/LRscalefactor;
    tiltscale=(upIRvalue+downIRvalue)/UDscalefactor;
    if (leftIRvalue>rightIRvalue)
    {
      leftright=(leftIRvalue-rightIRvalue)*5/panscale;
      pan=pan-leftright;
    }
    if (leftIRvalue<rightIRvalue)
    {
      leftright=(rightIRvalue-leftIRvalue)*5/panscale;
      pan=pan+leftright;
    }
    if (upIRvalue>downIRvalue)
    {
      updown=(upIRvalue-downIRvalue)*5/tiltscale;
      tilt=tilt-updown;
    }
    if (downIRvalue>upIRvalue)
    {
      updown=(downIRvalue-upIRvalue)*5/tiltscale;
      tilt=tilt+updown;
    }
    constrain(pan,LRmin,LRmax);
    constrain(tilt,UDmin,UDmax); 

    //-------------------------------------------------------------Turn body to follow object--------------------------------------------
    temp=LRmax-pan;
    if (temp<600)
    {
      leftspeed=leftmotorstop-600+temp;
      rightspeed=rightmotorstop-600+temp;
    }
    temp=pan-LRmin;
    if (temp<600)
    {
      rightspeed=rightmotorstop+600-temp;
      leftspeed=leftmotorstop+600-temp;
    }

    //------------------------------------------------------Move forward or backward to follow object------------------------------------
    temp=distance-bestdistance;
    temp=abs(temp);

    if (temp>10)
    {
      temp=temp-10;
      if (distance>bestdistance)
      {
        rightspeed=rightspeed-temp;
        leftspeed=leftspeed+temp;
      }
      else
      {
        rightspeed=rightspeed+temp;
        leftspeed=leftspeed-temp;
      }
    }
  }
}

//============================================================Avoid hitting objects==================================================

void ObjectDetection()
{
  //turn on edge detection LEDs
  digitalWrite(leftfrontLED,1);
  digitalWrite(leftrearLED,1);
  digitalWrite(rightrearLED,1);
  digitalWrite(rightfrontLED,1);

  // read total IR values
  leftfrontsen=analogRead(leftfrontsenpin);
  leftrearsen=analogRead(leftrearsenpin);
  rightrearsen=analogRead(rightrearsenpin);
  rightfrontsen=analogRead(rightfrontsenpin);

  // turn off edge detection LEDs
  digitalWrite(leftfrontLED,0);
  digitalWrite(leftrearLED,0);
  digitalWrite(rightrearLED,0);
  digitalWrite(rightfrontLED,0);

  // subtract ambient IR from total IR to give reflected IR values
  leftfrontsen=leftfrontsen-analogRead(leftfrontsenpin);
  leftrearsen=leftrearsen-analogRead(leftrearsenpin);
  rightrearsen=rightrearsen-analogRead(rightrearsenpin);
  rightfrontsen=rightfrontsen-analogRead(rightfrontsenpin);

  // turn on indicator LED if object closer than safe distance otherwise chase LEDs
  digitalWrite(leftfrontLED,(lightchase==1 || leftfrontsen>safedistance));
  digitalWrite(leftrearLED,(lightchase==2 || leftrearsen>safedistance));
  digitalWrite(rightrearLED,(lightchase==3 || rightrearsen>safedistance));
  digitalWrite(rightfrontLED,(lightchase==4 || rightfrontsen>safedistance));

  // Adjust motor speeds to avoid collision
  if (leftfrontsen>safedistance && leftspeed<leftmotorstop) leftspeed=leftmotorstop;
  if (leftrearsen>safedistance && leftspeed>leftmotorstop) leftspeed=leftmotorstop;
  if (rightrearsen>safedistance && rightspeed<rightmotorstop) rightspeed=rightmotorstop;
  if (rightfrontsen>safedistance && rightspeed>rightmotorstop) rightspeed=rightmotorstop;
}

















