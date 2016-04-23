/*
 * Sketch to compare size of SdFat V2 with Arduino SD library.
 * See SD_Size.pde for Arduino SD sketch.
 */
#include <SdFat.h>

SdFat sd;

SdFile file;
//------------------------------------------------------------------------------
void setup() {
 Serial.begin(9600);
 
 if (!sd.init()) {
   Serial.println("init failed");
   return;
 }
 file.open("SIZE_TST.TXT", O_WRITE | O_CREAT | O_APPEND);
 
 file.println("Hello");
 
 file.close();
}
//------------------------------------------------------------------------------
void loop() {}
