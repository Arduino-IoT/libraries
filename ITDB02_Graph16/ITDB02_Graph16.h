/*
  ITDB02_Graph16.h - Arduino library support for ITDB02 LCD Board
  Copyright (C)2010-2011 Henning Karlsen. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ITead studio. You can find the latest version of the library at
  http://www.henningkarlsen.com/electronics

  This library has been made especially for the 3.2" TFT LCD Screen Module: 
  ITDB02-3.2 by ITead studio. This library has been designed to use 16bit mode, 
  and it should work with the 2.4" Module in 16bit mode as well, although I do
  not have one, so this is untested.

  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.henningkarlsen.com/electronics/contact.php

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Version:   2.0  - Aug  15 2010  - initial release
			 2.1  - Sep  30 2010  - Added Arduino Mega compatibility.
                                    Fixed a bug with CENTER and RIGHT in LANDSCAPE mode.
									Fixed a bug in printNumI and printNumF when the 
									number to be printed was 0.
			 2.2  - Oct  14 2010  - Added support for ITDB02-3.2WC
									Added drawBitmap() with its associated tool
			 2.3  - Nov  24 2010  - Added Arduino Mega2560 compatibility
									Added support for rotating text and bitmaps
			 2.4  - Jan  18 2011  - Fixed an error in the requirements
			 2.5  - Jan  30 2011  - Added loadBitmap()
									Optimized drawBitmap() when not using rotation
			 2.6  - Mar   4 2011  - Fixed a bug in printNumF when the number to be
									printed was (-)0.something
			 3.0  - Mar  19 2011  - General optimization
			 3.01 - Mar  20 2011  - Reduced memory footprint slightly
			 4.0  - Mar  27 2011  - Remade the font-system to make it more flexible
			 4.1  - Apr  19 2011  - Remade the tinyFAT integration. Moved loadBitmap 
									to the ITDB02_tinyFAT library

  (*) Initial release is v2.0 to keep it in sync with the 8bit library.
*/


#ifndef ITDB02_Graph16_h
#define ITDB02_Graph16_h

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT 0
#define LANDSCAPE 1

#define ASPECT_4x3	0
#define ASPECT_16x9	1

#include "WProgram.h"

struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
};

class ITDB02
{
	public:
		ITDB02();
		ITDB02(int RS, int WR,int CS, int RST, byte aspect=ASPECT_4x3);
		void InitLCD(byte orientation=PORTRAIT);
		void clrScr();
		void drawPixel(int x, int y);
		void drawLine(int x1, int y1, int x2, int y2);
		void fillScr(byte r, byte g, byte b);
		void drawRect(int x1, int y1, int x2, int y2);
		void drawRoundRect(int x1, int y1, int x2, int y2);
		void fillRect(int x1, int y1, int x2, int y2);
		void fillRoundRect(int x1, int y1, int x2, int y2);
		void drawCircle(int x, int y, int radius);
		void fillCircle(int x, int y, int radius);
		void setColor(byte r, byte g, byte b);
		void setBackColor(byte r, byte g, byte b);
		void print(char *st, int x, int y, int deg=0);
		void printNumI(long num, int x, int y);
		void printNumF(double num, byte dec, int x, int y);
		void setFont(uint8_t* font);
		void drawBitmap(int x, int y, int sx, int sy, unsigned int* data, int scale=1);
		void drawBitmap(int x, int y, int sx, int sy, unsigned int* data, int deg, int rox, int roy);

	protected:
		byte fcolorr,fcolorg,fcolorb;
		byte bcolorr,bcolorg,bcolorb;
		byte orient;
		long disp_y_size;
		volatile uint8_t *P_RS, *P_WR, *P_CS, *P_RST;
		uint8_t B_RS, B_WR, B_CS, B_RST;
		_current_font	cfont;

		void LCD_Writ_Bus(char VH,char VL);
		void LCD_Write_COM(char VL);
		void LCD_Write_DATA(char VH,char VL);
		void main_W_com_data(char com1,int dat1);
		void setPixel(byte r,byte g,byte b);
		void drawHLine(int x, int y, int l);
		void drawVLine(int x, int y, int l);
		void printChar(byte c, int x, int y);
		void setXY(word x1, word y1, word x2, word y2);
		void rotateChar(byte c, int x, int y, int pos, int deg);
};

#endif