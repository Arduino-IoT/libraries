// EmonLibCM.cpp - Library for openenergymonitor
// GNU GPL

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

// #include "WProgram.h" un-comment for use on older versions of Arduino IDE

#include "EmonLibCM.h"

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif



int cycles_per_second = 50; // mains frequency in Hz (i.e. 50 or 60)
float datalog_period_in_seconds = 1.0;
int min_startup_cycles = 10;

// Maximum number of channels used to create arrays
static const int max_no_of_channels = 5;

// User set number of channels used by for loops
int no_of_channels = 4;

// for general interaction between the main code and the ISR
volatile boolean datalogEventPending;
volatile boolean newMainsCycle = false; // provides a 50 Hz 'tick'
unsigned long lastCycle = 0;     // provides a timeout mechamism

int realPower_CT[max_no_of_channels];
double Irms_CT[max_no_of_channels];
long wh_CT[max_no_of_channels];

bool EmonLibCM_ACAC;
int EmonLibCM_Vrms;
    
// analogue ports
static const byte voltageSensor = 0;                // <-- used as an input
static const byte ADC_Sequence[] = {0,1,2,3,4,5};   // <-- used as an input

// Calibration values
//-------------------
// Two calibration values are used in this sketch: powerCal, and phaseCal. 
// With most hardware, the default values are likely to work fine without 
// need for change.  A compact explanation of each of these values now follows:

// When calculating real power, which is what this code does, the individual 
// conversion rates for voltage and current are not of importance.  It is 
// only the conversion rate for POWER which is important.  This is the 
// product of the individual conversion rates for voltage and current.  It 
// therefore has the units of ADC-steps squared per Watt.  Most systems will
// have a power conversion rate of around 20 (ADC-steps squared per Watt).
// 
// powerCal is the RECIPR0CAL of the power conversion rate.  A good value 
// to start with is therefore 1/20 = 0.05 (Watts per ADC-step squared)
//

// Voltage calibration constant:

// AC-AC Voltage adapter is designed to step down the voltage from 230V to 9V
// but the AC Voltage adapter is running open circuit and so output voltage is
// likely to be 20% higher than 9V (9 x 1.2) = 10.8V. 
// Open circuit step down = 230 / 10.8 = 21.3

// The output voltage is then steped down further with the voltage divider which has 
// values Rb = 10k, Rt = 120k (which will reduce the voltage by 13 times.

// The combined step down is therefore 21.3 x 13 = 276.9 which is the 
// theoretical calibration constant entered below.

// Current calibration constant:
// Current calibration constant = 2000 / 22 Ohms burden resistor (The CT sensor has a ratio of 2000:1)

// (2000 / 120R burden resistor

bool stop = false;
bool firstcycle = true;
    
double currentCal[max_no_of_channels];

// phaseCal is used to alter the phase of the voltage waveform relative to the
// current waveform.  The algorithm interpolates between the most recent pair
// of voltage samples according to the value of phaseCal. 
//
//    With phaseCal = 1, the most recent sample is used.  
//    With phaseCal = 0, the previous sample is used
//    With phaseCal = 0.5, the mid-point (average) value in used
//
// NB. Any tool which determines the optimal value of phaseCal must have a similar 
// scheme for taking sample values as does this sketch!
// http://openenergymonitor.org/emon/node/3792#comment-18683

double  phaseCal_CT[max_no_of_channels]; // = {0.22,0.41,0.6,1.25};

// To support the logging of Vrms, voltageCal has been included too.  When running at 240 V AC, the
// the range of ADC values will be similar to the actual range of volts, so the optimal value 
// for this cal factor will be close to unity.

// Default: 276.9 * (3.3/1023) = 0.89
double voltageCal = 0.85;

// --------------  general global variables -----------------
// Some of these variables are used in multiple blocks so cannot be static.
// For integer maths, many variables need to be 'long'
static const byte startUpPeriod = 3;    // in seconds, to allow LP filter to settle
static const int DCoffset_I = 512;      // nominal mid-point value of ADC @ x1 scale
long DCoffset_V_long;                   // <--- for LPF
long DCoffsetV_min;                     // <--- for LPF
long DCoffsetV_max;                     // <--- for LPF
long cumV_deltasThisCycle_long;         // for the LPF which determines DC offset (voltage)
static long lastSampleV_minusDC_long;          // for the phaseCal algorithm
int phaseCal_int_CT[max_no_of_channels];    // to avoid the need for floating-point maths
int datalogPeriodInMainsCycles;

// accumulators & counters for use by the ISR
long sumP_CT[max_no_of_channels];
long sumI_CT[max_no_of_channels];
long sum_Vsquared; // for Vrms datalogging   
int samplesDuringThisDatalogPeriod;   

