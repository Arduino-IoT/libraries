/*
 * Example of getline from section 27.7.1.3 of the C++ standard
 */
#include <SdFat.h>

// file system object
SdFat sd;

// create a serial stream
ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------
void makeTestFile() {
  ofstream sdout("GETLINE.TXT");
  // use flash for text to save RAM
  sdout << pstr(
    "short line\n"
    "\n" 
    "17 character line\n"
    "too long for buffer\n"
    "line with no nl");
}
//------------------------------------------------------------------------------
void testGetline() {
  const int line_buffer_size = 18;
  char buffer[line_buffer_size];
  ifstream sdin("GETLINE.TXT");
  int line_number = 0;
  
  while (sdin.getline(buffer, line_buffer_size, '\n') || sdin.gcount()) {
    int count = sdin.gcount();
    if (sdin.fail()) {
      cout << "Partial long line";
      sdin.clear(sdin.rdstate() & ~ios_base::failbit);    
    } else if (sdin.eof()) {
      cout << "Partial final line"; // sdin.fail() is false
    } else {
      count--; // Don’t include newline in count
      cout << "Line " << ++line_number;
    }
    cout << " (" << count << " chars): " << buffer << endl;
  }
}
//------------------------------------------------------------------------------
void setup(void) {
  Serial.begin(9600);
  
  // pstr stores strings in flash to save RAM
  cout << pstr("Type any character to start\n");
  while(!Serial.available());
  
  // initialize the file system
  if (!sd.init()) sd.initErrorHalt();
  
  // make the test file
  makeTestFile();
  
  // run the example
  testGetline();
}
//------------------------------------------------------------------------------
void loop(void) {}
