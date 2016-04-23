/*
* VC0706.h
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

#ifndef _VC0706_UART_H_
#define _VC0706_UART_H_

#include "Arduino.h"
#include "SoftwareSerial.h"
#include <SPI.h>

#define DEBUG                       1
#define USE_SOFTWARE_SERIAL			1
#define TRANSFER_BY_SPI				1

#if DEBUG
    #define DBG(x)                  Serial.println(x);
#else
    #define DBG(x)
#endif

#define SLAVE_PIN					8
#define DEFAULT_BAUDRATE            38400
#define DEFAULT_TIMEOUT             200
#define RESPONSE_HEAD_BYTE          4

#define VC0706_PTL_BYTE             0x00
#define VC0706_SERIAL_BYTE          0x01
#define VC0706_CMD_BYTE             0x02
#define VC0706_STATUS_BYTE          0x03

#define VC0706_SEND_MARK            0x56
#define VC0706_RECV_MARK            0x76

#define VC0706_GET_VERSION          0x11
#define VC0706_SET_SERIAL_NUMBER    0x21
#define VC0706_SET_PORT             0x24
#define VC0706_SYSTEM_RESET         0x26
#define VC0706_READ_DATA            0x30
#define VC0706_WRITE_DATA           0x31
#define VC0706_READ_FBUF            0x32
#define VC0706_WRITE_FBUF           0x33
#define VC0706_GET_FBUF_LEN         0x34
#define VC0706_SET_FBUF_LEN         0x35
#define VC0706_FBUF_CTRL            0x36
#define VC0706_COMM_MOTION_CTRL     0x37
#define VC0706_COMM_MOTION_STATUS   0x38
#define VC0706_COMM_MOTION_DETECTED 0x39
#define VC0706_MIRROR_CTRL          0x3A
#define VC0706_MIRROR_STATUS        0x3B
#define VC0706_COLOR_CTRL           0x3C
#define VC0706_COLOR_STATUS         0x3D
#define VC0706_POWER_SAVE_CTRL      0x3E
#define VC0706_POWER_SAVE_STATUS    0x3F
#define VC0706_AE_CTRL              0x40
#define VC0706_AE_STATUS            0x41
#define VC0706_MOTION_CTRL          0x42
#define VC0706_MOTION_STATUS        0x43
#define VC0706_TV_OUT_CTRL          0x44
#define VC0706_OSD_ADD_CHAR         0x45
#define VC0706_DOWNSIZE_CTRL        0x54
#define VC0706_DOWNSIZE_STATUS      0x55
#define VC0706_GET_FLASH_SIZE       0x60
#define VC0706_ERASE_FLASH_SECTOR   0x61
#define VC0706_ERASE_FLASH_ALL      0x62
#define VC0706_READ_LOGO            0x70
#define VC0706_SET_BIZTMAP          0x71
#define VC0706_BATCH_WRITE          0x80


#define VC0706_STOPCURRENTFRAME     0x00
#define VC0706_STOPNEXTFRAME        0x01
#define VC0706_RESUMEFRAME          0x02
#define VC0706_STEPFRAME            0x03

#define VC0706_640x480              0x00
#define VC0706_320x240              0x11
#define VC0706_160x120              0x22

#define VC0706_MOTIONCONTROL        0x00
#define VC0706_UARTMOTION           0x01
#define VC0706_ACTIVATEMOTION       0x01

#define VC0706_SET_ZOOM             0x52
#define VC0706_GET_ZOOM             0x53

#define CAMERA_BUFF_SIZE            100
#define CAMERADELAY                 100

#define VC0706_SERIAL_NUMBER        0x00

enum VC0706_RESPONSE
{
    RESP_OK                 = 0x00,
    RESP_INVALID_CMD        = 0x01,
    RESP_INVALID_CMD_LEN    = 0x02,
    RESP_INVALID_DATA_FORMAT= 0x03,
    RESP_DISABLE_EXEC       = 0x04,
    RESP_EXEC_ERROR         = 0x05,
    
    RESP_DATA_HEAD_ERROR    = 0x06,
    RESP_DATA_FORMAT_ERROR  = 0x07,
    RESP_DATA_CMD_ERROR     = 0x08,
    RESP_DATA_LEN_ERROR     = 0x09
};

enum VC0706_BaudRate
{
    BaudRate_9600           = 0x00,
    BaudRate_19200          = 0x01,
    BaudRate_38400          = 0x02,
    BaudRate_57600          = 0x03,
    BaudRate_115200         = 0x04
};

class VC0706 
{
public:
#if USE_SOFTWARE_SERIAL
    VC0706(SoftwareSerial *serial){
#else
	VC0706(HardwareSerial *serial){
#endif
        vc = serial;
        framePosition = 0;
        bufferLen = 0;
    };
    void begin(VC0706_BaudRate baudRate);
    boolean resetSystem(void);
    boolean resetBaudrate(VC0706_BaudRate rate);
    uint8_t available(void);
    char* getVersion(void);
    uint8_t getImageSize(void);
    boolean setImageSize(uint8_t size); 
    boolean takePicture(void);
    boolean resumeVideo(void);
    void getPicture(uint16_t length); 
	uint8_t* readPicture(uint8_t length);
    boolean motionDetected(void); 
    uint8_t setMotionStatus(uint8_t x, uint8_t d1, uint8_t d2);     
    uint8_t getMotionStatus(uint8_t x);
    boolean setMotionDetect(boolean flag); 
    boolean getMotionDetect(void); 
    uint8_t getDownsize(void); 
    boolean setDownsize(uint8_t size); 
    boolean enableTVOutput(bool enable); 
    uint32_t getFrameLength(void);  
    void debugBuff(void);
      
private:
    uint16_t baud;
    uint8_t vcBuff[CAMERA_BUFF_SIZE];
    uint8_t bufferLen;
    uint16_t framePosition;
#if USE_SOFTWARE_SERIAL
    SoftwareSerial *vc;
#else
	HardwareSerial *vc;
#endif
    boolean _task(uint8_t cmd[], uint8_t cmdLen, uint8_t respBytes, uint8_t timeOut = DEFAULT_TIMEOUT, 
                  boolean flushflag = true, boolean verify = true); 
    void _send(uint8_t cmd[], uint8_t len); 
    uint8_t _read(uint8_t bytes, uint8_t timeOut); 
    uint8_t _verify(uint8_t cmd);
};

#endif
