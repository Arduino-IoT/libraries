/*

  Chess.pde
  
  Little Rook Chess
  
example code modify by Arthur

www.kaise.com.tw
www.buyic.com.tw
--------------------------------------------------------------------------------------


  >>> Before compiling: Please remove comment from the constructor of the 
  >>> connected graphics display (see below).
  
  Universal 8bit Graphics Library, http://code.google.com/p/u8glib/
  
  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  

*/


#include "U8glib.h"

// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported 
// devices with all constructor calls is here: http://code.google.com/p/u8glib/wiki/device

U8GLIB_KSM110_128X64_SPI u8g(U8G_I2C_OPT_NONE);


// DOGS102 shield configuration values
uint8_t uiKeyPrev = 2;
uint8_t uiKeyNext = 4;
uint8_t uiKeySelect = 5;
uint8_t uiKeyBack = 3;

// DOGM128-Shield	 configuration values
// DOGXL60-Shield configuration values
//uint8_t uiKeyPrev = 7;
//uint8_t uiKeyNext = 3;
//uint8_t uiKeySelect = 2;
//uint8_t uiKeyBack = 8;

uint8_t uiKeyCodeFirst = CHESS_KEY_NONE;
uint8_t uiKeyCodeSecond = CHESS_KEY_NONE;
uint8_t uiKeyCode = CHESS_KEY_NONE;


void uiSetup(void) {
  // configure input keys 
  
  pinMode(uiKeyPrev, INPUT);           // set pin to input
  digitalWrite(uiKeyPrev, HIGH);       // turn on pullup resistors
  pinMode(uiKeyNext, INPUT);           // set pin to input
  digitalWrite(uiKeyNext, HIGH);       // turn on pullup resistors
  pinMode(uiKeySelect, INPUT);           // set pin to input
  digitalWrite(uiKeySelect, HIGH);       // turn on pullup resistors
  pinMode(uiKeyBack, INPUT);           // set pin to input
  digitalWrite(uiKeyBack, HIGH);       // turn on pullup resistors
}

void uiStep(void)
{
  uiKeyCodeSecond = uiKeyCodeFirst;
  if ( digitalRead(uiKeyPrev) == LOW )
    uiKeyCodeFirst = CHESS_KEY_PREV;
  else if ( digitalRead(uiKeyNext) == LOW )
    uiKeyCodeFirst = CHESS_KEY_NEXT;
  else if ( digitalRead(uiKeySelect) == LOW )
    uiKeyCodeFirst = CHESS_KEY_SELECT;
  else if ( digitalRead(uiKeyBack) == LOW )
    uiKeyCodeFirst = CHESS_KEY_BACK;
  else 
    uiKeyCodeFirst = CHESS_KEY_NONE;
  
  if ( uiKeyCodeSecond == uiKeyCodeFirst )
    uiKeyCode = uiKeyCodeFirst;
  else
    uiKeyCode = CHESS_KEY_NONE;
}


void setup() {
  // rotate screen, if required
  //u8g.setRot180();
  
  uiSetup();
  chess_Init(u8g.getU8g(), 0);
}

void loop() {  
  uint8_t keyCode = CHESS_KEY_NONE;
  
  u8g.firstPage();
  do {
    chess_Draw();
    uiStep();
    if ( uiKeyCode != CHESS_KEY_NONE )
      keyCode = uiKeyCode;
  } while( u8g.nextPage() );
  
  u8g_Delay(10);
  chess_Step(keyCode);
  uiStep();
  keyCode = uiKeyCode;
}


