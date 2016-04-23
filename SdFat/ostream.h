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
#ifndef ostream_h
#define ostream_h
/**
 * \file
 * \brief \ref ostream class
 */
#include <ios.h>
//------------------------------------------------------------------------------
/** macro for flash inserter */
#define pstr(str) pgm(PSTR(str))
/** \struct pgm
 * \brief type for string in flash
 */
struct pgm {
  /** Pointer to flash string */
  char *ptr;
  /** constructor
   * \param[in] str initializer for pointer.
   */
  explicit pgm(char* str) : ptr(str) {}
  /** constructor
   * \param[in] str initializer for pointer.
   */
  explicit pgm(const char *str) : ptr((char*)str) {}
};
//==============================================================================
/**
 * \class ostream
 * \brief Output Stream
 */
class ostream : public virtual ios {
 public:
  ostream() {}

  /** call manipulator
   * \param[in] pf function to call
   * \return the stream
   */
  ostream& operator<< (ostream& (*pf)(ostream& str)) {
    return pf(*this);
  }
  /** call manipulator
   * \param[in] pf function to call
   * \return the stream
   */
  ostream& operator<< (ios_base& (*pf)(ios_base& str)) {
    pf(*this);
    return *this;
  }
  /** Output bool
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (bool arg) {
    putBool(arg);
    return *this;
  }
  /** Output string
   * \param[in] arg string to output
   * \return the stream
   */
  ostream &operator<< (const char *arg) {
    putStr(arg);
    return *this;
  }
  /** Output string
   * \param[in] arg string to output
   * \return the stream
   */
  ostream &operator<< (const signed char *arg) {
    putStr((const char*)arg);
    return *this;
  }
  /** Output string
   * \param[in] arg string to output
   * \return the stream
   */
  ostream &operator<< (const unsigned char *arg) {
    putStr((const char*)arg);
    return *this;
  }
  /** Output character
   * \param[in] arg character to output
   * \return the stream
   */
  ostream &operator<< (char arg) {
    putChar(arg);
    return *this;
  }
  /** Output character
   * \param[in] arg character to output
   * \return the stream
   */
  ostream &operator<< (signed char arg) {
    putChar(static_cast<char>(arg));
    return *this;
  }
  /** Output character
   * \param[in] arg character to output
   * \return the stream
   */
  ostream &operator<< (unsigned char arg) {
    putChar(static_cast<char>(arg));
    return *this;
  }
  /** Output double
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (double arg) {
    putDouble(arg);
    return *this;
  }
  /** Output signed int
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (int16_t arg) {
    putNum((uint32_t)arg);
    return *this;
  }
  /** Output unsigned int
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (uint16_t arg) {
    putNum((uint32_t)arg);
    return *this;
  }
  /** Output signed long
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (int32_t arg) {
    putNum(arg);
    return *this;
  }
  /** Output uint32_t
   * \param[in] arg value to output
   * \return the stream
   */
  ostream &operator<< (uint32_t arg) {
    putNum(arg);
    return *this;
  }
  /** Output pointer
   * \param[in] arg value to output
   * \return the stream
   */
  ostream& operator<< (const void *arg) {
    putNum(reinterpret_cast<unsigned long>(arg));
    return *this;
  }
  /** Output a string from flash
   * \param[in] arg pgm struct pointing to string
   * \return the stream
   */
  ostream &operator<< (pgm arg) {
    putPgm(arg);
    return *this;
  }
  /**
   * Puts a character in a stream.
   *
   * The unformatted output function inserts the element \a ch.
   * It returns *this.
   *
   * \param[in] ch The character
   * \return A reference to the ostream object.
   */
  ostream& put(char ch){
    putch(ch);
    return *this;
  }
//  ostream& write(char *str, streamsize count);
  /**
   * Flushes the buffer associated with this stream. The flush function
   * calls the sync function of the associated file.
   * \return A reference to the ostream object.
   */
  ostream& flush() {
    if (!sync()) setstate(badbit);
    return *this;
  }
  /**
   * \return the stream position
   */
  pos_type tellp();
  /**
   * Set the stream position
   * \param[in] pos The absolute position in which to move the read pointer.
   * \return Is always *this.  Failure is indicated by the state of *this.
   */
  ostream& seekp(pos_type pos);
  /**
   * Set the stream position
   *
   * \param[in] off An offset to move the read pointer relative to way.
   * off is a signed 32-bit int so the offset is limited to +- 2GB.
   * \param[in] way One of the ios_base::seekdir enumerations.
   * \return Is always *this.  Failure is indicated by the state of *this.
   */
  ostream& seekp(off_type off, ios_base::seekdir way);
 protected:
  /// @cond SHOW_PROTECTED
  /** Put character with binary/text conversion
   * \param[in] ch character to write
   */
  virtual void putch(char ch) = 0;
  virtual void putstr(const char *str) {
    while (*str) putch(*str++);
  }
  virtual bool seekpos(pos_type pos) = 0;
  
  virtual bool sync() = 0;
  
  virtual pos_type tellpos() = 0;
  /// @endcond
 private:
  void do_fill(unsigned len);
  void fill_not_left(unsigned len);
  char* fmtNum(uint32_t n, char *ptr, uint8_t base);
  void putBool(bool b);
  void putChar(char c);
  void putDouble(double n);
  void putNum(uint32_t n, bool neg = false);
  void putNum(int32_t n);
  void putPgm(const pgm& arg);
  void putStr(const char* str);
};
#endif  // ostream_h
