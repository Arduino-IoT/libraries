//--------- Read temperature from DS18B20 witn one Sec interval --------
//--------- Declare one DS18B20 Temperature object  -------------
#define   DS18B20_Pin   7  //Define DS18S20 onewire signal pin on D7
#include <OneWire.h>
#include <DS18B20.h>
DS18B20 dd(DS18B20_Pin);  // on digital pin 7

//---------------------------------------------------------

void setup()
{
   Serial.begin(9600) ;
}

void loop()
{  
  Serial.println(dd.getTemperature(),2) ; // show the temperature with xx.xx 
  delay(1000) ;
}

