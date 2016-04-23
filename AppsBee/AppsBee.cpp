/*
  AppsBee.cpp - ZigBee Library for AppsBee Shield
  Copyright (c) 2014 Simon Kuo/Tony Su.  All right reserved.

  This library is free software and for AppsBee ZigBee shield which 
  is designed by Appsduino Inc. ; you can redistribute it and/or
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
/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 Code by Simon Kuo/Tony Su in TAIWAN
 http://www.appsduino.com
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Arduino.h"
#include "AppsBee.h"
#include <Streaming.h>
#include "SoftwareSerial.h"

TCommand::TCommand (String initStr)
{
  TargetID = 0;
  SourceID = 0;
  CommandID = 0;
  Parameter = 0;
	
	if (initStr=="")
		return;	
	else {
    String ts, ss="";

    int comaPos = initStr.indexOf(',');
    ts = initStr.substring(0, comaPos);
    TargetID = ts.toInt();
    
    initStr = initStr.substring(comaPos+1, initStr.length());
    comaPos = initStr.indexOf(',');
    ts = initStr.substring(0, comaPos);
    CommandID = ts.toInt();
    
    initStr = initStr.substring(comaPos+1, initStr.length());
    comaPos = initStr.indexOf(',');
    ts = initStr.substring(0, comaPos);
    Parameter = ts.toInt();
  }
}

//========================================================================

AppsBee::AppsBee(uint8_t RX, uint8_t TX, uint8_t Role)
{		
	swSerial = new SoftwareSerial(RX, TX);
	ts = swSerial;
	hwSerial = NULL;

  _role = Role;

  if (_role == CO) {   // CO:0  RO:1
		TX_HEADER[0] = 0xAA;
		TX_HEADER[1] = 0xFF;  //代表廣播位址
		TX_HEADER[2] = 0xFF;  //代表廣播位址
		TX_HEADER[3] = 0x55;
		
  	UniMyID = 0;  //CO 端 ID 固定為 0
	} else {
		TX_HEADER[0] = 0xAA;
		TX_HEADER[1] = 0x00;  
		TX_HEADER[2] = 0x00;
		TX_HEADER[3] = 0x55;
		
  	UniMyID = -1; //RO 端 ID 等到呼叫 begin()  之後才能得知
	}
}

AppsBee::AppsBee(HardwareSerial &h, uint8_t Role)
{
	swSerial = NULL;
	hwSerial = &h;
	ts = hwSerial;

 //-- 0529 --	
  _role = Role;
  
  if (_role == CO) {   // CO:0  RO:1
		TX_HEADER[0] = 0xAA;
		TX_HEADER[1] = 0xFF;  //代表廣播位址
		TX_HEADER[2] = 0xFF;  //代表廣播位址
		TX_HEADER[3] = 0x55;
		
  	UniMyID = 0;  //CO 端 ID 固定為 0
	} else {
		TX_HEADER[0] = 0xAA;
		TX_HEADER[1] = 0x00;  
		TX_HEADER[2] = 0x00;
		TX_HEADER[3] = 0x55;
		
  	UniMyID = -1; //RO 端 ID 等到呼叫 begin()  之後才能得知
	}  
//-- 0529 --
}

void AppsBee::begin(long speed)
{
	//-------------------------
  pinMode(M01,OUTPUT) ;
  pinMode(RESET,OUTPUT) ;  
  digitalWrite(RESET,HIGH ) ;  //---------------
  digitalWrite(M01,HIGH) ; //  
  delay(2000) ;
  digitalWrite(RESET,LOW) ;
  delay(2) ;
  digitalWrite(RESET,HIGH) ;

	//-------------------------
	_state = SYNC;
  _DataOffset = 0 ;// Received data buffer offset 
  _checksum = 0 ;// Received data buffer offset 

	_speed = speed;
	if (swSerial)
		swSerial->begin(_speed);
	else if (hwSerial)
		hwSerial->begin(_speed);
	else
		;  //應該要丟出錯誤訊息
		
	delay(1500) ;	// RO : delay > 500ms,CO : delay > 1500ms
	
	if (_role==CO)
	  UniMyID = 0;    // 自己的 ID: CO必為0
	else  //(_role==RO)
//		;
	  UniMyID = AppsBeeUniIDread(); //取得自己的 ID: RO已經預先燒錄在晶片內  CO必為0
}
//------------------ simulate Serial functions ------------------

size_t AppsBee::print(const String &ss)
{
	//return swSerial->print(ss);
  return ts->print(ss);
}

//0528-------------------------------------
size_t AppsBee::print(const char str[])
{
  return ts->print(str);
}
size_t AppsBee::print(char c)
{
  return ts->print(c);
}
size_t AppsBee::print(unsigned char b, int base)
{
  return ts->print(b, base);
}
size_t AppsBee::print(int n, int base)  
{
  return ts->print(n, base);
}
size_t AppsBee::print(unsigned int n, int base)
{
  return ts->print(n, base);
}
size_t AppsBee::print(long n, int base)
{
  return ts->print(n, base);
}
size_t AppsBee::print(unsigned long n, int base)
{
  return ts->print(n, base);
}
//0528-------------------------------------


int AppsBee::read()
{
	return ts->read();
}
 
size_t AppsBee::write(const uint8_t *buffer, size_t size)
{
	return ts->write(buffer, size);
} 

size_t AppsBee::write(uint8_t byte)
{
  return ts->write(byte);	
}

int AppsBee::available()
{
	return ts->available();
}


//-------- Read RO Source Address ID --------------
//  return Unicast source address(Valid : 1,.. 2047, -1 : not valid)
//-------------------------------------------------
int AppsBee::AppsBeeUniIDread() {
  
  String SID = "", ss="";
  int val ; // incoming data
  
  delay(1000) ; // wait for 1 sec
  print("UNI_SEC_ADDR") ;
  
  while(!available()) ;
  
  while(available()) { // Response "UNI_SEC_ADDR: #", # : -1 or 1,.. 2047
     val = read() ;
     ss += char(val) ;
     if((val == 0x2D) or (val >= 0x30 && val <= 0x39))  // check val if = "-" or "0","1",..."9" 
        SID += char (val) ; 
     delay(2) ; // delay 1ms wait for data available(9600 bps)
   } 
#ifdef KDEBUG
  (*pSer) << "read ss=" << ss << endl;
  (*pSer) << "read SID=" <<  SID << endl;
#endif

   return SID.toInt(); // return UniID
} 

//-------- RO ID Address Setting --------------
//  setting Address/設定位址(Command Format : UNI_SEC_ADDR #)
//  # : RO可設定有效位址 : 1,2, ~~ 2047(11 bits)
//  return Unicast source address(Valid : 1,.. 2047, -1 : not valid)
//-------------------------------------------------
int AppsBee::AppsBeeUniIDWrite(int RO_UniID) {
 
	if (_role==CO)
	  return 0 ;    // 自己的 ID: CO必為0

  if(RO_UniID == -1 || (RO_UniID > 0 && RO_UniID < 2048)) { 
    print("UNI_SEC_ADDR ") ;
    print(RO_UniID) ;   //0528
    	
    while(!available()) ;// waiting for Response "UNI_SEC_ADDR: #", # : -1 or 1,.. 2047
    while(available()) { // Response "UNI_SEC_ADDR: #", # : -1 or 1,.. 2047
 	     read() ;
 	     delay(2) ;          // flush out the response chars
    }
    digitalWrite(RESET,LOW) ;						 // reset to activate the setting
    delay(2) ;
    digitalWrite(RESET,HIGH) ;
    
    UniMyID = AppsBeeUniIDread() ;
  }  
  return UniMyID ; // return new RO ID
} 

//---------- define Mode Setting Command -----------------
const char *AppsBee::ModeCommand[] = 
  						{ "PANID ",   			// PANID : 1 ~ 65535,default : 4372　
								"BAUD_RATE ",			// Baud rate : 1200/2400/.../115200, default : 9600
					      "TX_POWER ",      // TX Power : 0 ~ 21,default : 21
								"CHANNEL ",       // Channel(4 bytes,bit mapping) : 11 ~ 26,default : 11(8192)
								"ANNCE_RATIO ",   // ANNCE Ratio : 1 ~ 32767,default : 600
								"UNI_SEC_ADDR ",  // RO ID Setting : 1,..2047(Valid),default : -1(Invalid)
								"PW_RESET "				// Software Reset : Reset if any number
							} ;		
																						
//-------- AppsBee Setting Mode Method --------------
//  mode : setting mode from enum settingType in AppsBee.h
//  Setting data : 
//          0/long : 0 - inquiry(查詢指令), long - setting data
//          data   : the setting data to AppsBee module
//  return  :  response message from AppsBee
//-------------------------------------------------
char *AppsBee::SettingMode(settingType mode,long SettingData) {
 	
	int i=0 ;
	char *Err_message = "*Invalid Data*" ;

	if(SettingData == 0) { // inquiry command
		print(ModeCommand[mode]) ;
			
		while(!available()) ;// waiting for Response 
    while(available()) {   // Response available ?
 	     ResponseBuffer[i++] = read() ; // read one byte response message
 	     delay(2) ; 
 	  }	
    ResponseBuffer[i] = '\0' ; // NULL terminator
    
    return ResponseBuffer ; // return the address of message
  }
  else { // Mode command Setting
  	switch(mode) {
  		case PANID :
  			if(SettingData < 1 || SettingData > 65535)
  				return Err_message ;
  		break ;	
  			
  		case BAUD_RATE :
  			if(!BaudRate_Check(SettingData))
  				return Err_message ; 				 
  		break ;	
  			
  		case TX_POWER :
  			if(SettingData < 0 || SettingData > 21)
  				return Err_message ;  				 
  		break ;
  				
  		case CHANNEL : // Channel 11 ~ 26(single channel) or >= 2048(multiple channel),2048 = 0x800(channel 11)
  			if(SettingData < 11 || (SettingData > 26 && SettingData < 2048)) 
  				return Err_message ;
  			if(SettingData > 10 && SettingData < 27) // Single channel setting(CH 11 ~ 26)
  				SettingData = (long) 0x800 << (SettingData -11) ; // left shift to relative channel bit  				 				 
  		break ;	
  			
  		case ANNCE_RATIO :
  			if(SettingData < 1 || SettingData > 32767)
  				return Err_message ;  				 
  		break ;	
  			
  		case UNI_SEC_ADDR :
  			if(SettingData < -1 || SettingData > 2047 || _role==CO) // CO ID: 必為0)
  				return Err_message ;  				 
  		break ;	
  			
  		case PW_RESET :  // any number will be ok and Reset module
  		break ;	
  	}

		print(ModeCommand[mode]) ;
		print(SettingData) ;  //0528  (long)
			
		while(!available()) ;// waiting for Response 
    while(available()) {   // Response available ?
 	     ResponseBuffer[i++] = read() ; // read one byte response message
 	     delay(2) ; 
 	  }	
    ResponseBuffer[i] = '\0' ; // NULL terminator

    digitalWrite(RESET,LOW) ;						 // reset to activate the setting
    delay(2) ;
    digitalWrite(RESET,HIGH) ;
    
    if(mode == UNI_SEC_ADDR)
    	UniMyID = (int) SettingData ;  // update the RO ID address : UniMyID
    
    return ResponseBuffer ; // return the address of message 		  	
  }	
} 

//-------- Baud Rate checking(1200/2400,..115200)  --------------
//  rate  : input baud rate
//  return true if valid,false if invalid
//-------------------------------------------------
boolean AppsBee::BaudRate_Check(long rate) {
	
const long BaudRate_Table[] = {1200,2400,4800,9600,19200,38400,57600,115200} ;
	
		for(int i=0;i < 8;i++)
		  if(rate == BaudRate_Table[i])
		  	return true ;
		  	
		return false ;  	
}	

/*------------------ AppsBee Send Frame and Read Frame( < 64 Bytes) ----------------
   Tips(data type) : char : signed data type(-128 ~ 127)
                     byte : equal unsigned char(0 ~ 255)
   Data Frame format : 
         7E :   Preamble(Start Tx)
         LL    : Paid Load Length after LL field(250 data Bytes maximum)
         CC    : Cmd Flag
         SS SS : Source address ---> come from UniMyID(Global var : read from AppsBee Module)
         DD DD,.. : Real raw Data
         MM    : checksum(1 Byte)
   Cmd flag/type :
   			    _isGCommand : General Command  
  					_isString   : String  
  					_isInt      : Integer  
  					_isFloat    : Float
*/

