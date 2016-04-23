/*
  AppsBee.h - ZigBee Library for AppsBee Shield
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
/*
 *   AppsBee.h
 */
#include "Arduino.h"

#ifndef AppsBee_included  //==================================================
#define AppsBee_included

//#define KDEBUG
//#ifdef KDEBUG
//
//#endif


//------------ Add AppsBee Comm on 7/2 ------------------------------
#include "SoftwareSerial.h"  //for Mega and Mega 2560 support, only the following can be used for RX: 10, 11, 12, 13, 14, 15....

//--------- AppsBee Shield I/O definition
//SoftwareSerial AppsBeeSerial(5, 4); // RX, TX  Software Serial support 9600bps ~ 38400bps,can't support 115,200bps
#define M01      6     // Mode select : Broadcast(0)/UNICast(1)
#define RESET    9     // AppsBee Reset Pin 
#define CO       0
#define RO       1

#define SYNC          0
#define PREAMBLE      1
#define DATALENRCVED  2
#define TX_BUF_SIZE      64
#define RX_BUF_SIZE      64
#define SETTING_BUF_SIZE 40
#define BROADCAST     0xFFFF			// Broadcast data to all Nodes(CO & ROs)
#define SENDtoCO		  0x0000			// Send data to CO

//---------------------------------
enum fmType {
  _isGCommand=1,  
  _isString,  
  _isInt,  
  _isFloat 
};
//--------- Setting Mode definition ------------
enum settingType {
	PANID = 0,  // start from index 0
	BAUD_RATE,
	TX_POWER,
	CHANNEL,
	ANNCE_RATIO,
	UNI_SEC_ADDR,
	PW_RESET
} ;	
	 

//---------------------------------
class TCommand 
{
	public: 
		int TargetID;  //0429
		int SourceID;  //0429
		byte CommandID;
		byte Parameter;
		
		TCommand (String initStr="");
		TCommand (byte _TargetID, byte _SourceID,	byte _CommandID, byte _Parameter) :
			TargetID   (_TargetID ), SourceID   (_SourceID ), CommandID  (_CommandID), Parameter  (_Parameter)
			{ };
};

//---------------------------------
//class AppsBee  : public SoftwareSerial
class AppsBee 
{
	private:
  	int _state;   //接收資料時，紀錄狀態
    int _DataOffset;  // Received data buffer offset 
		int _DataLength;				//=0
    byte _checksum;   // Received data buffer offset 
    
    int _role;
		long _speed;  //傳輸速度

		SoftwareSerial *swSerial;
		HardwareSerial *hwSerial;
		Stream *ts;
		
		static const char *ModeCommand[] ;    // Setting Mode Command String array
		boolean BaudRate_Check(long) ;				// Checking input baud rate is ok or not ? 
		
	public:
		byte TX_HEADER[4]; // CO身分專用 {0xAA,0xFF,0xFF,0x55} ; // CO broadcast data to all ROs
		                   // RO身分專用 {0xAA,0x00,0x00,0x55} ; // desitation : RO transmit data to CO(address : 00) 


		AppsBee(uint8_t RX=5, uint8_t TX=4, uint8_t Role=RO);
		AppsBee(HardwareSerial &h, uint8_t Role=RO); // Hardware Serial 

	  int UniMyID;  // 0429
    HardwareSerial *pSer; //除錯用 serial
		char DataRxBuffer[RX_BUF_SIZE];
		char DataTxBuffer[TX_BUF_SIZE];
		char ResponseBuffer[SETTING_BUF_SIZE];
				
		byte RxCmd;   	//*cmd = (byte)DataRxBuffer[0] ;
		int RxDatalen;  //*datalen = _DataLength - 3 ;
		int RxSourceID; //*sourceid = DataRxBuffer[1] * 256 + (byte) DataRxBuffer[2] ;
		char *RxData;		//data = &DataRxBuffer[3] ;

		int value_int;
		float value_float;
		String value_String;
		
		void begin(long speed=9600);
  	int available();
  	int read();
  	size_t write(uint8_t byte);
  	size_t write(const uint8_t *buffer, size_t size);
  	
		size_t print(const String &);
		
		//0528-----------------------------
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
		//0528-----------------------------

		int GetRole() { return _role; };
    int Get_DataLength() { return _DataLength; };
    
		void AppsBeeSendCommand(byte CMD, char *Data,int DataLength);
		boolean AppsBeeEncode(char c);
		byte Get_DataFrame(); //傳回所使用指令編號 cmd, 另外也呼叫 UnPackFrame ，解出各欄位資料
		int AppsBeeUniIDread();
		int AppsBeeUniIDWrite(int);
		char *SettingMode(settingType mode,long SettingData) ;
		
		
		// 通用數位控制指令
  	void SendGCommand(int TargetID, TCommand cmd);
  	void SendGCommand(int _TargetID, int _SourceID, byte _CommandID, byte _Parameter);
		TCommand getGCommand();

		//void PackFrame07 (int TargetID, String value);  //傳輸字串
  	void SendString(int TargetID, String value);
		String getString();
		
    //void PackFrame08 (int TargetID, int value);  //傳輸整數
  	void SendInt(int TargetID, int value);
    int getInt();

    //void PackFrame09 (int TargetID, float value);  //傳輸浮點數
  	void SendFloat(int TargetID, float value);
    float getFloat();
    
		void SetDebugSerial(HardwareSerial * ptr) { pSer = ptr; }

  	boolean AppsBeeEncodeFrame(void);
  	boolean receive();
};

#endif  //==================================================
