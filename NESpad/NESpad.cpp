/*
  NESpad - Arduino library for interfacing with a NES joystick

  Version: 1.3 (11/12/2010) - get rid of shortcut constructor - seems to be broken
  Version: 1.2 (05/25/2009) - put pin numbers in constructor (Pascal Hahn)
  Version: 1.1 (09/22/2008) - fixed compilation errors in arduino 0012 (Rob Duarte)
  Version: 1.0 (09/20/2007) - Created (Rob Duarte)
  
  This file is part of NESpad.

  NESpad is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  NESpad is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with NESpad.  If not, see <http://www.gnu.org/licenses/>.  
*/

#include "NESpad.h"
#include "WProgram.h"

// change the next three lines if necessary
// arduino pins and their connection to the nes joystick
// default: digital pin 2=strobe, 3=clock, 4=data
// see readme.txt for game pad pinout

// -- actually, let's just eliminate this since people seem to be having
// -- trouble with it.. stick with specifying the pin numbers as 
// -- arguments to the constructor

// NESpad::NESpad() {
//     NESpad::NESpad(2,3,4);
// }

NESpad::NESpad(int strobe, int clock, int data)
  : m_strobe (strobe),
  m_clock (clock),
  m_data (data)
  {
    pinMode(strobe, OUTPUT);
    pinMode(clock,  OUTPUT);
    pinMode(data, INPUT);
  }

byte NESpad::buttons(void)
{
  byte ret = 0;
  byte i;
  strobe();
  for (i = 0; i < 8; i++) {
    ret |= shiftin() << i;
  }
  return ~ret;
}

void NESpad::strobe(void)
{
  digitalWrite(m_strobe,HIGH);
  delayMicroseconds(12);
  digitalWrite(m_strobe,LOW);
}

byte NESpad::shiftin(void)
{
  byte ret = digitalRead(m_data);
  delayMicroseconds(12);
  digitalWrite(m_clock,HIGH);
  delayMicroseconds(12);
  digitalWrite(m_clock,LOW);
  return ret;
}
