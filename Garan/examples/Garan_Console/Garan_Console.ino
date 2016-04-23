/*
  Garan_Console

  Simple test code to play with Garan Audio Module's commands.

  Email: fox@mcuapps.com
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Garan.h"

#if !defined(__MSP430_CPU__)
SoftwareSerial garanSerial(8, 9); // RX, TX
#else
SoftwareSerial garanSerial(P1_4, P1_5); // RX, TX
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
  Serial.println("Console is ready!");
} 

void loop() {
  if (Serial.available()) {
    switch(Serial.read()) {
      case '1':
        player.singlePlay(1);
        break;
      case '2':
        player.sequencePlay(2);
        break;
      case '3':
        player.singlePlay(3);
        break;
      case '4':
        player.singlePlayName("00000001.mp3");
        break;
      case '5':
        player.sequencePlayName("00000001.mp3");
        break;
      case '6':
        player.singleLoopName("00000001.mp3");
        break;
      case '9':
        player.setVolume(15);
        break;
      case '0':
        player.setVolume(0);
        break;      
      case 'f':
        player.feedbackAtEnd();
        break;
      case 'n':
        player.next();
        break;
      case 'p':
        player.prev();
        break;
      case 't':
        player.getMusicNumbers();
        break;
      case 'v':
        player.getVersion();
        break;
      case ']':
        player.volumeUp();
        break;
      case '[':
        player.volumeDown();
        break; 
      default:
        Serial.println("This key maps to none.");     
    }
  }

  if (garanSerial.available()) {
    unsigned char recvByte = garanSerial.read();
    if (recvByte == 0x24) Serial.println();

    Serial.print(String(recvByte, HEX) + " ");
  }
}
