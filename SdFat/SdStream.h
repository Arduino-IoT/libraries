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
#ifndef SdStream_h
#define SdStream_h
/**
 * \file
 * \brief \ref fstream, \ref ifstream, and \ref ofstream classes
 */
#include <SdFile.h>
#include <WProgram.h>//////////////////////////////////////////////////////////////////////
#include <iostream.h>


//==============================================================================
/**
 * \class SdBase
 * \brief Base class for SD streams
 */
class SdBase : protected SdFile, virtual public ios {
 public:

 protected:
  /// @cond SHOW_PROTECTED
  int16_t getch();
  void putch(char c);
  void putstr(const char *str);
  void open(const char* path, ios::openmode mode);
  /**
   * Internal do not use
   * \return
   */
  ios::openmode getmode() {return mode_;}
  /**
   * internal do not use
   * \param[in] mode
   */
  void setmode(ios::openmode mode) {mode_ = mode;}
  /// @endcond
 private:
  ios::openmode mode_;
};
//==============================================================================
/**
 * \class fstream
 * \brief SD file input/output stream.
 */
class fstream : public iostream, SdBase {
 public:
  fstream() {}
  /**
   * Constructor with open
   *
   * \param[in] path path to open
   * \param[in] mode open mode
   */
  fstream (const char* path, openmode mode = in | out){
    open(path, mode);
  }
  ~fstream() {}
  /**
   * clear state and writeError
   * \param[in] state new state for stream
   */
  void clear(iostate state = goodbit) {
    ios::clear(state);
    SdFile::writeError = false;
  }
  /**
   *  Close a file and force cached data and directory information
   *  to be written to the storage device.
   */
  void close() {SdFile::close();}
  /**
   * open a fstream
   * \param[in] path file to open
   * \param[in] mode open mode
   *
   * Valid open modes are (at end, ios::ate, and/or ios::binary may be added):
   *
   * ios::out or ios::out | ios::trunc - Truncate to 0 length, if existent,
   * or create a file for writing only
   *
   * ios::out | ios::app - Append; open or create a file only for writing
   * at end of file
   *
   * ios::in | ios::out - Open file for update (reading and writing)
   *
   * ios::in | ios::out | ios::trunc - Truncate to 0 length, if existent,
   * or create file for update
   *
   * ios::in | ios::out | ios::app - Append; open or create text file for
   * update, writing at end of file
   */
  void open(const char* path, openmode mode = ios::in | ios::out){
    SdBase::open(path, mode);
  }
  /** \return True if stream is open else false. */
  bool is_open () {return SdFile::isOpen();}

 protected:
  /// @cond SHOW_PROTECTED
  /** Internal - do not use
   * \return
   */
  int16_t getch() {return SdBase::getch();}
    /** Internal - do not use
   * \param[out] pos
   */
  void getpos(fpos_t* pos) {SdFile::getpos(pos);}
  /** Internal - do not use
   * \param[in] c
   */
  void putch(char c) {SdBase::putch(c);}
  /** Internal - do not use
   * \param[in] pos
   */
  bool seekpos(pos_type pos) {return SdBase::seekSet(pos);}
  void setpos(fpos_t* pos) {SdFile::setpos(pos);}
  bool sync() {return SdBase::sync();}
  pos_type tellpos() {return SdBase::curPosition();}
  /// @endcond
};
//==============================================================================
/**
 * \class ifstream
 * \brief SD file input stream.
 */
class ifstream : public istream, SdBase {
 public:
  ifstream() {}
  /**
   * constructor with open
   * \param[in] path file to open
   * \param[in] mode open mode
   */
  ifstream(const char* path, openmode mode = in) {
    open(path, mode);
  }
  ~ifstream() {}
  /**
   *  Close a file and force cached data and directory information
   *  to be written to the storage device.
   */
  void close() {SdFile::close();}
  /** \return True if stream is open else false. */
  bool is_open() {return SdFile::isOpen();}
  /**
   * Open an ifstream
   * \param[in] path file to open
   * \param[in] mode open mode
   *
   * \a mode must be ios::in with optional at end, ios::ate,
   * and/or ios::binary.
   */
  void open (const char* path, openmode mode = ios::in) {
    SdBase::open(path, mode);
  }
 protected:
  /// @cond SHOW_PROTECTED
  /** Internal - do not use
   * \return
   */
  int16_t getch() {return SdBase::getch();}
  /** Internal - do not use
   * \param[out] pos
   */
  void getpos(fpos_t* pos) {SdFile::getpos(pos);}
  /** Internal - do not use
   * \param[in] pos
   */
  bool seekpos(pos_type pos) {return SdBase::seekSet(pos);}
  void setpos(fpos_t* pos) {SdFile::setpos(pos);}
  pos_type tellpos() {return SdBase::curPosition();}
  /// @endcond
};
//==============================================================================
/**
 * \class ofstream
 * \brief SD card output stream.
 */
class ofstream : public ostream, SdBase {
 public:
  ofstream() {}
  /**
   * constructor with open
   * \param[in] path file to open
   * \param[in] mode open mode
   */
  ofstream(const char* path, ios::openmode mode = ios::out) {
    open(path, mode);
  }
  ~ofstream() {}
  
  /**
   * clear state and writeError
   * \param[in] state new state for stream
   */
  void clear(iostate state = goodbit) {
    ios::clear(state);
    SdFile::writeError = false;
  }
  /**
   *  Close a file and force cached data and directory information
   *  to be written to the storage device.
   */
  void close() {SdFile::close();}
  /**
   * Open an ofstream
   * \param[in] path file to open
   * \param[in] mode open mode
   *
   * See fstream::open(), for modes. ofstreams can not be read with ios::in.
   */
  void open(const char* path, openmode mode = out) {
    SdBase::open(path, mode);
  }
  /** \return True if stream is open else false. */
  bool is_open() {return SdFile::isOpen();}
 protected:
  /// @cond SHOW_PROTECTED
  /**
   * Internal do not use
   * \param[in] c
   */
  void putch(char c) {SdBase::putch(c);}
  void putstr(const char* str) {SdBase::putstr(str);}
  bool seekpos(pos_type pos) {return SdBase::seekSet(pos);}
  /**
   * Internal do not use
   * \param[in] b
   */
  bool sync() {return SdBase::sync();}
  pos_type tellpos() {return SdBase::curPosition();}
  /// @endcond
};
//------------------------------------------------------------------------------
#endif  // SdStream_h
