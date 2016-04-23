/* 
   this example from the SNESpad Arduino library
   displays the buttons on the joystick as bits
   on the serial port - rahji@rahji.com
   
   Version: 1.3 (11/12/2010) - got rid of shortcut constructor - seems to be broken
   
*/

#include <SNESpad.h>

// put your own strobe/clock/data pin numbers here -- see the pinout in readme.txt
SNESpad nintendo = SNESpad(2,3,4);

int state = 0;

void setup() {
  Serial.begin(57600);  
}

void loop() {
  
  state = nintendo.buttons();

  // shows the shifted bits from the joystick
  // buttons are high (1) when up 
  // and low (0) when pressed
  Serial.println(~state, BIN);

  delay(500);
}