void AppsBee::AppsBeeSendCommand(byte CMD, char *Data, int DataLength)
{
  //char *Data=DataTxBuffer;
  byte checksum = 0 ;
  byte TX_StartFrame[5] = {0x7E,0,0,0,0} ; // Start Frame : 0x7E,LL,CC,SS,SS
  
  TX_StartFrame[1] = byte (DataLength + 3) ; // CMD(1 Byte) + SOurce ID(2 Bytes)
  TX_StartFrame[2] = CMD ;
  TX_StartFrame[3] = (byte) (UniMyID/256) ; // MSB of Source ID
  TX_StartFrame[4] = (byte) UniMyID ;      // LSB of Source ID
  checksum = CMD + TX_StartFrame[3] + TX_StartFrame[4] ;
  for (int i=0;i<DataLength;i++)
     checksum += Data[i] ;
  
  checksum = (byte) 0xFF - checksum ;
  
  write(TX_HEADER,4) ; // transmit destination Header first 
  write(TX_StartFrame,5) ;    
  write((byte *)Data,DataLength) ; 
  write(checksum) ; 
}

/*------------------ AppsBee Read Frame and Read Frame( < 64 Bytes) ----------------
   Data Frame format : 
         7E :   Preamble(Start Tx)
         LL    : Paid Load Length after LL field(256 data Bytes maximum)
         CC    : Cmd flag
         SS SS : Source address
         DD DD,.. : Real raw Data
         MM    : checksum(1 Byte)
   Cmd flag/type :
   			    _isGCommand : General Command  
  					_isString   : String  
  					_isInt      : Integer  
  					_isFloat    : Float
   return :
   
         -1 : no complete data frame received
         bytecount : complete frame received and return the bytes number(CMD + Source ID + DATA frame)
*/
boolean AppsBee::AppsBeeEncode(char c)
{
  boolean valid_frame = false ;
  
  //------------------
  switch(_state) {
    case SYNC :
      if(c == (char) 0x7E)
         _state = PREAMBLE ; // 若狀態SYNC 且又收到0x7E，代表收到PREAMBLE訊號，狀態機進入PREAMBLE狀態
      return valid_frame ;
    case PREAMBLE :   // 若狀態PREAMBLE  收到byte代表後續資料長度，狀態機進入DATALENRCVED 連續接收資料狀態
      _DataLength = (int) c ;
            
      _state = DATALENRCVED ; // received data length byte
      _DataOffset = 0 ; // initialize offset of Data received buffer
      _checksum = 0 ;
      return valid_frame ;
    case DATALENRCVED : // 若狀態DATALENRCVED  接下來要連續接收_DataLength數量的資料
                        //   收足資料後  狀態機進入SYNC 接收資料完成狀態
      _checksum += c ; // calculate checksum
      if(_DataOffset < RX_BUF_SIZE)  //0530
         DataRxBuffer[_DataOffset++] = c ;// DataRxBuffer[0] = CMD,[1],[2] : sourceID,[3],..[xx] : DATA message(ASCII string)
      if(_DataOffset > _DataLength) {
        if(_checksum == (byte) 0xFF) 
          valid_frame = true ;
        _state = SYNC ;  
      }
    default :   
      return valid_frame ;
    }
}

