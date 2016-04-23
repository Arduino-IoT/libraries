
Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your
Arduino/hardware/libraries folder.


Building
--------------------------------------------------------------------------------

After this library is installed, you just have to start the Arduino application.
You may see a few warning messages as it's built.

To use this library in a sketch, go to the Sketch | Import Library menu and
select SNESpad.  This will add a corresponding line to the top of your sketch:
#include <SNESpad.h>

To stop using this library, delete that line from your sketch.

Geeky information:
After a successful build of this library, a new file named "SNESpad.o" will appear
in "Arduino/hardware/libraries". This file is the built/compiled library
code.

If you choose to modify the code for this library (i.e. "SNESpad.cpp" or "SNESpad.h"),
then you must first 'unbuild' this library by deleting the "SNESpad.o" file. The
new "SNESpad.o" with your code will appear after the next press of "verify"


Hardware
--------------------------------------------------------------------------------

SNES Joystick connector pinout:
  
 (1) +5V
 (2) CLOCK
 (3) STROBE
 (4) DATA
 (5) NC
 (6) NC
 (7) GND

More info at http://code.google.com/p/nespad