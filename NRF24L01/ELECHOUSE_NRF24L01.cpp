/*
	ELECHOUSE_NRF24L01.cpp - NRF24L01 module library
	Copyright (c) 2010 Michael.
  	Author: Michael, <www.elechouse.com>
  	Version: December 29, 2010

	This library is designed to use NRF24L01 module on Arduino platform.
	NRF24L01 module is an useful wireless module.Using the functions of the 
	library, you can easily send and receive data by the NRF24L01 module. 
	Just have fun!
	For the details, please refer to the datasheet of NRF24L01.
*/
#include <ELECHOUSE_NRF24L01.h>
#include <WProgram.h>


/****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :NRF24L01 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::Init(void)
{
	SpiInit();						//spi pins&mode
	pinMode(CE_PIN, OUTPUT);
	pinMode(IRQ_PIN, INPUT);
	digitalWrite(CE_PIN, LOW);		//power down/stand by
	digitalWrite(CSN_PIN, HIGH);	//Spi disable
	digitalWrite(SCK_PIN, LOW);
}

/****************************************************************
*FUNCTION NAME:SpiInit
*FUNCTION     :spi communication initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::SpiInit(void)
{
  // initialize the SPI pins
  pinMode(SCK_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(CSN_PIN, OUTPUT);

  // enable SPI Master, MSB, SPI mode 0, FOSC/4
  SpiMode(0);
}
/****************************************************************
*FUNCTION NAME:SpiMode
*FUNCTION     :set spi mode
*INPUT        :        config               mode
			   (0<<CPOL) | (0 << CPHA)		 0
			   (0<<CPOL) | (1 << CPHA)		 1
			   (1<<CPOL) | (0 << CPHA)		 2
			   (1<<CPOL) | (1 << CPHA)		 3
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::SpiMode(byte config)
{
  byte tmp;

  // enable SPI master with configuration byte specified
  SPCR = 0;
  SPCR = (config & 0x7F) | (1<<SPE) | (1<<MSTR);
  tmp = SPSR;
  tmp = SPDR;
}

/****************************************************************
*FUNCTION NAME:Spi_RW
*FUNCTION     :spi write and read
*INPUT        :value: data to send
*OUTPUT       :data to receive
****************************************************************/
byte ELECHOUSE_NRF24L01::Spi_RW(byte value)
{
  SPDR = value;
  while (!(SPSR & (1<<SPIF))) ;
  return SPDR;
}

/****************************************************************
*FUNCTION NAME:SpiWriteCommand
*FUNCTION     :write command to NRF24L01,(FLUSH_TX/FLUSH_RX) 
*INPUT        :value: command
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::SpiWriteCommand(byte value)
{
	digitalWrite(CSN_PIN, LOW);
	Spi_RW(value);
	digitalWrite(CSN_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiWriteReg
*FUNCTION     :NRF24L01 write data to register
*INPUT        :addr: register address; value: register value
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::SpiWriteReg(byte addr, byte value)
{
	digitalWrite(CSN_PIN, LOW);
	Spi_RW(addr);
	Spi_RW(value);
	digitalWrite(CSN_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiWriteBurstReg
*FUNCTION     :NRF24L01 write burst data to register
*INPUT        :addr: register address; buffer:register value array; num:number to write
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::SpiWriteBurstReg(byte addr, byte *buffer, byte num)
{
	byte i;

    digitalWrite(CSN_PIN, LOW);
    Spi_RW(addr);
    for (i = 0; i < num; i++)
 	{
        Spi_RW(buffer[i]);
    }
    digitalWrite(CSN_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiReadReg
*FUNCTION     :NRF24L01 read data from register
*INPUT        :addr: register address
*OUTPUT       :register value
****************************************************************/
byte ELECHOUSE_NRF24L01::SpiReadReg(byte addr) 
{
	byte  value;

	digitalWrite(CSN_PIN, LOW);
	Spi_RW(addr);
	value=Spi_RW(0);
	digitalWrite(CSN_PIN, HIGH);

	return value;
}

