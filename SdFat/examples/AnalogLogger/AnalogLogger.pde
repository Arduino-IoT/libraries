// A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  1000  // mills between entries
#define SENSOR_COUNT     3  // number of analog pins to log
#define ECHO_TO_SERIAL   1  // echo data to serial port
#define WAIT_TO_START    1  // Wait for serial input in setup()
#define SYNC_INTERVAL 1000  // mills between calls to sync()
uint32_t syncTime = 0;      // time of last sync()

#include <SdFat.h>
#include <SdFatUtil.h>

// file system object
SdFat sd;

// binary file object
SdFile file;

// Serial print stream
ArduinoOutStream cout(Serial);

// buffer to format data - makes it eaiser to echo to Serial
char buf[50];
//------------------------------------------------------------------------------
#if SENSOR_COUNT > 6
#error SENSOR_COUNT too large
#endif  // SENSOR_COUNT
//------------------------------------------------------------------------------
// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  
  // pstr stores strings in flash to save RAM
  cout << endl << pstr("FreeRam: ") << FreeRam() << endl;

#if WAIT_TO_START
  cout << pstr("Type any character to start\n");
  while (!Serial.available());
#endif  // WAIT_TO_START

  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  // if SD chip select is not SS, the second argument to init is CS pin number
  if (!sd.init(SPI_HALF_SPEED)) sd.initErrorHalt();

  // create a new file in root, the current working directory
  char name[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    name[6] = i/10 + '0';
    name[7] = i%10 + '0';
    if (file.open(name, O_CREAT | O_EXCL | O_WRITE)) break;
  }
  if (!file.isOpen()) error("file.open");

  cout << pstr("Logging to: ") << name << endl;

  //format header in buffer
  obufstream bout(buf, sizeof(buf));
  
  bout << pstr("millis");
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    bout << ",sens" << int(i);
  }
  
#if ECHO_TO_SERIAL
  cout << buf << endl;
#endif  // ECHO_TO_SERIAL

  // add CR/LF for Windows style file (file.write is binary, not text)
  bout << "\r\n";
  file.write(buf);

  // force write of header to SD card
  file.sync();

  if (file.writeError) error("write header failed");
}
//------------------------------------------------------------------------------
void loop() {
  uint32_t m;

  // wait for time to be a multiple of interval
  do {
    m = millis();
  } while(m % LOG_INTERVAL);

  // use buffer stream to format line
  obufstream bout(buf, sizeof(buf));

  // start with time in millis
  bout << m;
  // read analog pins and format data
  for (uint8_t ia = 0; ia < SENSOR_COUNT; ia++) {
    bout << ',' << analogRead(ia);
  }
#if ECHO_TO_SERIAL
  cout << buf << endl;
#endif  // ECHO_TO_SERIAL

  // add CR/LF for Windows style file (file.write is binary, not text)
  bout << "\r\n";
  file.write(buf);

  // don't sync too often - requires 2048 bytes of I/O to SD card
  if ((millis() - syncTime) >= SYNC_INTERVAL) {
    file.sync();
    syncTime = millis();
  }

  // check for errors
  if (file.writeError) error("write data failed");

  // don't log two points in the same millis
  if (m == millis()) delay(1);
}
