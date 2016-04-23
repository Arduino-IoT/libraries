#include <Streaming.h>
#include "SoftwareSerial.h" 
#include <AppsBee.h>

//宣告Zigbee物件
//AppsBee AZbee(5, 4, RO);   //  AppsBee(TX, RX, role) 
AppsBee AZbee(5, 4, CO);     //  AppsBee(TX, RX, role)

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

//---------------------------------------------------
long startTime;
const long duration = 1000;  //控制每秒檢查一次事件

void setup() 
{
  //-----------------------
  Serial.begin(115200);
  lcd.begin (16,2);  // initialize the lcd 
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
  lcd.backlight();  //Backlight ON if under program control
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print(F("Connecting...")) ;
  
  //-------------------------------------
  // set the data rate for the SoftwareSerial port
  AZbee.begin(9600);
  Serial << "ID:" << AZbee.UniMyID << endl;

  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd << F("ID#") << AZbee.UniMyID << "          ";

  //-------------------------------------
  startTime  = millis();
}

//-----------------------------------------------------
void loop()
{
  // 　每隔固定時間 duration, 執行一次
  if (millis()-startTime >= duration) {
    // call event
    OneSecTimerHandler(); 
    startTime  = millis();
  }
  CheckZigBeeReceiveData();   //檢查Zigbee是否接收到資料
}

//-----------------------------------------------------
int tt = 160;
void OneSecTimerHandler() //-- update per sec -- 
{
  tt+=1;   if (tt>360) tt=160;
 
  //--------------------------------
  lcd.setCursor(15,0); //Start at character 0 on line 0
  lcd.print(tt % 10) ;

  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd << "ID#" << AZbee.UniMyID << " ";   //顯示Zigbee自己的ID#
  
  //-------------------
  lcd.setCursor(8,0);     
  lcd << _FLOAT(tt/10.0, 1) << ' ';   //顯示自己的計數值
 
  //-------------------
    // 每隔數秒，傳送 CO 自己的計數值給所有的 RO
    switch(tt % 3) {
      case 0 : 
          AZbee.SendInt (BROADCAST, tt);  //08. 傳整數 Integer
          break;
      case 1 : 
          AZbee.SendFloat (BROADCAST, (float)tt/10.0);  //09.　傳浮點數 float
          break;
      case 2 : 
          {
            String s = "Str";
            s = s + tt;
            AZbee.SendString (BROADCAST, s);  //07. 傳字串 String
            break;
          }
      default:
         AZbee.SendFloat (BROADCAST, (float)tt/10.0);  //　預設 傳浮點數 float
  }
}

//-----------------------------------------------------
// 檢查Zigbee是否接收到資料
void CheckZigBeeReceiveData()
{
  if (AZbee.receive()) {
    // 如果 Zigbee 接收到資料
      switch(AZbee.RxCmd) { //判斷是收到哪一個類型的資料
        case _isString :   // 接收到字串資料 String
          lcd.setCursor(0, 1);  //顯示到LCD
          lcd << "#" << AZbee.RxSourceID << ":(str)" << AZbee.value_String << "   "; 
          break;
        case _isFloat :    // 接收到浮點數資料 float
          lcd.setCursor(0, 1);  //顯示到LCD
          lcd << "#" << AZbee.RxSourceID << ":(float)" << _FLOAT(AZbee.value_float, 1) << "   "; 
          break;
        case _isInt :      // 接收到整數資料 float
          lcd.setCursor(0, 1);  //顯示到LCD
          lcd << "#" << AZbee.RxSourceID << ":(int)" << AZbee.value_int << "   "; 
          break;
        default:
          break;
      }
  } 
}  



