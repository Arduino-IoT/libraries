#include "pieceduino.h"
#define SSID        "Tronk-Interactive"
#define PASSWORD    "tronk54304753"
pieceduino wifi(Serial1);
uint32_t len;

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
    wifi.connToWifi(SSID,PASSWORD);
  }
  Serial.print(wifi.getIP());//取得IP
  wifi.disableMUX();//開啟多人連線模式
   wifi.createUDP("192.168.0.101",7400);//開啟TCP Server
  delay(100);
  wifi.Send("Hello This Client" , 17);//送出訊息到Server端
}

void loop(){

  //取得socket client訊息並回傳
  //------------------------------------------
  len = wifi.recv();
  if(len){
    char recvData[len-1];
    
    for (uint32_t i = 0; i < len; i++) {
      //Serial.print("Recive ");
      //Serial.print(i);
      //Serial.print(" Byte: ");
      Serial.print(wifi.MessageBuffer[i]); 
      //Serial.print("  From: ");
      //Serial.println(wifi.client_id);
      recvData[i] = wifi.MessageBuffer[i];
    }
    Serial.println("");
    //delay(100);
    wifi.Send("Hello This Client" , 17);//送出訊息到Server端
  }else{
    //wifi.Send("Hello This Client" , 17);//送出訊息到Server端
  }
  //------------------------------------------
  if(!wifi.connected()){
    Serial.println("disconnected");
    if(wifi.createUDP("192.168.0.101",7400)){//開啟TCP Server
      wifi.Send("Hello This Client" , 17);
    }
  }
}

