#ifndef DS18B20_included  //==================================================
#define DS18B20_included

#include "OneWire.h"

//---------------------------------
class DS18B20 : public OneWire
{
	public:
		DS18B20 ( uint8_t pin) : OneWire(pin)
    { ; } 
    float getTemperature();
};

#endif  //==================================================
