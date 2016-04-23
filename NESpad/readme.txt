NESpad 1.2

Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your
Arduino/hardware/libraries folder.


Building
--------------------------------------------------------------------------------

After this library is installed, you just have to start the Arduino application.
You may see a few warning messages as it's built.

To use this library in a sketch, go to the Sketch | Import Library menu and
select NESpad.  This will add a corresponding line to the top of your sketch:
#include <NESpad.h>

To stop using this library, delete that line from your sketch.

Geeky information:
After a successful build of this library, a new file named "NESpad.o" will appear
in "Arduino/hardware/libraries/NESpad". This file is the built/compiled library
code.

If you choose to modify the code for this library (i.e. "NESpad.cpp" or "NESpad.h"),
then you must first 'unbuild' this library by deleting the "NESpad.o" file. The
new "NESpad.o" with your code will appear after the next press of "verify"


Hardware
--------------------------------------------------------------------------------

NES Joystick connector pinout:
  
                  (1) GND    <- Brown
White -> +5V (5)  (2) CLOCK  <- Red
          NC (6)  (3) STROBE <- Orange
          NC (7)  (4) DATA   <- Yellow


More info at http://code.google.com/p/nespad