// copies of ISR data for use by the main code
volatile long copyOf_sumP_CT[max_no_of_channels]; 
volatile long copyOf_sumI_CT[max_no_of_channels]; 
volatile long copyOf_sum_Vsquared;
volatile int copyOf_samplesDuringThisDatalogPeriod;

// For mechanisms to check the integrity of this code structure
int sampleSetsDuringThisMainsCycle;    
int lowestNoOfSampleSetsPerMainsCycle;
volatile int copyOf_lowestNoOfSampleSetsPerMainsCycle;

enum polarities {NEGATIVE, POSITIVE};
// For an enhanced polarity detection mechanism, which includes a persistence check
#define POLARITY_CHECK_MAXCOUNT 1
enum polarities polarityUnconfirmed;   
enum polarities polarityConfirmed;  // for improved zero-crossing detection
enum polarities polarityConfirmedOfLastSampleV;  // for zero-crossing detection

float powerCal_CT[max_no_of_channels];
float residualEnergy_CT[max_no_of_channels];

void EmonLibCM_number_of_channels(int _no_of_channels)
{
    no_of_channels = _no_of_channels;
}

void EmonLibCM_cycles_per_second(int _cycles_per_second)
{
    cycles_per_second = _cycles_per_second;
    datalogPeriodInMainsCycles = datalog_period_in_seconds * cycles_per_second;  
}

void EmonLibCM_min_startup_cycles(int _min_startup_cycles)
{
    min_startup_cycles = _min_startup_cycles;
}

void EmonLibCM_datalog_period(float _datalog_period_in_seconds)
{
    datalog_period_in_seconds = _datalog_period_in_seconds;
    datalogPeriodInMainsCycles = datalog_period_in_seconds * cycles_per_second;  
}

void EmonLibCM_voltageCal(double _voltageCal)
{
    voltageCal = _voltageCal;
}

void EmonLibCM_currentCal(int channel, double cal)
{
    currentCal[channel] = cal;
}

void EmonLibCM_phaseCal(int channel, double cal)
{
    phaseCal_CT[channel] = cal;
}

int EmonLibCM_getRealPower(int channel)
{
    return realPower_CT[channel];
}

double EmonLibCM_getIrms(int channel)
{
    return Irms_CT[channel];
}

int EmonLibCM_getWattHour(int channel)
{
    return wh_CT[channel];
}



void EmonLibCM_Init()
{   
    for (int i=0; i<no_of_channels; i++) 
    {
        powerCal_CT[i] = voltageCal * currentCal[i];
    
        residualEnergy_CT[i] = 0;
        
        // When using integer maths, calibration values that have supplied in floating point 
        // form need to be rescaled.
        phaseCal_int_CT[i] = phaseCal_CT[i] * 256; // for integer maths
        
        wh_CT[i] = 0;
    }
    // Define operating limits for the LP filters which identify DC offset in the voltage 
    // sample streams.  By limiting the output range, these filters always should start up 
    // correctly.
    DCoffset_V_long = 512L * 256; // nominal mid-point value of ADC @ x256 scale
    DCoffsetV_min = (long)(512L - 100) * 256; // mid-point of ADC minus a working margin
    DCoffsetV_max = (long)(512L + 100) * 256; // mid-point of ADC minus a working margin

    EmonLibCM_Start();
    
    datalogPeriodInMainsCycles = datalog_period_in_seconds * cycles_per_second;  
    datalogEventPending = false;
}

void EmonLibCM_Start()
{
    firstcycle = true;
    lastCycle = millis();
    
    // Set up the ADC to be free-running 
    // 
    // BIT:    7,    6,    5,    4,    3,    2,     1,     0
    // ADCSRA: ADEN, ADSC, ADFR, ADIF, ADIE, ADPS2, ADPS1, ADPS0
    //
    // ADEN: ADC Enable
    // ADSC: ADC Start Conversion
    // ADFR: ADC Free Running Select, or ADATE (ADC Auto Trigger Enable)
    // ADIF: ADC Interrupt Flag
    // ADIE: ADC Interrupt Enable
    // ADPS2, ADPS1, ADPS0: ADC Prescaler Select Bits (CLOCK FREQUENCY)
    //
    // The default value of ADCSRA before we change it with the following is 135
    // ADCSRA: ADEN, ADSC, ADFR, ADIF, ADIE, ADPS2, ADPS1, ADPS0
    //         128   64    32    16    8     4      2      1
    //         1     0     0     0     0     1      1      1
    // The ADC is enabled and the ADC clock is set to system clock / 128
    //
    // The following sets ADCSRA to a value of 239
    //         1     1     1     0     1     1      1      1
     
    ADCSRA  = (1<<ADPS0)+(1<<ADPS1)+(1<<ADPS2);  // Set the ADC's clock to system clock / 128
    ADCSRA |= (1 << ADEN);                       // Enable the ADC 

    ADCSRA |= (1<<ADATE);  // set the Auto Trigger Enable bit in the ADCSRA register.  Because 
                           // bits ADTS0-2 have not been set (i.e. they are all zero), the 
                           // ADC's trigger source is set to "free running mode".
                         
    ADCSRA |=(1<<ADIE);    // set the ADC interrupt enable bit. When this bit is written 
                           // to one and the I-bit in SREG is set, the 
                           // ADC Conversion Complete Interrupt is activated. 

    ADCSRA |= (1<<ADSC);   // start ADC manually first time 
    sei();                 // Enable Global Interrupts
    
    
}

