/*
 * Read logfile - demo of pathnames and current working directory
 */
#include <SdFat.h>

// file system object
SdFat sd;

SdFile file;
//------------------------------------------------------------------------------
void setup() {
  int c;
  Serial.begin(9600);

  // Initialize SdFat - print detailed message for failure
  if (!sd.init()) sd.initErrorHalt();

  // set current working directory
  if (!sd.chdir("LOGS/2011/JAN/")) {
    sd.errorHalt("chdir failed. Did you run eventlog.pde?");
  }
  // open file in current working directory
  if (!file.open("LOGFILE.TXT", O_READ)) sd.errorHalt("open failed");

  // copy the file to Serial
  while ((c = file.read()) >= 0) Serial.write(c);

  Serial.println();
  Serial.println("Done");
}
//------------------------------------------------------------------------------
void loop() {}
