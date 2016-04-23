#ifndef __ACS712_H__
#define __ACS712_H__

// debug console
#define DEBUG

// sample over 100ms, it is an exact number of cycles for both 50Hz and 60Hz mains
const unsigned long sampleTime = 100000UL;
// choose the number of samples to divide sampleTime exactly, but low enough for the ADC to keep up                           
const unsigned long numSamples = 250UL;
// the sampling interval, must be longer than then ADC conversion time                            
const unsigned long sampleInterval = sampleTime/numSamples;  
const float ampFactor = (50.76053976653696 / 1024.0);

class acs712
{
public:
  void determineVQ(int PIN) {
    long VQ = 0;
    //read 5000 samples to stabilise value
    for (int i=0; i<5000; i++) {
      VQ += analogRead(PIN);
      delay(1);//depends on sampling (on filter capacitor), can be 1/80000 (80kHz) max.
    }
    VQ /= 5000;

    #ifdef DEBUG
      printf_P(PSTR("ACS712: Info: estimating avg. quiscent voltage: %d\n\r"), 
        int(VQ));//map(VQ, 0, 1024, 0, 5000));
    #endif
    adc_zero = int(VQ);
  }

  /*float readCurrent(int PIN)
  {
    unsigned long currentAcc = 0;
    unsigned int count = 0;
    unsigned long prevMicros = micros() - sampleInterval ;
    while (count < numSamples)
    {
      if (micros() - prevMicros >= sampleInterval)
      {
        long adc_raw = analogRead(currentPin) - adc_zero;
        currentAcc += (unsigned long)(adc_raw * adc_raw);
        ++count;
        prevMicros += sampleInterval;
      }
    }
    float adc = sqrt((float)currentAcc/(float)numSamples);
    float rms = adc * (50 / 1024.0);
    #ifdef DEBUG
      Serial.print("ADC: "); Serial.println(adc);
      Serial.print("RMS: "); Serial.println(rms);
      printf_P(PSTR("ACS712: Info: amperage: %d mA.\n\r"), int(rms));
    #endif
    return rms;
  }*/

  //reads rms current from sensor
  float readCurrent(int PIN)
  {
    float iAmps, curWatts, curAmps;
    unsigned long sumAmps = 0;
    unsigned int count = 0;
    unsigned long prevMicros = micros() - sampleInterval ;
    while (count < numSamples)
    {
      if (micros() - prevMicros >= sampleInterval)
      {
        iAmps = (analogRead(PIN) - adc_zero);
        sumAmps += (unsigned long)(iAmps * iAmps);
      
        iAmps = (iAmps * ampFactor);
        ++count;
        prevMicros += sampleInterval;
      }
    }
  
    curAmps = sqrt(((float)sumAmps/(float)numSamples)) * ampFactor; 
    curAmps -= .01;
    if (curAmps < .03) { 
      curAmps = 0; 
    }
    curWatts = curAmps * 218;

    #ifdef DEBUG
      Serial.print("amps: "); Serial.println(curAmps);
      Serial.print("watts: "); Serial.println(curWatts);
    #endif
    return curWatts;
  }

private:
  //autoadjusted relative digital zero
  int adc_zero;

};

#endif // __ACS712_H__
