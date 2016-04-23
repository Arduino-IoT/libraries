/*
	ELECHOUSE_NRF24L01.h - NRF24L01 module library
	Copyright (c) 2010 Michael.
  	Author: Michael, <www.elechouse.com>
  	Version: December 29, 2010

	This library is designed to use NRF24L01 module on Arduino platform.
	NRF24L01 module is an useful wireless module.Using the functions of the 
	library, you can easily send and receive data by the NRF24L01 module. 
	Just have fun!
	For the details, please refer to the datasheet of NRF24L01.
*/

#ifndef ELECHOUSE_NRF24L01_h
#define ELECHOUSE_NRF24L01_h

#include "WProgram.h"

// Define interface to nRF24L01
#define SCK_PIN   13	// Serial Clock pin, (output)
#define MISO_PIN  12	// Master In, Slave Out pin (input)
#define MOSI_PIN  11	// Master Out, Slave In pin (output)
#define CSN_PIN   10	// Slave Select pin, (output to CSN, nRF24L01)
#define IRQ_PIN	  2		// Interrupt signal, from nRF24L01 (input)
#define CE_PIN	  9		// Chip Enable pin signal,Activates RX or TX mode (output)

// SPI(nRF24L01) commands
#define READ_REG    0x00  // Define read command to register
#define WRITE_REG   0x20  // Define write command to register
#define R_RX_PAYLOAD 0x61  // Define RX payload register address
#define W_TX_PAYLOAD 0xA0  // Define TX payload register address
#define FLUSH_TX    0xE1  // Define flush TX register command
#define FLUSH_RX    0xE2  // Define flush RX register command
#define REUSE_TX_PL 0xE3  // Define reuse TX payload register command
#define NOP         0xFF  // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses)
#define CONFIG      0x00  // 'Config' register address
#define EN_AA       0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR   0x02  // 'Enabled RX addresses' register address
#define SETUP_AW    0x03  // 'Setup address width' register address
#define SETUP_RETR  0x04  // 'Setup Auto. Retrans' register address
#define RF_CH       0x05  // 'RF channel' register address
#define RF_SETUP    0x06  // 'RF setup' register address
#define STATUS      0x07  // 'Status' register address
#define OBSERVE_TX  0x08  // 'Observe TX' register address
#define CD          0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F  // 'RX address pipe5' register address
#define TX_ADDR     0x10  // 'TX address' register address
#define RX_PW_P0    0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17  // 'FIFO Status Register' register address

//************************************* class **************************************************//
class ELECHOUSE_NRF24L01
{
	private:
		void SpiInit(void);
		void SpiMode(byte config);
		byte Spi_RW(byte value);
		void SpiWriteCommand(byte value);
		void SpiWriteReg(byte addr, byte value);
		void SpiWriteBurstReg(byte addr, byte *buffer, byte num);
		byte SpiReadReg(byte addr);
		void SpiReadBurstReg(byte addr, byte *buffer, byte num);

	public:
		void Init(void);
		void RegConfigSettings(void);
		void RX_Setting(byte pipe_num, byte *pipe_address, byte pipe_addr_width, byte RX_pload_width);
		void RX_ModeStart(void);
		void TX_Setting(byte *TX_addr, byte TX_addr_width, byte *TX_data_buf, byte TX_pload_width);
		void TX_ModeStart(void);
		byte CheckSendFlag(void);
		byte CheckReceiveFlag(void);
		byte ReceiveData(byte *rxBuffer, byte RX_pload_width);
};

extern ELECHOUSE_NRF24L01 ELECHOUSE_nRF24L01;

#endif 