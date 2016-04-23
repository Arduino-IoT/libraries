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
#include <Wprogram.h>
#include <SdFat.h>
#include <SdFatUtil.h>
//------------------------------------------------------------------------------
/** Change current working directory to root
 *
 * Changes the current working directory to the SD's root directory.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
bool SdFat::chdir() {
  SdFile::cwd_ = &cwd_;
  cwd_.close();
  return cwd_.openRoot(&vol_);
}
//------------------------------------------------------------------------------
/** Change current working directory
 *
 * Changes the current working directory to the \a path subdirectory.
 *
 * Example: Executing chdir("SUB") in the "/DIR" directory will change the
 * current working directory from "/DIR" to "/DIR/SUB".
 *
 * \param[in] path The name of the subdirectory.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
bool SdFat::chdir(const char *path) {
  SdFile dir;
  if (path[0] == '/' && path[1] == '\0') return chdir();
  if (!dir.open(path, O_READ)) goto fail;
  if (!dir.isDir()) goto fail;
  cwd_ = dir;
  return true;

 fail:
  return false;
}
//------------------------------------------------------------------------------
/** %Print any SD error code and halt. */
void SdFat::errorHalt() {
  errorPrint();
  while (1);
}
//------------------------------------------------------------------------------
/** %Print msg, any SD error code, and halt.
 *
 * \param[in] msg Message to print.
 */
void SdFat::errorHalt(char const* msg) {
  errorPrint(msg);
  while (1);
}
//------------------------------------------------------------------------------
/** %Print msg, any SD error code, and halt.
 *
 * \param[in] msg Message in program space (flash memory) to print.
 */
void SdFat::errorHalt_P(PGM_P msg) {
  errorPrint_P(msg);
  while (1);
}
//------------------------------------------------------------------------------
/** %Print any SD error code. */
void SdFat::errorPrint() {
  if (!card_.errorCode()) return;
  PgmPrint("SD errorCode: ");
  Serial.print(card_.errorCode(), HEX);
  Serial.println();
}
//------------------------------------------------------------------------------
/** %Print msg, any SD error code.
 *
 * \param[in] msg Message to print.
 */
void SdFat::errorPrint(char const* msg) {
  PgmPrint("error: ");
  Serial.println(msg);
  errorPrint();
}
//------------------------------------------------------------------------------
/** %Print msg, any SD error code.
 *
 * \param[in] msg Message in program space (flash memory) to print.
 */
void SdFat::errorPrint_P(PGM_P msg) {
  PgmPrint("error: ");
  SerialPrintln_P(msg);
  errorPrint();
}
//------------------------------------------------------------------------------
/**
 * Test for the existence of a file.
 *
 * \param[in] name Name of the file to be tested for.
 *
 * \return true if the file exists else false.
 */
bool SdFat::exists(const char* name) {
  return cwd_.exists(name);
}
//------------------------------------------------------------------------------
/**
 * Initialize an SdFat object.
 *
 * Initializes the SD card, SD volume, and root directory.
 *
 * \param[in] sckRateID value for SPI SCK rate. See Sd2Card::init().
 * \param[in] chipSelectPin SD chip select pin. See Sd2Card::init().
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
bool SdFat::init(uint8_t sckRateID, uint8_t chipSelectPin) {
  return (card_.init(sckRateID, chipSelectPin) && vol_.init(&card_) && chdir());
}
//------------------------------------------------------------------------------
/** %Print error details and halt after SdFat::init() fails. */
void SdFat::initErrorHalt() {
  initErrorPrint();
  while (1);
}
//------------------------------------------------------------------------------
/** Print error details after SdFat::init() fails. */
void SdFat::initErrorPrint() {
  if (card_.errorCode()) {
    PgmPrintln("Can't access SD card. Do not reformat.");
    errorPrint();
    if (card_.errorCode() == SD_CARD_ERROR_CMD0) {
      PgmPrintln("No card or SPI problem?");
    }
  } else if (vol_.fatType() == 0) {
    PgmPrintln("Invalid format, reformat SD.");
  } else if (vol_.fatType() == 12 && !FAT12_SUPPORT) {
    PgmPrintln("Card is formatted FAT12.");
  } else if (!cwd_.isOpen()) {
    PgmPrintln("Can't open root directory.");
  } else {
    PgmPrintln("No init error found.");
  }
}
//------------------------------------------------------------------------------
/** List the directory contents of the current working directory to Serial.
 *
 * \param[in] flags The inclusive OR of
 *
 * LS_DATE - %Print file modification date
 *
 * LS_SIZE - %Print file size.
 *
 * LS_R - Recursive list of subdirectories.
 *
 * \param[in] indent Amount of space before file name. Used for recursive
 * list to indicate subdirectory level.
 */
void SdFat::ls(uint8_t flags, uint8_t indent) {
  cwd_.ls(flags, indent);
}
//------------------------------------------------------------------------------
/** Make a subdirectory in the current working directory.
 *
 * \param[in] path A path with a valid 8.3 DOS name for the subdirectory.
 *
 * \param[in] pFlag Create missing parent directories if true.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
bool SdFat::mkdir(const char* path, bool pFlag) {
  SdFile sub;
  return sub.mkdir(&cwd_, path, pFlag);
}
//------------------------------------------------------------------------------
/** Remove a file from the current working directory.
*
* \param[in] path A path with a valid 8.3 DOS name for the file.
*
* \return The value one, true, is returned for success and
* the value zero, false, is returned for failure.
*/
bool SdFat::remove(const char* path) {
  return SdFile::remove(&cwd_, path);
}
//------------------------------------------------------------------------------
/** Remove a subdirectory from the current working directory.
 *
 * \param[in] path A path with a valid 8.3 DOS name for the subdirectory.
 *
 * The subdirectory file will be removed only if it is empty.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 */
bool SdFat::rmdir(const char* path) {
  SdFile sub;
  if (sub.open(path, O_READ)) {
    return sub.rmdir();
  }

 fail:
  return false;
}