/****************************************************************
*FUNCTION NAME:SpiReadBurstReg
*FUNCTION     :NRF24L01 read burst data from register
*INPUT        :addr: register address; buffer:array to store register value; num: number to read
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::SpiReadBurstReg(byte addr, byte *buffer, byte num)
{
	byte i;

	digitalWrite(CSN_PIN, LOW);
	Spi_RW(addr);
	for(i=0;i<num;i++)
	{
		buffer[i]=Spi_RW(0);
	}
	digitalWrite(CSN_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:RegConfigSettings
*FUNCTION     :some common NRF24L01 register config //details refer datasheet of NRF24L01//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::RegConfigSettings(void)
{
  	SpiWriteReg(WRITE_REG + RF_CH, 0x20);               // Select RF channel 
  	SpiWriteReg(WRITE_REG + RF_SETUP, 0x07);            // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
	SpiWriteReg(WRITE_REG + SETUP_AW, 0x03);			// address width 5 bytes
 	SpiWriteReg(WRITE_REG + EN_AA, 0x3F);               // Enable Auto.Ack:Pipe0-5
}

/****************************************************************
*FUNCTION NAME:RX_Setting
*FUNCTION     :NRF24L01 RX setting //details refer datasheet of NRF24L01//
*INPUT        :pipe_num: pipe(0-5), pipe_address: address corresponding to pipe_num, 
			   pipe_addr_width: address width(pipe 0-1:5, pipe 2-5:1), RX_pload_width: RX payload width(equal to TX payload width)
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::RX_Setting(byte pipe_num, byte *pipe_address, byte pipe_addr_width, byte RX_pload_width)
{
	byte temp;	

	digitalWrite(CE_PIN, LOW);
	SpiWriteBurstReg(WRITE_REG + RX_ADDR_P0 + pipe_num, pipe_address, pipe_addr_width);  //set pipe address 
  	SpiWriteReg(WRITE_REG + RX_PW_P0 + pipe_num, RX_pload_width);  // set pipe RX payload width
	temp=SpiReadReg(EN_RXADDR);
	temp=temp|(1<<pipe_num);
	SpiWriteReg(WRITE_REG + EN_RXADDR, temp);           // enable the pipe which is set
}

/****************************************************************
*FUNCTION NAME:RX_ModeStart
*FUNCTION     :start RX mode
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::RX_ModeStart(void)
{
	digitalWrite(CE_PIN, LOW);
  	SpiWriteReg(WRITE_REG + CONFIG, 0x0f);              // Set PWR_UP bit, enable CRC(2 bytes) , RX mode
	digitalWrite(CE_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:TX_Setting
*FUNCTION     :NRF24L01 TX setting //details refer datasheet of NRF24L01//
*INPUT        :TX_addr: address TX to, TX_addr_width: TX address width(equal to the setting in SETUP_AW)
			   TX_data_buf: data to send, TX_pload_width: TX payload width(equal to RX payload width)
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::TX_Setting(byte *TX_addr, byte TX_addr_width, byte *TX_data_buf, byte TX_pload_width)
{
	digitalWrite(CE_PIN, LOW);
  	SpiWriteBurstReg(WRITE_REG + TX_ADDR, TX_addr, TX_addr_width);     // set TX address
  	SpiWriteBurstReg(WRITE_REG + RX_ADDR_P0, TX_addr, TX_addr_width);  // RX_Addr_P0 same with TX_Addr for Auto Ack
  	SpiWriteBurstReg(W_TX_PAYLOAD, TX_data_buf, TX_pload_width);       // write data to TX FIFO
	SpiWriteReg(WRITE_REG + SETUP_RETR, 0x0a);  					   // 250us+86us£¬10 re-transmit on fail of AA
}

/****************************************************************
*FUNCTION NAME:TX_ModeStart
*FUNCTION     :start TX mode
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_NRF24L01::TX_ModeStart(void)
{
	digitalWrite(CE_PIN, LOW);
  	SpiWriteReg(WRITE_REG + CONFIG, 0x0e);      // Set PWR_UP bit, enable CRC(2 bytes) , TX mode
	digitalWrite(CE_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:CheckSendFlag
*FUNCTION     :check whether transmit data successfully or not
*INPUT        :none
*OUTPUT       :flag: 0 fail, 1 success
****************************************************************/
byte ELECHOUSE_NRF24L01::CheckSendFlag(void)
{
	byte sta_temp,flag;	

	while(digitalRead(IRQ_PIN));			//if transmit successfully or up to max number of retransmit, IRQ_PIN low
	sta_temp=SpiReadReg(STATUS);
	if(sta_temp&0x20)						//transmit successfully
	{
		flag=1;
	}
	else									//up to max number of retransmit
	{
		flag=0;
		digitalWrite(CE_PIN, LOW);
		SpiWriteCommand(FLUSH_TX);			//flush TX FIFO
	}
	SpiWriteReg(WRITE_REG + STATUS, sta_temp);  // clear TX_DS or MAX_RT interrupt
	
	return flag;
}

/****************************************************************
*FUNCTION NAME:CheckReceiveFlag
*FUNCTION     :check receive data or not
*INPUT        :none
*OUTPUT       :flag: 0 no data; 1 receive data 
****************************************************************/
byte ELECHOUSE_NRF24L01::CheckReceiveFlag(void)
{
	byte sta_temp;

	sta_temp=SpiReadReg(STATUS);
	if(sta_temp&0x40)									//receive data
	{
		SpiWriteReg(WRITE_REG + STATUS, sta_temp);    	//clear RX_DR interrupt flag
		return 1;
	}
	else												//no data
	{
		return 0;
	}
}

/****************************************************************
*FUNCTION NAME:ReceiveData
*FUNCTION     :read data received from RXfifo
*INPUT        :rxBuffer: buffer to store data, RX_pload_width: RX data width
*OUTPUT       :pipe number which received data
****************************************************************/
byte ELECHOUSE_NRF24L01::ReceiveData(byte *rxBuffer, byte RX_pload_width)
{
	byte sta_temp,pipe_num;	

	sta_temp=SpiReadReg(STATUS);
	pipe_num=(sta_temp&0x0e)>>1;
	SpiReadBurstReg(R_RX_PAYLOAD, rxBuffer, RX_pload_width);

	return pipe_num;
}

ELECHOUSE_NRF24L01 ELECHOUSE_nRF24L01;

