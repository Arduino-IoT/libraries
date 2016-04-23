/* Arduino SdFat Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <SdFat.h>
#include <WProgram.h>
extern "C" void __cxa_pure_virtual() {
//   Serial.println("VIRTUAL");///////////////////////////////////////////////////////
   exit (1);
 }
//==============================================================================
  /// @cond SHOW_PROTECTED
int16_t SdBase::getch() {
  uint8_t c;
  int8_t s = read(&c, 1);
  if (s != 1) {
    if (s < 0 ) {
      setstate(badbit);
    } else {
      setstate(eofbit);
    }
    return -1;
  }
  if (c != '\r' || (getmode() & ios::binary)) return c;
  s = read(&c, 1);
  if (s == 1 && c == '\n') return c;
  if (s == 1) seekCur(-1);
  return '\r';
}
//------------------------------------------------------------------------------
void SdBase::open(const char* path, ios::openmode mode) {
uint8_t flags;
#define SWITCH  ////////////////////////////////////////////////////////////////////////
#ifdef SWITCH
  switch(mode & (app | in | out | trunc)) {
  case in:
    flags = O_READ;
    break;
  case out:
  case (out | trunc):
    flags = O_WRITE | O_TRUNC | O_CREAT;
    break;
  case (out | app):
    flags = O_WRITE | O_APPEND | O_CREAT;
    break;
  case (in | out):
    flags = O_READ | O_WRITE;
    break;
  case (in | out | trunc):
    flags = O_READ | O_WRITE | O_TRUNC | O_CREAT;
    break;
  case (in | out | app):
    flags = O_READ | O_WRITE | O_APPEND | O_CREAT;
    break;
  default:
    goto fail;
  }
#else
  flags = ios::out & mode ? O_WRITE : 0;
  if (mode & ios::in) flags |= O_READ;
  if (mode & ios::app) flags |= O_APPEND | O_CREAT;
  if (mode & ios::trunc) flags |= O_TRUNC | O_CREAT;
  if ((mode & (ios::in | ios::out | ios::app)) == ios::out) {
    flags |= O_CREAT | O_TRUNC;
  }
#endif
  if (!SdFile::open(path, flags)) goto fail;
  if ((mode & ios::ate) && !seekEnd()) goto fail;
  setmode(mode);
  clear();
  return;
  
 fail:
  SdFile::close();
  setstate(failbit);
  return;
}
//------------------------------------------------------------------------------
void SdBase::putch(char c) {
  if (c == '\n' && !(getmode() & ios::binary)) {
    write('\r');
  }
  write(c);
  if (writeError) setstate(badbit);
}
//------------------------------------------------------------------------------
void SdBase::putstr(const char* str) {
  uint16_t n = 0;
  while (1) {
    char c = str[n];
    if (c == '\0' || (c == '\n' && !(getmode() & ios::binary))) {
      if (n > 0) write(str, n);
      if (c == '\0') break;
      write('\r');
      str += n;
      n = 0;
    }
    n++;
  }
  if (writeError) setstate(badbit);
}
/// @endcond
