/*
  Four_Digital_Display.cpp
  
  This is a Suli-compatible Library.
  
  2014 Copyright (c) Seeed Technology Inc.  All right reserved.
  
  Author:Loovee
  2014-4-9
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "Four_Digit_Display.h"

IO_T __pinDta;
IO_T __pinClk;


uint8 Cmd_DispCtrl  = 0;
uint8 _PointFlag    = 0;     //_PointFlag=1:the clock point on


const int8 TubeTab[] =
{
    0x3f,0x06,0x5b,0x4f,
    0x66,0x6d,0x7d,0x07,
    0x7f,0x6f,0x77,0x7c,
    0x39,0x5e,0x79,0x71
}; //0~9,A,b,C,d,E,F

// write a byte
void writeByte(int8 wr_data)
{
    uint8 i,count1;
    for(i=0;i<8;i++)                                  //sent 8bit data
    {
        suli_pin_write(&__pinClk, HAL_PIN_LOW);
        if(wr_data & 0x01)suli_pin_write(&__pinDta, HAL_PIN_HIGH); //LSB first
        else suli_pin_write(&__pinDta,HAL_PIN_LOW);
        wr_data >>= 1;
        suli_pin_write(&__pinClk,HAL_PIN_HIGH);
    }
    suli_pin_write(&__pinClk,HAL_PIN_LOW);                 //wait for the ACK
    suli_pin_write(&__pinDta,HAL_PIN_HIGH);
    suli_pin_write(&__pinClk,HAL_PIN_HIGH);

    suli_pin_dir(&__pinDta, HAL_PIN_INPUT);
    while(suli_pin_read(&__pinDta))
    {
        count1 +=1;
        if(count1 == 200)//
        {
            suli_pin_dir(&__pinDta, HAL_PIN_OUTPUT);
            suli_pin_write(&__pinDta,HAL_PIN_LOW);
            count1 =0;
        }
        suli_pin_dir(&__pinDta, HAL_PIN_INPUT);
    }
    suli_pin_dir(&__pinDta, HAL_PIN_OUTPUT);
}

// send start signal to TM1637
void start(void)
{
    suli_pin_write(&__pinClk, HAL_PIN_HIGH);//send start signal to TM1637
    suli_pin_write(&__pinDta, HAL_PIN_HIGH);
    suli_pin_write(&__pinDta, HAL_PIN_LOW);
    suli_pin_write(&__pinClk, HAL_PIN_LOW);
}

// End of transmission
void stop(void)
{
    suli_pin_write(&__pinClk,HAL_PIN_LOW);
    suli_pin_write(&__pinDta,HAL_PIN_LOW);
    suli_pin_write(&__pinClk,HAL_PIN_HIGH);
    suli_pin_write(&__pinDta,HAL_PIN_HIGH);
}

//
void codingS(int8 *DispData)
{
    uint8 PointData;
    if(_PointFlag == POINT_ON)PointData = 0x80;
    else PointData = 0;
    for(uint8 i = 0;i < 4;i ++)
    {
        if(DispData[i] == 0x7f)DispData[i] = 0x00;
        else DispData[i] = TubeTab[DispData[i]] + PointData;
    }
}

//
uint8 coding(int8 DispData)
{
    uint8 PointData;
    if(_PointFlag == POINT_ON)PointData = 0x80;
    else PointData = 0;
    if(DispData == 0x7f) DispData = 0x00 + PointData;//The bit digital tube off
    else DispData = TubeTab[DispData] + PointData;
    return DispData;
}

// initialize
void four_digit_init(PIN_T data, PIN_T clk)
{

    suli_pin_init(&__pinDta, data);
    suli_pin_init(&__pinClk, clk);

    suli_pin_dir(&__pinDta, HAL_PIN_OUTPUT);
    suli_pin_dir(&__pinClk, HAL_PIN_OUTPUT);

    four_digit_clear();
}

// set brightness
void four_digit_set_brightness(enum brightness_t b)           //set before calling display
{
    Cmd_DispCtrl = 0x88 + b;
}

// show :
void four_digit_set_point(bool disp_point)              //set before calling display
{
    _PointFlag = disp_point;
}

// display a bit
void four_digit_display(uint8 BitAddr,int8 DispData)
{
    uint8 SegData;
    SegData = coding(DispData);
    start();          //start signal sent to TM1637 from MCU
    writeByte(ADDR_FIXED);//
    stop();           //
    start();          //
    writeByte(BitAddr|0xc0);//
    writeByte(SegData);//
    stop();            //
    start();          //
    writeByte(Cmd_DispCtrl);//
    stop();           //
}

// clear display
void four_digit_clear()
{
    four_digit_display(0x00,0x7f);
    four_digit_display(0x01,0x7f);
    four_digit_display(0x02,0x7f);
    four_digit_display(0x03,0x7f);
}
