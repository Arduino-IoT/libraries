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
/**
 * \file
 * \brief configuration definitions
 */
#ifndef SdFatConfig_h
#define SdFatConfig_h
#include <stdint.h>
//------------------------------------------------------------------------------
/**
 * Call flush for endl if ENDL_CALLS_FLUSH is nonzero
 *
 * The standard for iostreams is to call flush.  This is very costly for
 * SdFat.  Each call to flush causes 2048 bytes of I/O to the SD.
 *
 * SdFat has a single 512 byte buffer for SD I/O so it must write the current
 * data block to the SD, read the directory block from the SD, update the
 * directory entry, write the directory block to the SD and read the data
 * block back into the buffer.
 *
 * The SD flash memory controller is not designed for this many rewrites
 * so performance may be reduced by more than a factor of 100.
 *
 * If ENDL_CALLS_FLUSH is zero, you must call flush and/or close to force
 * all data to be written to the SD.
 */
 #define ENDL_CALLS_FLUSH 0
//------------------------------------------------------------------------------
/**
 * Enable Arduino Print support if USE_PRINT is non-zero.
 *
 * Disabling Print will save some flash if you only read files.
 */
#define USE_PRINT 1
//------------------------------------------------------------------------------
/**
 * Allow use of deprecated functions if ALLOW_DEPRECATED_FUNCTIONS is non-zero
 */
#define ALLOW_DEPRECATED_FUNCTIONS 1
//------------------------------------------------------------------------------
/**
 * Allow FAT12 volumes if FAT12_SUPPORT is non-zero.
 * FAT12 has not been well tested.
 */
#define FAT12_SUPPORT 0
//------------------------------------------------------------------------------
/**
 * Define MEGA_SOFT_SPI non-zero to use software SPI on Mega Arduinos.
 * Pins used are SS 10, MOSI 11, MISO 12, and SCK 13.
 *
 * MEGA_SOFT_SPI allows an unmodified Adafruit GPS Shield to be used
 * on Mega Arduinos.  Software SPI works well with GPS Shield V1.1
 * but many SD cards will fail with GPS Shield V1.0.
 */
#define MEGA_SOFT_SPI 0
//------------------------------------------------------------------------------
/**
 * Set USE_SOFTWARE_SPI non-zero to always use software SPI.
 */
#define USE_SOFTWARE_SPI 0
// define software SPI pins so Mega can use unmodified 168/328 shields
/** Software SPI chip select pin for the SD */
uint8_t const SOFT_SPI_CS_PIN = 10;
/** Software SPI Master Out Slave In pin */
uint8_t const SOFT_SPI_MOSI_PIN = 11;
/** Software SPI Master In Slave Out pin */
uint8_t const SOFT_SPI_MISO_PIN = 12;
/** Software SPI Clock pin */
uint8_t const SOFT_SPI_SCK_PIN = 13;
//------------------------------------------------------------------------------
/**
 * Protect block zero from write if SD_PROTECT_BLOCK_ZERO is non-zero.
 * Default is zero since formatting an SD requires writing block zero.
 */
#define SD_PROTECT_BLOCK_ZERO 0
#endif  // SdFatConfig_h
