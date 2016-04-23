/*
  RX_ADR_WIDTH default is 5, modify the SETUP_AW setting in library file for other value
  RX_ADR_WIDTH should be equal to TX_PLOAD_WIDTH
*/
#include <ELECHOUSE_NRF24L01.h>

#define RX_ADR_WIDTH   5  // RX address width 
#define RX_PLOAD_WIDTH  32  // RX payload width

byte RX_ADDRESS_P0[RX_ADR_WIDTH] = {0x00,0x10,0x10,0x10,0x10}; //pipe 0 address
byte RX_ADDRESS_P1[RX_ADR_WIDTH] = {0x01,0x10,0x10,0x10,0x10};  //pipe 1 address
byte RX_ADDRESS_P2[1] = {0x02};            // pipe 2 address, MSBytes are equal to pipe 1
byte RX_buffer[RX_PLOAD_WIDTH]={0};        //RX buffer
byte i,temp;

void setup()
{
  Serial.begin(9600);
  ELECHOUSE_nRF24L01.Init();        //initialization
  ELECHOUSE_nRF24L01.RegConfigSettings();  //RF,address width,enable AA
  ELECHOUSE_nRF24L01.RX_Setting(0,RX_ADDRESS_P0,RX_ADR_WIDTH,RX_PLOAD_WIDTH); // pipe 0 address
  ELECHOUSE_nRF24L01.RX_Setting(1,RX_ADDRESS_P1,RX_ADR_WIDTH,RX_PLOAD_WIDTH); // pipe 1 address
  ELECHOUSE_nRF24L01.RX_Setting(2,RX_ADDRESS_P2,1,RX_PLOAD_WIDTH); //pipe 2 address
  ELECHOUSE_nRF24L01.RX_ModeStart(); //start receive
}

void loop()
{
  temp=ELECHOUSE_nRF24L01.CheckReceiveFlag(); //check status
  if(temp)                      // have data
  {
    temp=ELECHOUSE_nRF24L01.ReceiveData(RX_buffer,RX_PLOAD_WIDTH);//pipe number and RX data
    Serial.print(temp,HEX);
    Serial.print(' ',BYTE);
    for(i=0;i<RX_PLOAD_WIDTH;i++)
    {
      Serial.print(RX_buffer[i],DEC);
      Serial.print(' ',BYTE);
    }
    Serial.println("");
    ELECHOUSE_nRF24L01.RX_ModeStart();
  }
  delay(1);
}
