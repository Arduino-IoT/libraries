#include <Arduino.h>
#include "EmonLibCM.h"

#include <JeeLib.h> 
ISR(WDT_vect) { Sleepy::watchdogEvent(); } 

void setup()
{  
  Serial.begin(9600);
  Serial.println("EmonTx v3.4 EmonLibCM Continuous Monitoring Tester");
  
  EmonLibCM_number_of_channels(4);       // number of current channels
  EmonLibCM_cycles_per_second(50);       // frequency 50Hz, 60Hz
  EmonLibCM_datalog_period(10);          // period of readings in seconds
  
  EmonLibCM_min_startup_cycles(10);      // number of cycles to let ADC run before starting first actual measurement
                                         // larger value improves stability if operated in stop->sleep->start mode

  EmonLibCM_voltageCal(0.84);            // 260.4 * (3.3/1023)
  
  EmonLibCM_currentCal(0,90.9*(3.3/1023));  // 2000 turns / 22 Ohms burden resistor
  EmonLibCM_currentCal(1,90.9*(3.3/1023));  // 2000 turns / 22 Ohms burden resistor
  EmonLibCM_currentCal(2,90.9*(3.3/1023));  // 2000 turns / 22 Ohms burden resistor
  EmonLibCM_currentCal(3,16.6*(3.3/1023));  // 2000 turns / 120 Ohms burden resistor

  EmonLibCM_phaseCal(0,0.22);
  EmonLibCM_phaseCal(1,0.41);
  EmonLibCM_phaseCal(2,0.60);
  EmonLibCM_phaseCal(3,1.25);
  
  EmonLibCM_Init();
}

void loop()             
{
  if (EmonLibCM_Ready())   
  {
    Serial.print(EmonLibCM_getRealPower(0)); Serial.print(':'); Serial.print(EmonLibCM_getWattHour(0)); Serial.print(',');
    Serial.print(EmonLibCM_getRealPower(1)); Serial.print(':'); Serial.print(EmonLibCM_getWattHour(1)); Serial.print(',');
    Serial.print(EmonLibCM_getRealPower(2)); Serial.print(':'); Serial.print(EmonLibCM_getWattHour(2)); Serial.print(',');
    Serial.print(EmonLibCM_getRealPower(3)); Serial.print(':'); Serial.print(EmonLibCM_getWattHour(3)); Serial.print(',');
    Serial.print(VrmsTimes100);
    Serial.println(); 
    
    // UNCOMMENT TO TEST STOP/START SLEEP OPERATION
    // EmonLibCM_Stop();
    // delay(50);
    // Sleepy::loseSomeTime(10000);
    // EmonLibCM_Start();
  }
}
