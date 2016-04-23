/* Touch Panel Example */
/* The code is OK.*/
//20140208 art100  tft2.4" st7783 shield on arduino uno
// art100 write message 
//#include "st7783SD.h" // pins SD_SS-10 SD_DI-11(in) SD_DO-12(out) SD_SCK-13 no work
//#include "st7783TFT.h" //has not written
//#include "st7783TouchScreen.h" //has not written
#include "TFTLCD.h" // work
#include "TouchScreen.h" // bad but it works
//TouchScreen define
#define YP A1 //tftloop-YU pcbundertft-13 arduinoshieldLCD_WR!!! arduinouno-A1
#define XM A2 //tftloop-XR pcbundertft-14 arduinoshieldLCD_RS!!! arduinouno-A2
#define YM 7  //tftloop-XR pcbundertft-15 arduinoshieldLCD_D7 arduinouno-7
#define XP 6  //tftloop-XR pcbundertft-12 arduinoshieldLCD_D6 arduinouno-6
#define TS_MINX 150//0
#define TS_MINY 120//0
#define TS_MAXX 920//900//320      //for calibrate analog resistans ~1500om/700om
#define TS_MAXY 940//800//900//240 //for calibrate analog resistans ~1500om/700om
// For better pressure precision, we need to know the resistance// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
//TFT  define
#define LCD_RESET A4// 
#define LCD_CS A3
#define LCD_CD A2 //cmosbuffer-arduinoshieldLCD_RS!!! arduinouno-A2 this is jock :-?
#define LCD_WR A1 //cmosbuffer-arduinoshieldLCD_WR!!! arduinouno-A1 this is jock :-?
#define LCD_RD A0
// Color definitions
#define WHITE           0xFFFF
#define	BLACK           0x0000
#define	RED             0xF800
#define	BLUE            0x001F
#define	GREEN           0x07E0
#define YELLOW          0xFFE0 
TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
//SD define //has not written
#define bordur   5
#define pen      3
//---------------------------------------
void setup(void) {
  Serial.begin(9600);
  Serial.println("Paint");
  tft.reset();
  uint16_t identifier = tft.readRegister(0x0);
  tft.initDisplay(); 
  tft.fillScreen(BLACK);
}
#define MINPRESSURE 10    // analog calibrate
#define MAXPRESSURE 1000  // analog calibrate
//=======================================
void loop(){
  Point p = ts.getPoint(); // get analog point
  pinMode(XM, OUTPUT); //2 chip on one pins
  pinMode(YP, OUTPUT); //2 chip on one pins
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) { // analog calibrate
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width ()); // aprocsimate variable
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height()); // aprocsimate variable
    
//    if (300< p.x) { // erase only on bordur  //no need have reset button
//      Serial.println("erase"); // test
//      tft.fillRect(0, 0, tft.width(), tft.height(), RED);// BLACK); //fill black
//    }
    
    // test
    Serial.print("getPoint x=");Serial.print(p.x);Serial.print(" y=");Serial.print(p.y);Serial.print(" PRESSURE=");Serial.print(p.z);  
    for(int i=0;i<10;i++) 
     tft.drawFastLine(240-p.x, i+320-p.y, 10, WHITE,0);
     //Modify by TYWU
    // paint
   // if (((p.x)<tft.width()) && ((p.y)<tft.height())) { tft.fillRect(p.x,p.y,pen,pen,WHITE); } 
  }
}