void EmonLibCM_Stop()
{
    stop = true;
}

void EmonLibCM_StopInteral()
{
    // This stop function returns the ADC to default state
    ADCSRA  = (1<<ADPS0)+(1<<ADPS1)+(1<<ADPS2);  // Set the ADC's clock to system clock / 128
    ADCSRA |= (1<<ADEN);                         // Enable the ADC
    ADCSRA |= (0<<ADATE);
    ADCSRA |= (0<<ADIE);
    ADCSRA |= (0<<ADSC);
    
    stop = false;
}

void EmonLibCM_get_readings()
{
    float powerNow;
    float IrmsNow;
    float energyNow;
    int wattHoursRecent;
    
    for (int i=0; i<no_of_channels; i++) {
        powerNow = copyOf_sumP_CT[i] * powerCal_CT[i] / copyOf_samplesDuringThisDatalogPeriod;  // fp for accuracy
        realPower_CT[i] = powerNow + 0.5;                                                       // rounded down to nearest Watt
        
        IrmsNow = sqrt(copyOf_sumI_CT[i] / copyOf_samplesDuringThisDatalogPeriod) * currentCal[i];
        Irms_CT[i] = IrmsNow;
        
        energyNow = (powerNow * datalog_period_in_seconds) + residualEnergy_CT[i];              // fp for accuracy
        wattHoursRecent = energyNow / 3600;                                     // integer division to extract whole Wh
        wh_CT[i]+= wattHoursRecent;                                                             // accumulated WattHours since start-up
        residualEnergy_CT[i] = energyNow - (wattHoursRecent * 3600);            // fp for accuracy
    }

    EmonLibCM_Vrms = sqrt(copyOf_sum_Vsquared / copyOf_samplesDuringThisDatalogPeriod) * voltageCal;
}

bool EmonLibCM_Ready()
{
    if (datalogEventPending) {
        datalogEventPending = false;
        
        EmonLibCM_get_readings();
        return true;
    }
    return false;
}


void EmonLibCM_confirmPolarity()
{
    /* This routine prevents a zero-crossing point from being declared until 
    * a certain number of consecutive samples in the 'other' half of the 
    * waveform have been encountered.  It forms part of the ISR.
    */ 
    static byte count = 0;
    
    if (polarityUnconfirmed != polarityConfirmedOfLastSampleV) { 
        count++; 
    } else {
        count = 0; 
    }

    if (count >= POLARITY_CHECK_MAXCOUNT) {
        count = 0;
        polarityConfirmed = polarityUnconfirmed;
    }
}


