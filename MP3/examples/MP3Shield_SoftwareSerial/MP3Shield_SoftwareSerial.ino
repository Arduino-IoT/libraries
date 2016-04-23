#include <SoftwareSerial.h>
#include <MP3.h>

/** define mp3 class */
MP3 mp3;

void setup()
{
  /** begin function */
  mp3.begin(MP3_SOFTWARE_SERIAL);    // select software serial
//  mp3.begin();                       // select hardware serial(or mp3.begin(MP3_HARDWARE_SERIAL);)
  
  /** set volum to the MAX */
  mp3.volume(0x1F);
  
  /** set MP3 Shield CYCLE mode */
  mp3.set_mode(MP3::CYCLE);
  
  /** play music in sd, '0001' for first music */
  mp3.play_sd(0x0001);
  
  /** play music in USB-disk */ 
  //mp3.play_usb_disk(0x0001);
  
  /** play music in SPI FLASH */ 
  //mp3.play_spi_flash(0x0001);
}

void loop()
{
	/** function code here */
}
