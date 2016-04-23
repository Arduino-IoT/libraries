/*
  UltraSonicRanger_Reading

  Email: fox@mcuapps.com
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Garan.h"

#include "Ultrasonic.h"

#if !defined(__MSP430_CPU__)
SoftwareSerial garanSerial(8, 9); // RX, TX
Ultrasonic ultrasonic(A5);
#else
SoftwareSerial garanSerial(P1_4, P1_5); // RX, TX
Ultrasonic ultrasonic(P1_6);
#endif

Garan player(garanSerial);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 

#if !defined(__MSP430_CPU__)
  // wait for USB CDC serial port to connect.  Arduino Leonardo only
  while (!Serial) {
  }
#endif
} 

void loop() {
  long RangeInCentimeters;
  char name[13];

  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  Serial.print(RangeInCentimeters);
  Serial.println(" cm");

  sprintf(name, "%08d.mp3", RangeInCentimeters);
  player.singlePlayName(name);

  delay(3000);
}

