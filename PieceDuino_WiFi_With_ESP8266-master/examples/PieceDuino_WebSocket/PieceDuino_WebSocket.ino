#include "pieceduino.h"
#define SSID        "Your AP SSID"
#define PASSWORD    "Your AP Password"
#define pieceduino_cloud_token    "Your Token" //To www.pieceduino.com pieceCloud get your own token
pieceduino wifi(Serial1);
uint32_t len;
unsigned long timer;

void setup(){
  Serial.begin(9600);
  Serial1.begin(115200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  wifi.begin();//初始化
  wifi.reset();//重啟WiFi
  wifi.setWifiMode(1);//將WiFi模組設定為Station模式
  while(!wifi.connToWifi(SSID,PASSWORD)){//連接網路
    Serial.println("try to reconnect");
  }
  Serial.println(wifi.getIP());//取得IP
  wifi.disableMUX();//關閉多人連線模式
  wifi.WebSocketConnect(pieceduino_cloud_token);
  wifi.setCallback(Catch);

  pinMode(13,OUTPUT);
  timer = millis();
}

void loop(){
  wifi.recv();

  if (millis() - timer > 3000){
      wifi.Throw("A",millis()/1000.0);
      //wifi.SaveDataToCloud("C","20.3");
      timer = millis();
  }
}

void Catch(char key, float value) {
  Serial.print(key);
  Serial.print(" : ");
  Serial.println(value);
  if (key == 'A') {
    if (value == 0) {
      digitalWrite(13, LOW);
    }
    else if (value == 1) {
      digitalWrite(13, HIGH);
    }
  }
}