// 取得完整的 data frame 放到 DataRxBuffer內，並且於_DataLength 紀錄有效資料的長度
// 檢查是否已經收集到完整的 frame

boolean AppsBee::receive()
{
  if (!available())
    return false;
  
	char incomingByte;
	boolean valid_frame = false;
  long _t1 = millis(); // 0.2s timeout control  
  
	while (available()) { //處理 serial 緩衝區內的資料
		if ( millis()-_t1>=200)  //時間保護機制 0.2秒
        break;  // 1 sec time out
		        
    incomingByte = read(); //讀取byte資料
    valid_frame = AppsBeeEncode(incomingByte);  //將byte資料轉交由AppsBeeEncode負責組合成frame

    // 若是已經收集到完整的frame，
    // 呼叫Get_DataFrame  取出資料放到緩衝區內，並且拆解命令格式
    // 並解脫離 while 迴圈
    if (valid_frame) {
      Get_DataFrame(); //此行執行後 AZbee.RxCmd 會紀錄收到的frame是屬於第幾號格式
      break;
    }
  }
	return valid_frame; 
}

//------------- Get the Data from DataRxBuffer --------------
// return cmd
byte AppsBee::Get_DataFrame()
{
	RxCmd = (byte)DataRxBuffer[0] ;
	RxDatalen = _DataLength - 3 ;
	RxSourceID = DataRxBuffer[1] * 256 + (byte) DataRxBuffer[2] ;
	RxData =  &DataRxBuffer[3] ;	

	switch(RxCmd) {
    case _isGCommand : //傳送一個通用指令框
  		getGCommand();
  		break;
    case _isString :	 //傳送一個字串	  
  		getString();
  		break;
    case _isInt :		 //傳送一個整數
  		getInt();
  		break;
    case _isFloat :	 //傳送一個浮點數  
  		getFloat();
  		break;
  	default:
  	  break;
  };

	return RxCmd;
}

