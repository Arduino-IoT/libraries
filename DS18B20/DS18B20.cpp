//#include "OneWire.h"
#include "DS18B20.h"

float DS18B20::getTemperature()
{
	byte data[12];
  byte addr[8];

  if ( !search(addr)) {
      //no more sensors on chain, reset search
      reset_search();
      return -99;
  }

  if ( crc8( addr, 7) != addr[7]) {
      return -99;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      return -99;
  }

  reset();
  select(addr);
  write(0x44,1); // start conversion, with parasite power on at the end

  byte present = reset();
  select(addr);    
  write(0xBE); // Read Scratchpad
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = read();
  }
  reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
}

