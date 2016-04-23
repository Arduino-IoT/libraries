/*
  MGLCD.h - Arduino library support for Monochrome Graphics LCDs
  Copyright (C)2011 Henning Karlsen. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ElecFreaks. You can find the latest version of the library at
  http://www.henningkarlsen.com/electronics

  This library has been made to make it easy to use Monochrome Graphics LCDs
  with an Arduino.

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
*/

#ifndef MGLCD_h
#define MGLCD_h

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#include "WProgram.h"

struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
	uint8_t inverted;
};

class MGLCD
{
	public:
		MGLCD(uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7, uint8_t A0, uint8_t RW, uint8_t EP, uint8_t RST);

		void	initLCD();
		void	rotateDisplay(uint8_t value);
		void	clrScr();
		void	fillScr();
		void	invert(bool mode);
		void	setPixel(uint8_t x, uint8_t y);
		void	clrPixel(uint8_t x, uint8_t y);
		void	invPixel(uint8_t x, uint8_t y);
		void	drawLine(int x1, int y1, int x2, int y2);
		void	drawRect(int x1, int y1, int x2, int y2);
		void	drawRoundRect(int x1, int y1, int x2, int y2);
		void	drawCircle(int x, int y, int radius);
		void	drawBitmap(int x, int y, uint8_t* bitmap, int sx, int sy, bool flash=true);
		void	invertText(bool mode);
		void	print(char *st, int x, int y);
		void	printNumI(long num, int x, int y);
		void	printNumF(double num, byte dec, int x, int y);
		void	setFont(uint8_t* font);

	protected:
		volatile uint8_t		*P_A0, *P_RW, *P_EP, *P_RST;
		volatile uint8_t		B_A0, B_RW, B_EP, B_RST;
		volatile uint8_t		*PO_DB[8];
		volatile uint8_t		*PI_DB[8];
		volatile uint8_t		*DD_DB[8];
		volatile uint8_t		B_DB[8];
		volatile uint8_t		_flip_horiz;
		volatile uint8_t		_inited;
		_current_font	cfont;

		void	_send_com(uint8_t data);
		void	_send_data(uint8_t data);
		uint8_t	_read_data(uint8_t dummy=false);
		void	_data_direction(uint8_t dir);
		void	setpage(uint8_t page);
		void	setaddr(uint8_t addr);
		void	drawHLine(int x, int y, int l);
		void	drawVLine(int x, int y, int l);
		void	_print_char(unsigned char c, int x, int row);
};

#endif