//-------------------------------------
// 通用數位控制指令
/*
		byte TargetID;
		byte SourceID;
		byte CommandID;
		byte Parameter;
*/
void AppsBee::SendGCommand(int _TargetID, int _SourceID, byte _CommandID, byte _Parameter)
{
  char *dataPtr = DataTxBuffer;

  *((int *)(DataTxBuffer+0)) = _TargetID;  //DataTxBuffer[0] = cmd.TargetID;
  //DataTxBuffer[0] = cmd.TargetID >> 8;
  //DataTxBuffer[1] = cmd.TargetID & 0x00FF;
  
  *((int *)(DataTxBuffer+2)) = _SourceID;  //DataTxBuffer[2] = cmd.SourceID;
  //DataTxBuffer[2] = cmd.SourceID >> 8;
  //DataTxBuffer[3] = cmd.SourceID & 0x00FF;

  DataTxBuffer[4] = _CommandID;
  DataTxBuffer[5] = _Parameter;
  
  TX_HEADER[1] = _TargetID >> 8;  
  TX_HEADER[2] = _TargetID & 0x00FF;//改成傳送指定單點模式
	AppsBeeSendCommand(6, DataTxBuffer, 6);    //6號命令號碼, 送出 6byte 數
	TX_HEADER[1] = 0XFF;  TX_HEADER[2] = 0XFF;  //改回預設廣播模式
		
}

