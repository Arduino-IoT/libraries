#include "pieceduino.h"
pieceduino wifi(Serial1);
uint32_t len;
void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  wifi.begin();//初始化
  wifi.reset();//重啟WiFi
  wifi.setWifiMode(2);//將WiFi模組設定為Station模式
  if (wifi.setAP("PieceDuino-AP","",1,0)){
    Serial.println("Create AP Success");
    Serial.print("IP: ");       
    Serial.println(wifi.getIP());
  }else{
    Serial.println("Create AP Failure");
  }

  wifi.enableMUX();//開啟多人連線模式
  wifi.createTCPServer(8090);//開啟TCP Server
}

void loop() {
   //取得socket client訊息並回傳
  //------------------------------------------
  len = wifi.recv();
  if(len){
    char recvData[len-1];
    for (uint32_t i = 0; i < len; i++) {
      Serial.print("Recive ");
      Serial.print(i);
      Serial.print(" Byte: ");
      Serial.print(wifi.MessageBuffer[i]); 
      Serial.print("  From: ");
      Serial.println(wifi.client_id);
      recvData[i] = wifi.MessageBuffer[i];
    }
    delay(1000);
    wifi.Send(wifi.client_id , "Hello This is Server" , 20);
  }
  //------------------------------------------

}