void EmonLibCM_allGeneralProcessing_withinISR()
{
  if (stop) EmonLibCM_StopInteral();
  /* This routine dealts with activities that are only required at specific points
   * within each mains cycle.  It forms part of the ISR.
   */ 
  static int cycleCountForDatalogging = 0;
  
  if (polarityConfirmed == POSITIVE) 
  { 
      if (polarityConfirmedOfLastSampleV != POSITIVE)
      {
        /* Instantaneous power contributions are summed in accumulators during each 
         * datalogging period.  At the end of each period, copies are made of their 
         * content for use by the main code.  The accumulators, and any associated
         * counters are then reset for use during the next period.
         */       
        cycleCountForDatalogging ++;
        lastCycle = millis();
        
        // Used in stop start opperation, discards the first partial cycle
        if (cycleCountForDatalogging >= min_startup_cycles && firstcycle==true)
        {
            firstcycle = false;
            cycleCountForDatalogging = 0;
            for (int i=0; i<no_of_channels; i++) { 
              sumP_CT[i] = 0;
              sumI_CT[i] = 0;
            }
            sum_Vsquared = 0;
            lowestNoOfSampleSetsPerMainsCycle = 999;
            samplesDuringThisDatalogPeriod = 0;
        }
        
        
        if (cycleCountForDatalogging  >= datalogPeriodInMainsCycles && firstcycle==false) 
        { 
          cycleCountForDatalogging = 0;    
          for (int i=0; i<no_of_channels; i++) {
            copyOf_sumP_CT[i] = sumP_CT[i]; 
            copyOf_sumI_CT[i] = sumI_CT[i]; 
          }
          copyOf_sum_Vsquared = sum_Vsquared;
          copyOf_samplesDuringThisDatalogPeriod = samplesDuringThisDatalogPeriod;
          copyOf_lowestNoOfSampleSetsPerMainsCycle = lowestNoOfSampleSetsPerMainsCycle; // (for diags only)
          for (int i=0; i<no_of_channels; i++) {
            sumP_CT[i] = 0;
            sumI_CT[i] = 0;
          }
          sum_Vsquared = 0;
          lowestNoOfSampleSetsPerMainsCycle = 999;
          samplesDuringThisDatalogPeriod = 0;
          
          EmonLibCM_ACAC = true;
          datalogEventPending = true;
          
          // Stops the sampling at the end of the cycle if EmonLibCM_Stop() has been called
          // if (stop) EmonLibCM_StopInteral();
        }
      } // end of processing that is specific to the first Vsample in each +ve half cycle   
  } // end of processing that is specific to samples where the voltage is positive
  
  else // the polarity of this sample is negative
  {     
    if (polarityConfirmedOfLastSampleV != NEGATIVE)
    {
      // This is the start of a new -ve half cycle (just after the zero-crossing point)
      //
      // This is a convenient point to update the twin Low Pass Filters for DC-offset removal,
      // one on each voltage channel.  This needs to be done right from the start.
      long previousOffset;       
      previousOffset = DCoffset_V_long; // for voltage source V
      DCoffset_V_long = previousOffset + (cumV_deltasThisCycle_long>>6); // faster than * 0.01
      cumV_deltasThisCycle_long = 0;
      
      // To ensure that each of these LP filters will always start up correctly when 240V AC is 
      // available, its output value needs to be prevented from drifting beyond the likely range 
      // of the voltage signal.  This avoids the need to include a HPF as is often used for 
      // sketches of this type.
      //
      if (DCoffset_V_long < DCoffsetV_min) {  // for voltage source V
        DCoffset_V_long = DCoffsetV_min; }
      else  
      if (DCoffset_V_long > DCoffsetV_max) {
        DCoffset_V_long = DCoffsetV_max; }
        
      // check_RF_LED_status();       
                
    } // end of processing that is specific to the first Vsample in each -ve half cycle
  } // end of processing that is specific to samples where the voltage is positive
  
  // In the case where the voltage signal is missing this part counts the missing cycles
  // up to the duration of the datalog period at which point it will make the rms current      
  // readings available for CT only mode.
  
  unsigned long missing_cycles = (millis() - lastCycle) / (1000 / cycles_per_second);
  
  if (missing_cycles > datalogPeriodInMainsCycles) {
    lastCycle = millis(); // reset the lastCycle count here.
    firstcycle = true;    // firstcycle reset to true so that next reading
                          // with voltage signal starts from the right place
                          
          cycleCountForDatalogging = 0;    
          for (int i=0; i<no_of_channels; i++) {
            copyOf_sumP_CT[i] = 0;
            copyOf_sumI_CT[i] = sumI_CT[i]; 
          }
          copyOf_sum_Vsquared = sum_Vsquared;
          copyOf_samplesDuringThisDatalogPeriod = samplesDuringThisDatalogPeriod;
          copyOf_lowestNoOfSampleSetsPerMainsCycle = lowestNoOfSampleSetsPerMainsCycle; // (for diags only)
          for (int i=0; i<no_of_channels; i++) {
            sumP_CT[i] = 0;
            sumI_CT[i] = 0;
          }
          sum_Vsquared = 0;
          lowestNoOfSampleSetsPerMainsCycle = 999;
          samplesDuringThisDatalogPeriod = 0;
          
          EmonLibCM_ACAC = false;
          datalogEventPending = true;
          
          // Stops the sampling at the end of the cycle if EmonLibCM_Stop() has been called
          // if (stop) EmonLibCM_StopInteral();                
  }
}
// end of allGeneralProcessing()

