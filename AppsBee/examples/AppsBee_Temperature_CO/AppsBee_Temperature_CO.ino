//--------------------------------------------------
// 　每秒讀一次溫度(DS18B20)並廣播給所有的Nodes(ROs)
//   將J19的DQ Pin接至D7,Vdd 接至 5V 
//--------------------------------------------------
#include <Streaming.h>
#include <SoftwareSerial.h>
#include <AppsBee.h>

//--宣告 AppsBee 物件
//RX, TX  Software Serial support 9600bps ~ 57600bps,can't support 115200bps over
//AppsBee AZbee(5, 4, RO);   //AppsBee(TX, RX, role) 
AppsBee AZbee(5, 4, CO);    //AppsBee(TX, RX, role)
//AppsBee AZbee(Serial, RO);  //RO使用硬體serial,AppsBee(Serial,Role)
//AppsBee AZbee(Serial, CO);  //CO使用硬體serial,AppsBee(Serial,Role)

/*-----( Import needed libraries )-----*/ 
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library
//-----( Declare Constants )-----
#define I2C_ADDR    0x27  // Define I2C Address for the PCF8574T 
//---(Following are the PCF8574 pin assignments to LCD connections )----
#define BACKLIGHT_PIN  3
#define  LED_OFF  1
#define  LED_ON  0
/*-----( Declare objects )-----*/  
LiquidCrystal_I2C  lcd(I2C_ADDR,2,1,0,4,5,6,7); // declare I2C LCD object
//------------------------------
#include <Timer.h>
Timer tick ;// setup one sec timer event
//-------- DS18B20 Temperature sensor -------------
#define   DS18B20_Pin   7  //Define DS18S20 onewire signal pin on D7
#include <OneWire.h>
#include <DS18B20.h>
DS18B20 dd(DS18B20_Pin);  // on digital pin 7

//---------------------------------------------------------

void setup()
{
   AZbee.begin(9600); // initialize AppsBee with baud rate = 9600 bps
   lcd.begin(16, 2);    //Initialize LCD as 16 x 2
   // Switch on the backlight
   lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
   lcd.setBacklight(LED_ON);
   lcd.backlight();  //Backlight ON if under program control
   lcd.print("ID #0 : ") ;
   lcd.setCursor(0,1);     // Display from 2nd Row
   lcd << "ID #" << AZbee.UniMyID << " : " ; // Display current ID
 //---------- Set up Timer -------  
   tick.every(1000, OneSecTimerHandler);// read temperature/humidity once per second 
}

void loop()
{  
  tick.update(); // check timer event
  CheckAppsBeeReceiveData();   //檢查Zigbee是否接收到資料
}

//-----------------------------------------------------
// 檢查Zigbee是否接收到資料
void CheckAppsBeeReceiveData()
{
  if (AZbee.receive()) {   //kuo 0504
    // 如果 Zigbee 接收到資料
      lcd.setCursor(4, AZbee.RxSourceID % 2) ; // display message based on source ID
      lcd << AZbee.RxSourceID << " : " ; // display source ID  //kuo 0504
      switch(AZbee.RxCmd) { //判斷是收到哪一個類型的資料
        case _isString :   // 接收到字串資料 String
          lcd << AZbee.value_String  ; 
          break;
        case _isFloat :    // 接收到浮點數資料 float  //kuo 0504
          lcd <<  _FLOAT(AZbee.value_float, 1) << (char)(0xDF) << "C " ;// LCD degree "o" Char
          break;
        case _isInt :      // 接收到整數資料 float  //kuo 0504
          lcd << AZbee.value_int ; 
          break;
        default:
          break;
      }
  } 
}  

/*
 Data Format : Temperature
 Timer Interval : 1 sec 
*/
void OneSecTimerHandler() //-- update per sec -- 
{
    float Temp ;
    
    Temp = dd.getTemperature();    // read the temperature from DS18B20
    lcd.setCursor(0,AZbee.UniMyID % 2);     // Display from 2nd Row
    lcd << "ID #" << AZbee.UniMyID << " : " << _FLOAT(Temp,1) << (char) (0xDF) << "C " ;
    AZbee.SendFloat(BROADCAST,Temp) ; // Tx Temperature to CO
 }


