//---------------- RO ID Setting ----------------
//  use AppsBee SettingMode(Type,#) method to set ID/Channel/Network ID/...
//-----------------------------------------------
#include <Streaming.h>
#include <SoftwareSerial.h>
#include <AppsBee.h>

//--宣告Zigbee物件
//RX, TX  Software Serial support 9600bps ~ 19200bps,can't support 38400 ~ 115200
AppsBee AZbee(5, 4, RO);   //  AppsBee(TX, RX, role) 
//AppsBee AZbee(5, 4, CO);     //  AppsBee(TX, RX, role)
//AppsBee AZbee(Serial, RO);    //RO使用硬體serial
//AppsBee AZbee(Serial, CO);    //CO使用硬體serial

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

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  int  msgLen ;
  char *msg ;
  
  AZbee.begin(9600); // initialize AppsBee with baud rate = 9600 bps
  lcd.begin (16,2);  // initialize the lcd 
// Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
  lcd.backlight();  //Backlight ON if under program control
  lcd.setCursor(0,0); //Start at character 0 on line 0
  msg = AZbee.SettingMode(BAUD_RATE,0) ;// setting Address/設定位址(Command Format : UNI_SEC_ADDR #)
 //---- Check setting is OK or not       // # : RO可設定有效位址 : 1,2, ~~ 2047(11 bits)
  msgLen = strlen(msg) ; // Length of response message
  for(int i=0; i < msgLen;i++) {
    lcd.setCursor(i % 16, (i/16) % 2);   // setup cursor from (0,0)
    lcd.print(msg + i) ;
  }  
}// END Setup

void loop()   
{

} // END Loop