// This Interrupt Service Routine is for use when the ADC is in the free-running mode.
// It is executed whenever an ADC conversion has finished, approx every 104 us.  In 
// free-running mode, the ADC has already started its next conversion by the time that
// the ISR is executed.  The ISR therefore needs to "look ahead". 
//   At the end of conversion Type N, conversion Type N+1 will start automatically.  The ISR 
// which runs at this point therefore needs to capture the results of conversion Type N , 
// and set up the conditions for conversion Type N+2, and so on.  
//   Activities that are required for every new sample are performed here.  Activities
// that are only required at certain stages of the voltage waveform are performed within
// the helper function, allGeneralProcessing_withinISR().
//   A second helper function, confirmPolarity() is used to apply a persistence criterion
// when the polarity status of each voltage sample is checked. 
// 
void EmonLibCM_interrupt()  
{                                         
  static unsigned char sample_index = 0;
  unsigned char next = 0;
  static  long sampleV_minusDC_long;
  int rawSample;
  long filtV_div4;
  long filtI_div4;
  long instP;
  long inst_Vsquared;
  long inst_Isquared;
  long sampleIminusDC_long;
  long phaseShiftedSampleV_minusDC_long;
  
  rawSample = ADC;
  
  next = sample_index + 2;
  if (next>=no_of_channels+1) next -= no_of_channels+1;
  ADMUX = 0x40 + ADC_Sequence[next]; // set up the next-but-one conversion
  
  if (sample_index==0)
  {
      // remove DC offset from the raw voltage sample by subtracting the accurate value 
      // as determined by a LP filter.
      sampleV_minusDC_long = ((long)rawSample<<8) - DCoffset_V_long;
      //
      // deal with activities that are only needed at certain stages of each  
      // voltage cycle.
      if(sampleV_minusDC_long > 0) { 
        polarityUnconfirmed = POSITIVE; }
      else { 
        polarityUnconfirmed = NEGATIVE; }
      EmonLibCM_confirmPolarity();
      EmonLibCM_allGeneralProcessing_withinISR();
      //
      // for real power calculations
      sampleSetsDuringThisMainsCycle++; 
      samplesDuringThisDatalogPeriod++;      
      //
      // for the Vrms calculation 
      filtV_div4 = sampleV_minusDC_long>>2;  // reduce to 16-bits (now x64, or 2^6)
      inst_Vsquared = filtV_div4 * filtV_div4; // 32-bits (now x4096, or 2^12)
      inst_Vsquared = inst_Vsquared>>12;     // scaling is now x1 (V_ADC x I_ADC)
      sum_Vsquared += inst_Vsquared; // cumulative V^2 (V_ADC x I_ADC)
      //
      // store items for later use
      cumV_deltasThisCycle_long += sampleV_minusDC_long; // for use with LP filter
      lastSampleV_minusDC_long = sampleV_minusDC_long;  // required for phaseCal algorithm  
      polarityConfirmedOfLastSampleV = polarityConfirmed;  // for identification of half cycle boundaries
  }
  
  if (sample_index>=1 && sample_index <= no_of_channels)
  {
      if (rawSample>10) {
      // remove most of the DC offset from the current sample (the precise value does not matter)
      sampleIminusDC_long = ((long)(rawSample - DCoffset_I))<<8;
      
      // phase-shift the voltage waveform so that it aligns with the current 
      //phaseShiftedSampleV_minusDC_long = sampleV_minusDC_long;
      phaseShiftedSampleV_minusDC_long = lastSampleV_minusDC_long
         + (((sampleV_minusDC_long - lastSampleV_minusDC_long)*phaseCal_int_CT[sample_index-1])>>8);  
      
      // calculate the "real power" in this sample pair and add to the accumulated sum
      filtV_div4 = phaseShiftedSampleV_minusDC_long>>2;  // reduce to 16-bits (now x64, or 2^6)
      filtI_div4 = sampleIminusDC_long>>2; // reduce to 16-bits (now x64, or 2^6)
      instP = filtV_div4 * filtI_div4;  // 32-bits (now x4096, or 2^12)
      instP = instP>>12;     // scaling is now x1, as for Mk2 (V_ADC x I_ADC)  
      
      inst_Isquared = filtI_div4 * filtI_div4;
      inst_Isquared = inst_Isquared>>12;
      sumI_CT[sample_index-1] += inst_Isquared;
      
      // if (sample_CT4==0) instP_CT4 = 0;  
      sumP_CT[sample_index-1] +=instP; // cumulative power, scaling as for Mk2 (V_ADC x I_ADC)
      }
  }
  
  sample_index++; // advance the control flag
  if (sample_index>no_of_channels) sample_index = 0;
}

ISR(ADC_vect) {
    EmonLibCM_interrupt();
}
