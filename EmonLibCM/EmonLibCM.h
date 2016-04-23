/*
  EmonLibCM.h - Library for openenergymonitor
  GNU GPL
*/

// This sketch provides continuous single-phase monitoring of real power on four CT channels.
// All of the time-critical code is now contained within the ISR, only the slower activities
// are done within the main code.  These slower activities includes temperature sensing, 
// RF transmissions, and all Serial statements.  
//
// This sketch is now suitable for either 50 or 60 Hz operation.  To avoid any disturbance
// to the continuous sampling process, the RFM12B module remains permanently active rather 
// than being put to sleep between transmissions. 
//
// Original Author: Robin Emley (calypso_rae on Open Energy Monitor Forum)
// Addition of Wh totals by: Trystan Lea
// Upgrade of ISR structure to support temperature sensing: Robin Emley Nov '14
// Temperature sensing logic is from PLL Router code supplied by Martin Roberts, Dec '12.

#ifndef EmonLibCM_h
#define EmonLibCM_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

void EmonLibCM_number_of_channels(int _no_of_channels);
void EmonLibCM_cycles_per_second(int _cycles_per_second);
void EmonLibCM_min_startup_cycles(int _min_startup_cycles);
void EmonLibCM_datalog_period(float _datalog_period_in_seconds);

void EmonLibCM_voltageCal(double _voltageCal);
void EmonLibCM_currentCal(int channel, double cal);
void EmonLibCM_phaseCal(int channel, double cal);

int EmonLibCM_getRealPower(int channel);
double EmonLibCM_getIrms(int channel);
int EmonLibCM_getWattHour(int channel);

void EmonLibCM_Init();

void EmonLibCM_Start();
void EmonLibCM_Stop();

bool EmonLibCM_Ready();

// for general interaction between the main code and the ISR
extern volatile boolean datalogEventPending;

extern bool EmonLibCM_ACAC;

extern int EmonLibCM_Vrms;

#endif