void AppsBee::SendGCommand(int TargetID, TCommand cmd)
{
	SendGCommand(TargetID, cmd.SourceID, cmd.CommandID, cmd.Parameter);
} 

TCommand AppsBee::getGCommand()
{
	char * dataPtr = DataRxBuffer+3; //前3byte不管，從第4byte開始讀取 4 byte 為第6號指令封包格式
	
	TCommand cmd;
  //cmd.TargetID   = (int)dataPtr[0]*256 + dataPtr[0];
  cmd.TargetID = *((int *)(dataPtr)); 

  //cmd.SourceID   = (int)dataPtr[2]*256 + dataPtr[3];
  cmd.CommandID  = dataPtr[4];
  cmd.Parameter  = dataPtr[5];
	return cmd;  
}

//-------------------------------------
// 第 7 號指令封包格式  通用傳輸字串指令
// void AppsBee::getString (int TargetID, String value)
/*
		byte TargetID;
		byte SourceID;
		byte CommandID;
		byte Parameter;
*/
void AppsBee::SendString(int TargetID, String value)
{
  char *dataPtr = DataTxBuffer;
  DataTxBuffer[0] = (byte)value.length();
  value.toCharArray(DataTxBuffer+1, value.length()+1);
  
  TX_HEADER[1] = TargetID >> 8;  
  TX_HEADER[2] = TargetID & 0x00FF;//改成傳送指定單點模式

	AppsBeeSendCommand(_isString, DataTxBuffer, value.length()+1+1);    //7號命令號碼, 送出字數 =(首字元標示長度1 + 字串長度s.length() 
	
	TX_HEADER[1] = 0XFF;  TX_HEADER[2] = 0XFF;  //改回預設廣播模式
} 

