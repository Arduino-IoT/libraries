/*
  LCD5110_Basic.h - Arduino/chipKit library support for Nokia 5110 compatible LCDs
  Copyright (C)2013 Henning Karlsen. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ITead studio. You can find the latest version of the library at
  http://www.henningkarlsen.com/electronics

  This library has been made to make it easy to use the basic functions of
  the Nokia 5110 LCD module on an Arduino or a chipKit.

  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.henningkarlsen.com/electronics/contact.php

  This library is licensed under a CC BY-NC-SA 3.0 (Creative Commons 
  Attribution-NonCommercial-ShareAlike 3.0 Unported) License.
  For more information see: http://creativecommons.org/licenses/by-nc-sa/3.0/
*/

#ifndef LCD5110_Basic_h
#define LCD5110_Basic_h

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define LCD_COMMAND 0
#define LCD_DATA 1

// PCD8544 Commandset
// ------------------
// General commands
#define PCD8544_POWERDOWN			0x04
#define PCD8544_ENTRYMODE			0x02
#define PCD8544_EXTENDEDINSTRUCTION	0x01
#define PCD8544_DISPLAYBLANK		0x00
#define PCD8544_DISPLAYNORMAL		0x04
#define PCD8544_DISPLAYALLON		0x01
#define PCD8544_DISPLAYINVERTED		0x05
// Normal instruction set
#define PCD8544_FUNCTIONSET			0x20
#define PCD8544_DISPLAYCONTROL		0x08
#define PCD8544_SETYADDR			0x40
#define PCD8544_SETXADDR			0x80
// Extended instruction set
#define PCD8544_SETTEMP				0x04
#define PCD8544_SETBIAS				0x10
#define PCD8544_SETVOP				0x80
// Display presets
#define LCD_BIAS					0x03	// Range: 0-7 (0x00-0x07)
#define LCD_TEMP					0x02	// Range: 0-3 (0x00-0x03)
#define LCD_CONTRAST				0x46	// Range: 0-127 (0x00-0x7F)

#if defined(__AVR__)
	#include "Arduino.h"
	#include "hardware/avr/HW_AVR_defines.h"
#elif defined(__PIC32MX__)
	#include "WProgram.h"
	#include "hardware/pic32/HW_PIC32_defines.h"
#elif defined(__arm__)
	#include "Arduino.h"
	#include "hardware/arm/HW_ARM_defines.h"
#endif

struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
	uint8_t inverted;
};

class LCD5110
{
	public:
		LCD5110(int SCK, int MOSI, int DC, int RST, int CS);
		void InitLCD(int contrast=LCD_CONTRAST);
		void setContrast(int contrast);
		void clrScr();
		void clrRow(int row, int start_x = 0, int end_x = 83);
		void invert(bool mode);
		void invertText(bool mode);
		void print(char *st, int x, int y);
		void print(String st, int x, int y);
		void printNumI(long num, int x, int y, int length=0, char filler=' ');
		void printNumF(double num, byte dec, int x, int y, char divider='.', int length=0, char filler=' ');
		void setFont(uint8_t* font);
		void drawBitmap(int x, int y, bitmapdatatype bitmap, int sx, int sy);

	protected:
		regtype *P_SCK, *P_MOSI, *P_DC, *P_RST, *P_CS;
		regsize B_SCK, B_MOSI, B_DC, B_RST, B_CS;
		uint8_t SCK_Pin, RST_Pin;			// Needed for for faster MCUs
		_current_font	cfont;

		void _LCD_Write(unsigned char data, unsigned char mode);
		void _print_char(unsigned char c, int x, int row);
		void _convert_float(char *buf, double num, int width, byte prec);
};

#endif