/*
    Copyright (C)2011 Kostas Tamateas <nosebleedKT@gmail.com>
    This program is distributed under the terms of the GNU
    General Public License, version 2. You may use, modify,
    and redistribute it under the terms of this license.
    A copy should be included with this source.
*/

#include "Cam.h"

Fat16 pictureFile;
char PICname[] = "PIC000.JPG";

CAM::CAM()
{
}

boolean CAM::setup(char *resolution, long int baudrate)
{ 
  // Get last picture id from eeprom
  byte a = EEPROM.read(0);
  byte b = EEPROM.read(1);   
  PICid = b << 8 | a & 0xFF;
  
  // Roll back picture naming if we reach 512 files
  if(PICid > 512)
  {
    PICid = 0;
    EEPROM.write(0, 0);
    EEPROM.write(1, 0);  
  }    
  
  Serial1.begin(38400);
  SetBaudRate(baudrate);
  SetImageSize(resolution);
  Reset();  
  delay(3000); 
  
  if(Serial1.available()>0) 
    return true;
  else
    return false;
}

void CAM::shoot(char *time, char *lat, char *lon, char *alt)
{  
   snprintf(PICname,11,"PIC%03d.JPG",++PICid);
   
   if(DEBUG_ENABLE)
   {
     DEBUG.print("Shooting & Saving.. ");  
     DEBUG.print(PICname);
   }
          
   pictureFile.open(PICname, O_CREAT | O_WRITE);
   if (!pictureFile.isOpen()) 
   {
     if(DEBUG_ENABLE)
       DEBUG.println("..Picture file not created. Quit shooting!");
     return;
   }
   
   StartShooting(); 
     
   // Wait for Interval Time
   delay(readDelay);
  
   // Skip camera messages
   while(Serial1.available())
   {
     Serial1.read();
   }
   
   curAddr = 0;    
   jpegEnd = false;   
   unsigned long startTime = millis(); 

   while(!jpegEnd)
   {  
      j=0;

      // Very critical part.
      // A 640x480 image transfered at max baudrate(115200) and written on microSD takes about 15sec.
      // Increase ImgTimeout if you operate at lower baudrates 
      if (millis() - startTime > 15000) 
      { 
        if(DEBUG_ENABLE)
          DEBUG.println("Timed out"); 
        break;
      }
      
      // Request data
      ReadData();       

      // Wait for Interval Time
      delay(readDelay);
          
      // Read Request-data response
      b = 0;
      while(Serial1.available() && b<5)
      {
        Serial1.read();
        b++;
      }
      
      // Copy jpeg chunk from camera's internal buffer to avr buffer
      // Chunk size is always 128 byte
      while(Serial1.available())
      {
        b = Serial1.read();
        if((j<chunkSize)&&(!jpegEnd))
        {
           chunk[j++] = b;
       
           // Check if the jpeg is over
           if((chunk[j-2]==0xFF)&&(b==0xD9))
           {
             StopShooting(); 
             jpegEnd = true; 
           }  
         }
      }
      
      // Write jpeg chunk from avr buffer to microSD
      pictureFile.write(chunk, j);          

      /* Print chunk
      for(byte i=0;i<j;i++)
      {   
        if(chunk[i]<0x10)
          DEBUG.printChar('0');
        DEBUG.printHEX(chunk[i]);
      }                  
      DEBUG.println();
      */ 
   } 
  
  // Inject flight data
  if(jpegEnd)    
  {
     strcpy(gpsdata, "@@");
     strcat(gpsdata, time);
     strcat(gpsdata, ",");
     strcat(gpsdata, lat);
     strcat(gpsdata, ",");
     strcat(gpsdata, lon);
     strcat(gpsdata, ",");
     strcat(gpsdata, alt);
     strcat(gpsdata, "@@");
     
    // [FF D8] [FF FE] [size    comments] <-- Field Name
    // ---2--- ---2--- ---2--- ----34---- <-- Field capacity
    // 0     1 2     3 4     5 6       42 <-- Field start / end position in the jpeg header
    
    // FF D8 is already written
    // FF FE is already written
    // SIZE is already written. Size includes itself and comments.
    // So move on the 7th byte
    pictureFile.seekSet(6);
    
    // Write comment string
    // @@2059,xx.xxxx,yy.yyyy,zzzzzzzzz@@
    pictureFile.write(gpsdata);  

    if(DEBUG_ENABLE)
      DEBUG.println(" Ok");     
  }  
   
  pictureFile.close();  

  // Save last picture id on eeprom   
  EEPROM.write(0, PICid & 0xFF);
  EEPROM.write(1, (PICid >> 8) &  0xFF);    
}

