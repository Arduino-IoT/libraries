
/******************************************************************************

  Filename:	maple-spi.c
  Description:	SPI bus configuration for the WiShield 1.0 and Leaf Labs Maple board

 ******************************************************************************

  TCP/IP stack and driver for the WiShield 1.0 wireless devices

  Copyright(c) 2010 Adam Feuer All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   Author               Date        Comment
  ---------------------------------------------------------------
   Adam Feuer        19-Jul-2010   initial revision

 *****************************************************************************/

#include "nvic.h"
#include "exti.h"
#include "wirish.h"
#include "maple-spi.h"
#include "maple-util.h"
#include "spi.h"

// ZG2100 SPI HAL

int led0_bit = 0;
int led1_bit = 0;
int led2_bit = 0;
int led3_bit = 0;
int led_conn_bit = 0;

void enable_interrupt(uint8 channel) {

    // copied from exti_attach_interrupt() in libmaple/exti.c
    // TODO: refactor exti_attach_interrupt() to separate this into a separate function and eliminate duplication

    /* Configure the enable interrupt bits for the NVIC  */
    switch (channel) {
    case EXTI0:
    case EXTI1:
    case EXTI2:
    case EXTI3:
    case EXTI4:
        REG_SET(NVIC_ISER0, BIT(channel + 6));
        break;

    /* EXTI5-9 map to the same isr */
    case EXTI5:
    case EXTI6:
    case EXTI7:
    case EXTI8:
    case EXTI9:
        REG_SET(NVIC_ISER0, BIT(23));
        break;

    /* EXTI10-15 map to the same isr */
    case EXTI10:
    case EXTI11:
    case EXTI12:
    case EXTI13:
    case EXTI14:
    case EXTI15:
        REG_SET(NVIC_ISER1, BIT(8));
        break;
    }

}

void zg2100_isr_enable(uint8 channel) {
   enable_interrupt(channel);

   // enable USB interrupts so the bootloader will continue to work
   nvic_enable_interrupt(NVIC_INT_USBHP);
}

/*
#define SPI1_Init()  DDRB  |= SPI0_SS_BIT|SPI0_SCLK_BIT|SPI0_MOSI_BIT|LEDConn_BIT;\
	DDRB  &= ~SPI0_MISO_BIT;\
	PORTB = SPI0_SS_BIT;\
	SPCR  = 0x50;\
	SPSR  = 0x01
*/

// Maple SPI

static const uint32 prescaleFactors[MAPLE_MAX_SPI_FREQS] = {
   MAPLE_SPI_PRESCALE_2,               // SPI_18MHZ
   MAPLE_SPI_PRESCALE_4,               // SPI_9MHZ
   MAPLE_SPI_PRESCALE_8,               // SPI_4_5MHZ
   MAPLE_SPI_PRESCALE_16,              // SPI_2_25MHZ
   MAPLE_SPI_PRESCALE_32,              // SPI_1_125MHZ
   MAPLE_SPI_PRESCALE_64,              // SPI_562_500KHZ
   MAPLE_SPI_PRESCALE_128,             // SPI_281_250KHZ
   MAPLE_SPI_PRESCALE_256,             // SPI_140_625KHZ
};

void SPI1_Init() {
  MapleSPIFrequency freq;
  uint32 spi_num, endian, prescale, mode;

  serialUsbPrintlnWaitForInput("***In SPI1_Init()");
  //serialUsbPrintln("***In SPI1_Init()");

  // set up CS pin
  ZG2100_CSInit();
  ZG2100_CSon();

  pinMode(D2, INPUT);

  // init SPI
  spi_num = 1;
  endian = SPI_MSBFIRST;
  //freq = MAPLE_SPI_4_5MHZ;
  freq = MAPLE_SPI_9MHZ;
  prescale = prescaleFactors[freq]; // only valid for SPI1
  mode = 0;
  spi_init(spi_num, prescale, endian, mode);

  //serialUsbPrintlnWaitForInput("*** Done with spi_init.");
  serialUsbPrintln("*** Done with spi_init.");
}

/**
 * @brief SPI synchronous 8-bit write, blocking.
 * @param spi_num which spi to send on
 * @return data shifted back from the slave
 */
uint8 zg_spi_tx_byte(uint32 spi_num, uint8 data) {
   SPI *spi;

   ASSERT(spi_num == 1 || spi_num == 2);

   spi = (spi_num == 1) ? (SPI*)SPI1_BASE : (SPI*)SPI2_BASE;

   spi->DR = data;

   while ( !(spi->SR & SR_TXE)   ||
           !(spi->SR & SR_RXNE)  ||
	    (spi->SR & SR_BSY))
     ;

   return spi->DR;
}

/*
uint8 zg_spi_tx(uint32 spi_num, uint8 *buf, uint32 len) {
   SPI *spi;
   uint32 i = 0;
   uint8 rc;

   ASSERT(spi_num == 1 || spi_num == 2);
   spi = (spi_num == 1) ? (SPI*)SPI1_BASE : (SPI*)SPI2_BASE;

   if (!len) {
      return 0;
   }

   while (i < len) {
      spi->DR = buf[i];
      while (!(spi->SR & SR_TXE) ||
              (spi->SR & SR_BSY) ||
             !(spi->SR & SR_RXNE))
         ;
      rc = spi->DR;
      i++;
   }
   return rc;
}

*/
