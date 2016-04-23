/*
  TX_ADR_WIDTH default is 5, modify the SETUP_AW setting in library file for other value
  TX_PLOAD_WIDTH should be equal to RX_PLOAD_WIDTH
*/
#include <ELECHOUSE_NRF24L01.h>

#define TX_ADR_WIDTH   5    // TX address width 
#define TX_PLOAD_WIDTH 32   // TX payload width

byte TX_ADDRESS[TX_ADR_WIDTH] = {0x00,0x10,0x10,0x10,0x10};  // TX address
byte TX_buffer[TX_PLOAD_WIDTH]={0};     // data buffer to transmit
byte i,temp;

void setup()
{
  Serial.begin(9600);
  ELECHOUSE_nRF24L01.Init();      //initialization
  ELECHOUSE_nRF24L01.RegConfigSettings();  //RF,address width,enable AA
  for(i=0;i<TX_PLOAD_WIDTH;i++)
  {
     TX_buffer[i]=i;
  }
}

void loop()
{
  ELECHOUSE_nRF24L01.TX_Setting(TX_ADDRESS,TX_ADR_WIDTH,TX_buffer,TX_PLOAD_WIDTH);//TX setting
  ELECHOUSE_nRF24L01.TX_ModeStart();  //start TX
  temp=ELECHOUSE_nRF24L01.CheckSendFlag();  //TX flag
  Serial.print(temp,HEX);
  Serial.println("");
  delay(1000);
}
