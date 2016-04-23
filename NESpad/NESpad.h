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

#ifndef NESpad_h
#define NESpad_h

#include <inttypes.h>

#define NES_A       B00000001
#define NES_B       B00000010
#define NES_SELECT  B00000100
#define NES_START   B00001000
#define NES_UP      B00010000
#define NES_DOWN    B00100000
#define NES_LEFT    B01000000
#define NES_RIGHT   B10000000

class NESpad {

  public:
    NESpad();
    NESpad(int strobe, int clock, int data);
    uint8_t buttons();

  private:
    void strobe();
    uint8_t shiftin();
    int m_strobe, m_clock, m_data;

};

#endif

