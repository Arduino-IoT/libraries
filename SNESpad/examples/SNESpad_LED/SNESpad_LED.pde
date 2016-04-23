/* 
   this example from the SNESpad Arduino library
   shows the buttons pressed on LEDs, including a
   few ways to check whether a button is pressed
   rahji@rahji.com
   
   Version: 1.3 (11/12/2010) - updated to fix a bug in the last example
   
*/

#include <SNESpad.h>

// put your own strobe/clock/data pin numbers here -- see the pinout in readme.txt
SNESpad nintendo = SNESpad(2,3,4);

int state = 0;

void setup() {
  pinMode(5,  OUTPUT);
  pinMode(6,  OUTPUT);
  pinMode(7,  OUTPUT);
  pinMode(8,  OUTPUT);
  pinMode(9,  OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);    
}

void loop() {
  
  state = nintendo.buttons();

  digitalWrite(5,  state & SNES_A );      // state & SNES_A is equal to true/HIGH if the A button is pressed
  digitalWrite(6,  state & SNES_B );      // ditto for the B button ...
  digitalWrite(7,  state & SNES_SELECT );
  digitalWrite(8,  state & SNES_START );
  digitalWrite(9,  state & SNES_UP );
  digitalWrite(10, state & SNES_DOWN );
  digitalWrite(11, state & SNES_LEFT );
  digitalWrite(12, state & SNES_RIGHT );

  delay(1);

  /*
    // a way to test if a button is pressed:
    if (state & SNES_A) ...

    // a way to test if a button is not pressed:
    if !(state & SNES_A) ...

    // and a way to check for diagonal directions:
    // (ie: the UP button is pressed AND/&& the LEFT button is also pressed)
    
    if ( (state & SNES_UP) && (state & SNES_LEFT) ) ...

  */

}
