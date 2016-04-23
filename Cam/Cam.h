/*
  Copyright (C)2011 Kostas Tamateas <nosebleedKT@gmail.com> 
 
  This program is distributed under the terms of the GNU 
  General Public License, version 2. You may use, modify, 
  and redistribute it under the terms of this license. 
  A copy should be included with this source. 
*/

#ifndef __CAM_H__
#define __CAM_H__

#include <WProgram.h>
#include <Fat16.h>
#include <stdio.h>
#include <EEPROM.h>
#include "Settings.h"

const unsigned int chunkSize = 128;
const byte readDelay = 10;

class CAM
{
  public:
    CAM();
    boolean setup(char *, long int);
    void shoot(char *, char *, char *, char *);
  
  private:  
    void GetFileSize();
    void StartShooting();
    void Reset();
    void ReadData();
    void StopShooting();
    void SetImageSize(char *);
    void SetCompressRatio();
    void EnterPowerSave();
    void ExitPowerSave();
    void SetBaudRate(long int);
    
    byte b;
    boolean jpegEnd;
    unsigned int j; 
    long int curAddr;
    byte MH, ML;           // Starting Address
    byte LH, LL;           // Length 
    byte chunk[128];
    unsigned int chunkSize;
    byte DH, DL;           // Delay
    byte readDelay;   
    unsigned int PICid;
    char gpsdata[34];
};

#endif
