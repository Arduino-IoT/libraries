/*
* VC0706.cpp
* A library for camera shield(VC0706 chipset)
*
* Copyright (c) 2014 seeed technology inc.
* Copyright (c) 2012, Adafruit Industries.
*
* All rights reserved.
*
* This library is based on Adafruit's VC0706 Serial Camera Library. Thanks to 
* their contribution to the code, we modify it to support our Seeed's Camera 
* Shield module.
*
* Below is the introduction of Adafruit's VC0706 Serial Camera module, we add it to here
* to express our thanks to them.
*
* ****************************************************************************
* This is a library for the Adafruit TTL JPEG Camera (VC0706 chipset)
*
* Pick one up today in the adafruit shop!
* ------> http://www.adafruit.com/products/397
*
* These displays use Serial to communicate, 2 pins are required to interface
*
* Adafruit invests time and resources providing this open source code, 
* please support Adafruit and open-source hardware by purchasing 
* products from Adafruit!
*
* Written by Limor Fried/Ladyada for Adafruit Industries.  
* BSD license, all text above must be included in any redistribution
* ******************************************************************************
*
* Software License Agreement (BSD License)
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holders nor the
* names of its contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "VC0706_UART.h"

void VC0706::_send(uint8_t cmd[], uint8_t len) 
{
    vc->write((byte)VC0706_SEND_MARK);
    vc->write((byte)VC0706_SERIAL_NUMBER);
    for (uint8_t i = 0; i < len; i++) {
        vc->write((byte)cmd[i]);
    }
}

uint8_t VC0706::_read(uint8_t bytes, uint8_t timeOut) 
{
    bufferLen = 0;
    uint8_t counter = 0;
    while ((counter < timeOut) && (bufferLen < bytes)){
        if (!vc->available()) {
            delay(1);
            counter++;
            continue;
        }
        counter = 0;
        vcBuff[bufferLen++] = vc->read();
    }
    return bufferLen;
}

uint8_t VC0706::_verify(uint8_t cmd)
{   
    if((vcBuff[VC0706_PTL_BYTE] != VC0706_RECV_MARK) ||
       (vcBuff[VC0706_SERIAL_BYTE] != VC0706_SERIAL_NUMBER)){
        DBG("return format error\n");
        return RESP_DATA_FORMAT_ERROR;
    }

    if(vcBuff[VC0706_CMD_BYTE] != cmd){
        DBG("return command error\n");
        return RESP_DATA_CMD_ERROR;
    }

    return vcBuff[VC0706_STATUS_BYTE]; // 0 is OK, other is NG
}


boolean VC0706::_task(uint8_t cmd[], uint8_t cmdLen, uint8_t respBytes, uint8_t timeOut, boolean flushflag, boolean verify) 
{
    if (flushflag) {
        while(vc->available()){
            uint8_t tmp = vc->read();
        }
    }
    
    _send(cmd, cmdLen);
    
    if (_read(respBytes, timeOut) != respBytes) {
        DBG("read error");
        return false;
    }
    
    if(verify){
        if (_verify(cmd[0])){
            DBG("verify error");
            return false;
        }
    }
    return true;
}

void VC0706::begin(VC0706_BaudRate baudRate) 
{
    vc->begin(DEFAULT_BAUDRATE);
    resetBaudrate(baudRate);
    vc->begin(baud);
#if TRANSFER_BY_SPI
	pinMode(SLAVE_PIN, OUTPUT);
	digitalWrite(SLAVE_PIN,HIGH);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
	SPI.begin(); 
#endif
}

uint8_t VC0706::available(void) 
{
    return bufferLen;
}

char* VC0706::getVersion(void) 
{
    uint8_t cmd[] = {VC0706_GET_VERSION,0x00}; //0x56+0x00+0x11+0x00
    _send(cmd, sizeof(cmd));
    if(!_read(CAMERA_BUFF_SIZE/4,DEFAULT_TIMEOUT)){ //0x76+0x00+0x11+0x00+...
        return 0;
    }
    vcBuff[bufferLen] = 0; 
    return (char *)(vcBuff+5);
}

boolean VC0706::resetBaudrate(VC0706_BaudRate rate)
{
    uint8_t S1RELH;
    uint8_t S1RELL;
    
    switch (rate){
        case BaudRate_9600:
            S1RELH = 0xAE;
            S1RELL = 0xC8;
			baud = 9600;
            break;
        case BaudRate_19200:
            S1RELH = 0x56;
            S1RELL = 0xE4;
			baud = 19200;
            break;
        case BaudRate_38400:
            S1RELH = 0x2A;
            S1RELL = 0xF2;
			baud = 38400;
            break;
        case BaudRate_57600:
            S1RELH = 0x1C;
            S1RELL = 0x4C;
			baud = 57600;
            break;
        case BaudRate_115200:
            S1RELH = 0x0D;
            S1RELL = 0xA6;
			baud = 115200;
            break;
        default : //19200
            S1RELH = 0x56;
            S1RELL = 0xE4;
			baud = 19200;
            break;  
    }
    uint8_t cmd[] = {VC0706_SET_PORT,0x03,0x01,S1RELH,S1RELL};
    return _task(cmd, sizeof(cmd), 5);
}


boolean VC0706::resetSystem(void) 
{
    uint8_t cmd[] = {VC0706_SYSTEM_RESET, 0x00};//0x56+0x00+0x26+0x00
    return _task(cmd, sizeof(cmd), 5);//0x76+0x00+0x26+0x00+0x00 
}

uint8_t VC0706::getImageSize(void) 
{
    uint8_t cmd[] = {VC0706_READ_DATA, 0x04, 0x04, 0x01, 0x00, 0x19};
    if (!_task(cmd, sizeof(cmd), 6))
        return -1;
    return vcBuff[5];
}

boolean VC0706::setImageSize(uint8_t size) 
{
    uint8_t cmd[] = {VC0706_WRITE_DATA, 0x05, 0x04, 0x01, 0x00, 0x19, size};
    return _task(cmd, sizeof(cmd), 5);
}

boolean VC0706::takePicture(void) 
{
    uint8_t cmd[] = {VC0706_FBUF_CTRL, 0x01, VC0706_STOPCURRENTFRAME};
    framePosition = 0;
    return _task(cmd, sizeof(cmd), 5);
}

boolean VC0706::resumeVideo(void) 
{
    uint8_t cmd[] = {VC0706_FBUF_CTRL, 0x01, VC0706_RESUMEFRAME};
    return _task(cmd, sizeof(cmd), 5);
}

void VC0706::getPicture(uint16_t length) 
{
#if TRANSFER_BY_SPI
	uint8_t cmd[] = {VC0706_READ_FBUF, 0x0C, 0x00, 0x0F, 0x00, 0x00, 0x00, 
                       0x00, 0x00, 0x00, length >> 8, length & 0xFF, 0x00, CAMERADELAY & 0xFF};	
	_send(cmd, sizeof(cmd));
	delay(5);
#endif //nothing to do with uart transfer mode
}

uint8_t* VC0706::readPicture(uint8_t length)
{
#if TRANSFER_BY_SPI
	digitalWrite(SLAVE_PIN, LOW);
	bufferLen = 0;
	uint8_t tmp = SPI.transfer(0);
	while(bufferLen < length){
		vcBuff[bufferLen++] = SPI.transfer(0);
	}
	digitalWrite(SLAVE_PIN, HIGH);
	return vcBuff;
#else // transfer by serial
    uint8_t cmd[] = {VC0706_READ_FBUF, 0x0C, 0x0, 0x0A, 0, 0, framePosition >> 8, 
                     framePosition & 0xFF, 0, 0, 0, length, CAMERADELAY >> 8, CAMERADELAY & 0xFF};
                     
    if (!_task(cmd, sizeof(cmd), 5, DEFAULT_TIMEOUT/4, false))
        return 0;

    if (_read(length+5, CAMERADELAY) == 0) 
        return 0;
        
    framePosition += length;
    return vcBuff;
#endif
}

boolean VC0706::motionDetected(void) 
{
    if (_read(4, 200) != 4) {
        return false;
    }
    if (!_verify(VC0706_COMM_MOTION_DETECTED))
        return false;
    return true;
}

uint8_t VC0706::setMotionStatus(uint8_t x, uint8_t d1, uint8_t d2) 
{
    uint8_t cmd[] = {VC0706_MOTION_CTRL, 0x03, x, d1, d2};
    return _task(cmd, sizeof(cmd), 5);
}

uint8_t VC0706::getMotionStatus(uint8_t x) 
{
    uint8_t cmd[] = {VC0706_MOTION_STATUS, 0x01, x};
    return _task(cmd, sizeof(cmd), 5);
}

boolean VC0706::setMotionDetect(boolean flag) 
{
    if (! setMotionStatus(VC0706_MOTIONCONTROL, VC0706_UARTMOTION, VC0706_ACTIVATEMOTION))
        return false;
    uint8_t cmd[] = {VC0706_COMM_MOTION_CTRL, 0x01, flag};
    _task(cmd, sizeof(cmd), 5);
}

boolean VC0706::getMotionDetect(void) 
{
    uint8_t cmd[] = {VC0706_COMM_MOTION_STATUS, 0x00};
    if (! _task(cmd, sizeof(cmd), 5))
        return false;
    return vcBuff[5];
}

uint8_t VC0706::getDownsize(void) 
{
    uint8_t cmd[] = {VC0706_DOWNSIZE_STATUS, 0x0};
    if (! _task(cmd, sizeof(cmd), 6)) 
        return -1;
    return vcBuff[5];
}

boolean VC0706::setDownsize(uint8_t size) 
{
    uint8_t cmd[] = {VC0706_DOWNSIZE_CTRL, 0x01, size};
    return _task(cmd, sizeof(cmd), 5);
}

boolean VC0706::enableTVOutput(bool enable) 
{
    uint8_t cmd[] = {VC0706_TV_OUT_CTRL, 0x01, enable};
    return _task(cmd, sizeof(cmd), 5);
}

uint32_t VC0706::getFrameLength(void) 
{
    uint8_t cmd[] = {VC0706_GET_FBUF_LEN, 0x01, 0x00};
    if (!_task(cmd, sizeof(cmd), 9))
        return 0;
    uint32_t length = (vcBuff[5]<<24)|(vcBuff[6]<<16)|(vcBuff[7]<<8)|vcBuff[8];
    return length;
}

void VC0706::debugBuff() 
{
    for (uint8_t i = 0; i< bufferLen; i++) {
        Serial.print(" 0x");
        Serial.print(vcBuff[i], HEX); 
    }
    Serial.println();
}