String AppsBee::getString()
{
	char * dataPtr = DataRxBuffer+3; //前3byte不管，從第4byte開始讀取 ?? byte 為第xx號指令封包格式
	
  int len = dataPtr[0];
  String s;
  s = dataPtr+1;

  value_String = s;
  return s;
}


//-------------------------------------
// 第 8 號指令封包格式   傳輸整數 2byte
void AppsBee::SendInt(int TargetID, int value)
//void AppsBee::getInt (int TargetID, int value)
{
	//(*pSer) << "PackFrame01 In " << endl;  
	//dtostrf(Temperature, 1, 2, DataTxBuffer); // Convert float to ASCII chars
	int * vptr = (int *)DataTxBuffer;
	*vptr = value;
	int vsize = sizeof(value);
	
  TX_HEADER[1] = TargetID >> 8;  
  TX_HEADER[2] = TargetID & 0x00FF;//改成傳送指定單點模式
  
  //(*pSer) << "02 " << endl;  
	AppsBeeSendCommand(_isInt, DataTxBuffer, vsize);  //8號命令號碼, 送出 2byte 數
  //(*pSer) << "03 " << endl;  
	TX_HEADER[1] = 0XFF;  TX_HEADER[2] = 0XFF;  //改回預設廣播模式
}

int AppsBee::getInt()
{
  //byte buffer[4];
	int * dataPtr = (int *)(DataRxBuffer+3); //前3byte不管，從第4byte開始讀取 2 byte 為第8號指令封包格式
  int value = *dataPtr;  
  value_int = value;  
  return value;
}

//-------------------------------------
// 第 9 號指令封包格式   傳輸浮點數  4byte
void AppsBee::SendFloat(int TargetID, float value)
{
	float * vptr = (float *)DataTxBuffer;
	*vptr = value;
	int vsize = sizeof(value);
	
  TX_HEADER[1] = TargetID >> 8;  
  TX_HEADER[2] = TargetID & 0x00FF;//改成傳送指定單點模式
  
	AppsBeeSendCommand(_isFloat, DataTxBuffer, vsize);  //9號命令號碼, 送出 4byte 數

	TX_HEADER[1] = 0XFF;  TX_HEADER[2] = 0XFF;  //改回預設廣播模式
}

float AppsBee::getFloat()
{
	float * dataPtr = (float *)(DataRxBuffer+3); //前3byte不管，從第4byte開始讀取 sizeof(float) byte 為第8號指令封包格式
  float value = *dataPtr;  
  value_float = value;  
  return value;
}