// Send get file size command
void CAM::GetFileSize()
{
      // Command [56 00 34 01 00]
   
      // Returns --> 76 00 34 00 04 00 00 XH XL
      // XH XL is the file length of JPEG file
   
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x34);
      Serial1.write(0x01);
      Serial1.write(0x00);
}

//Send Reset command
void CAM::Reset()
{
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x26);
      Serial1.write(0x00);  
}

//Send take picture command
void CAM::StartShooting()
{
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x36);
      Serial1.write(0x01);
      Serial1.write(0x00);  
}

//Read data
void CAM::ReadData()
{  
      MH = (curAddr >> 8) & 0xFF;
      ML = (curAddr >> 0) & 0xFF;
      LH = (chunkSize >> 8) & 0xFF;
      LL = (chunkSize >> 0) & 0xFF;
      DH = (readDelay >> 8) & 0xFF;
      DL = (readDelay >> 0) & 0xFF;
      
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x32);
      Serial1.write(0x0c);
      Serial1.write(0x00); 
      Serial1.write(0x0a);
      Serial1.write(0x00);
      Serial1.write(0x00);
      Serial1.write(MH);
      Serial1.write(ML);   
      Serial1.write(0x00);
      Serial1.write(0x00);
      Serial1.write(LH);
      Serial1.write(LL);
      Serial1.write(DH);  
      Serial1.write(DL);
      curAddr += chunkSize; // address increases according to chunk size
}

void CAM::StopShooting()
{
    Serial1.write(0x56);
    Serial1.write(0x00);
    Serial1.write(0x36);
    Serial1.write(0x01);
    Serial1.write(0x03);        
}

void CAM::SetImageSize(char *resolution)
{
   // Command + Reset [56 00 26 00]
   
   // 56 00 31 05 04 01 00 19 00 --> VGA=640x480
   // 56 00 31 05 04 01 00 19 11 --> QVGA=320x240
   // 56 00 31 05 04 01 00 19 22 --> QQVGA=160x120
   
   // Returns --> 76 00 31 00 00
   
   byte res;
   
   if(strcmp(resolution, "VGA") == 0)
     res = 0x00;
   else if(strcmp(resolution, "QVGA") == 0)
     res = 0x11;
   else if(strcmp(resolution, "QQVGA") == 0)
     res = 0x22;
   else
     res = 0x00; 
   
   Serial1.write(0x56);
   Serial1.write(0x00);
   Serial1.write(0x31);
   Serial1.write(0x05);
   Serial1.write(0x04);
   Serial1.write(0x01);
   Serial1.write(0x00);
   Serial1.write(0x19);
   Serial1.write(res);
}

void CAM::SetCompressRatio()
{
  // Command + Ratio (last hex number 00 to FF)
  // 56 0 31 05 01 01 12 04 36 
  
  // Returns --> 76 00 31 00 00 36
  
   Serial1.write(0x56);
   Serial1.write(0x00);
   Serial1.write(0x31);
   Serial1.write(0x05);
   Serial1.write(0x01);
   Serial1.write(0x01);
   Serial1.write(0x12);
   Serial1.write(0x04);
   Serial1.write(0x00);    
  
   delayMicroseconds(1000 * 500); 
}

void CAM::EnterPowerSave()
{
  // Command + Enable byte (last hex number)
  // 56 00 3E 03 00 01 01 --> enter power save
  
  // Returns --> 76 00 3E 00 00
}

void CAM::ExitPowerSave()
{
  // Command + Disable byte (last hex number)
  // 56 00 3E 03 00 01 00 --> exit power save
  
  // Returns --> 76 00 3E 00 00
}

void CAM::SetBaudRate(long int baudrate)
{  
  // Command + XX YY (two last hex numbers)
  // 56 00 24 03 01 1C 4C --> 57600 baud

  // AE C8 9600
  // 56 E4 19200
  // 2A F2 38400
  // 1C 4C 57600
  // 0D A6 115200
  // 06 53 230400
  
  // Returns --> 76 00 24 00 00
  
  byte XX;
  byte YY;
  
  switch(baudrate)
  {
   case 9600:
     XX = 0xAE;
     YY = 0xC8;        
     break;
   case 19200:
     XX = 0x56;
     YY = 0xE4;          
     break;  
   case 38400:
     XX = 0x2A;
     YY = 0xF2;              
     break; 
   case 57600:
     XX = 0x1C;
     YY = 0x4C;        
     break;         
   case 115200:
     XX = 0x0D;
     YY = 0xA6;         
     break;   
   default:
     XX = 0x2A;
     YY = 0xF2;         
     break;      
   }
   
   Serial1.write(0x56);
   Serial1.write(0x00);
   Serial1.write(0x24);
   Serial1.write(0x03);
   Serial1.write(0x01);
   Serial1.write(XX);
   Serial1.write(YY);   
   
   Serial1.end();
   Serial1.begin(baudrate); 
}