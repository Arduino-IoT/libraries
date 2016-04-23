#include "Makeblock.h"
#include <avr/interrupt.h>
#include <Arduino.h> 

MePort_Sig mePort[11] = {{NC, NC}, {11, 10}, {3, 9}, {13, 12}, {2, 8},
                      {1, 0}, {17, 16}, {15,NC}, {14, NC}, {5,4}, {6, 7}}; 

/*        Port       */
MePort::MePort(uint8_t port)
{
	s1 = mePort[port].s1;
    s2 = mePort[port].s2;
}
bool MePort::read1() {
    bool val;
    val = digitalRead(s1);         
    return val;
}

bool MePort::read2() {
    bool val;
    val = digitalRead(s2);         
    return val;
}
/*      Digital Input       */

MeDigital::MeDigital(uint8_t port):MePort(port)
{
    pinMode(s1, INPUT);
    pinMode(s2, INPUT);
}

/*      Analog Input       */

MeAnalog::MeAnalog(uint8_t port):MePort(port)
{
    pinMode(s1, INPUT);
    pinMode(s2, INPUT);
}

int MeAnalog::read1() {
    int val;
    val = analogRead(s1);         
    return val;
}

int MeAnalog::read2() {
    int val;
    val = analogRead(s2);         
    return val;
}
/*      Output       */

MeOutput::MeOutput(uint8_t port):MePort(port)
{
    pinMode(s1, OUTPUT);
    pinMode(s2, OUTPUT);
}

void MeOutput::write1(int value) {
    if(value<=1023&&value>=-1023){
		analogWrite(s1,value*0.25);         
    }
}

void MeOutput::write2(int value) {
    if(value<=1023&&value>=-1023){
		analogWrite(s2,value*0.25);         
    }
}
/*             Wire               */
MeWire::MeWire(uint8_t port):MePort(port)
{
}
void MeWire::begin(int slaveAddress)
{
	_slaveAddress = slaveAddress;
	Wire.begin();
}
byte MeWire::read(byte dataAddress)
{
  byte rxByte;
  Wire.beginTransmission(_slaveAddress); // transmit to device
  Wire.write(dataAddress); // sends one byte
  Wire.endTransmission(); // stop transmitting
  delayMicroseconds(1);
  Wire.requestFrom(_slaveAddress,1); // request 6 bytes from slave device
  while(Wire.available()) // slave may send less than requested
    return rxByte = Wire.read(); // receive a byte as character
  return 0;
}

void MeWire::write(byte dataAddress, byte data)
{
  Wire.beginTransmission(_slaveAddress); // transmit to device
  Wire.write(dataAddress); // sends one byte
  Wire.endTransmission(); // stop transmitting
  
  Wire.beginTransmission(_slaveAddress); // transmit to device
  Wire.write(data); // sends one byte
  Wire.endTransmission(); // stop transmitting
}


/*             Serial                  */
MeSerial::MeSerial(uint8_t port):MePort(port),swSerial(s1,s2){
	if(port==PORT_5){
		_hard = true;
	}else{
		_hard = false;
	}
}
void MeSerial::begin(long baudrate){
	if(_hard){
		Serial.begin(baudrate);
	}else{
		swSerial.begin(baudrate);
	}
}
size_t MeSerial::write(uint8_t byte){
	if(_isServoBusy==true)return -1;
	if(_hard)
	return Serial.write(byte);
	return swSerial.write(byte);
}
int MeSerial::read(){
	if(_isServoBusy==true)return -1;
	if(_hard)
	return Serial.read();
	return swSerial.read();
}
int MeSerial::available(){
	if(_hard)
		return Serial.available();
	return swSerial.available();
}
bool MeSerial::listen(){
	if(_hard)
		return true;
	return swSerial.listen();
}
bool MeSerial::isListening(){
	if(_hard)
		return true;
	return swSerial.isListening();
}
bool MeSerial::paramAvailable(){
	char c = this->read();
	if(c>-1){
		if(c=='\n'){
			_params.clear();
			char str[_index];
			_cmds[_index]='\0';
			strcpy(str,_cmds);
			findParamName(str,_index+1);
			_index=0;
			return true;
		}else{
			_cmds[_index] = c;
			_index++; 
		}
	}
	return false;
}
int MeSerial::getParamValue(char* str){
	return _params.getParamValue(str);
}
char* MeSerial::getParamCode(char*str){
	return _params.getParamCode(str);
}
MeParams MeSerial::getParams(){
  return _params;
}
void MeSerial::findParamName(char* str,int len){
  byte i = 0;
  for(i=0;i<len;i++){
   if(str[i]=='='){
    char name[i+1];
     memcpy(name,str,i);
     name[i]='\0';
     char s[len];
      int j;
      for(j=i+1;j<len;j++){
       s[j-i-1]=str[j]; 
      }
     findParamValue(s,len-i-1,name);
       break;
   } 
  }
}
void MeSerial::findParamValue(char* str,int len,char*name){
  byte i = 0;
  for(i=0;i<len;i++){ 
   if(str[i]=='&'||str[i]=='\0'||i==len-1){
    char v[i+1];
     memcpy(v,str,i);
     v[i]='\0';
	 
     _params.setParam(name,v);
     if(i<len-1){
       char s[len];
      int j;
      for(j=i+1;j<len;j++){
       s[j-i-1]=str[j]; 
      }
       findParamName(s,len-i-1);
       break;
     }
   } 
  }
}
/*             LineFinder              */
MeLineFinder::MeLineFinder(uint8_t port):MeDigital(port){

}
int MeLineFinder::readSensors(){
	int state = S1_IN_S2_IN;
	int s1State = MeDigital::read1();
	int s2State = MeDigital::read2();
	state = ((1&s1State)<<1)|s2State;
	return state;
}
int MeLineFinder::readSensor1(){
	return MeDigital::read1();
}
int MeLineFinder::readSensor2(){
	return MeDigital::read2();
}
/*             LimitSwitch              */
MeLimitSwitch::MeLimitSwitch(uint8_t port):MeDigital(port){
}
bool MeLimitSwitch::touched(){
	return MeDigital::read1();
}
/*             MotorDriver              */
MeDCMotor::MeDCMotor(uint8_t port):MeOutput(port){

}
void MeDCMotor::run(int speed){
	speed = speed>255? 255:speed;
	speed = speed<-255?-255:speed;

	if(speed>=0)
	{
		MeOutput::write2(1023);
		MeOutput::write1(speed*4);
	}
	else
	{
		MeOutput::write2(0);
		MeOutput::write1(-speed*4);
	}
}
void MeDCMotor::stop(){
	MeOutput::write1(0);
}
/*           UltrasonicSenser                 */
MeUltrasonicSensor::MeUltrasonicSensor(uint8_t port):MeOutput(port){
}
long MeUltrasonicSensor::distanceCm(){
	long distance = measure();
	return ((distance/29)>>1);
}
long MeUltrasonicSensor::distanceInch(){
	long distance = measure();
	return ((distance/74)>>1);
}
long MeUltrasonicSensor::measure(){
	long duration;
	pinMode(s2, OUTPUT);
	MeOutput::write1(0);
	delayMicroseconds(2);
	MeOutput::write1(1023);
	delayMicroseconds(10);
	MeOutput::write1(0);
	pinMode(s1,INPUT);
	duration = pulseIn(s1,HIGH);
	return duration;
}
/*          shutter       */
MeShutter::MeShutter(uint8_t port):MeOutput(port)
{
	MeOutput::write1(1023);
	MeOutput::write2(1023);
}
void MeShutter::shotOn(){
	MeOutput::write1(0);
}
void MeShutter::shotOff(){

	MeOutput::write1(1023);
}
void MeShutter::focusOn(){
	MeOutput::write2(0);
}
void MeShutter::focusOff(){
	MeOutput::write2(1023);
}

/*           Bluetooth                 */
MeBluetooth::MeBluetooth(uint8_t port):MeSerial(port)
{
}

/*           Infrared Receiver                 */
MeInfraredReceiver::MeInfraredReceiver(uint8_t port):MeSerial(port)
{
}
void MeInfraredReceiver::begin(){
	MeSerial::begin(9600);
	pinMode(s1, INPUT);
	pinMode(s2, INPUT);
}
bool MeInfraredReceiver::buttonState()        // Not available in Switching mode
{
	return !read2();
}
/*         LED Strip        */
// portNum can ONLY be PORT_1 or PORT_2
MeLedStrip::MeLedStrip(uint8_t port):MeWire(port)
{

}
// initialize ledStrip Driver and set the led quantity. (value: 1-60)
void MeLedStrip::begin(int ledCount){
	MeWire::begin(0x05); // join i2c bus (address optional for master)
    MeWire::write(LS_LED_COUNT, ledCount);
    reset();
}
void MeLedStrip::autoFlash(int flashSpeed){
	MeWire::write(LS_SET_SPEED, flashSpeed);
	MeWire::write(LS_RUN_CTRL, LS_AUTO_FLASH);
}

void MeLedStrip::onceFlash(){
	MeWire::write(LS_RUN_CTRL, LS_ONCE_FLASH);
}

void MeLedStrip::stopFlash(){
	MeWire::write(LS_RUN_CTRL, LS_STOP_FLASH);
}


void MeLedStrip::reset(){
	MeWire::write(LS_RUN_CTRL, LS_RESET);
}


bool MeLedStrip::readState(){
	if(MeWire::read(LS_RUN_STATE))
		return true;
	  else
		return false;
}

void MeLedStrip::setPixelColor(byte lsNum, byte lsR,byte lsG, byte lsB, byte lsMode){
	MeWire::write(LS_SET_PIXEL_R, lsR);
	MeWire::write(LS_SET_PIXEL_G, lsG);
	MeWire::write(LS_SET_PIXEL_B, lsB);
	MeWire::write(LS_SET_PIXEL_NUM, lsNum);
	MeWire::write(LS_RUN_CTRL, lsMode);
}


void MeLedStrip::color_loop(){
	MeWire::write(LS_RUN_CTRL, LS_COLOR_LOOP);
}

void MeLedStrip::indicators(byte lsNum, byte lsR, byte lsG, byte lsB, byte lsSpd){
	MeWire::write(LS_SET_COUNT, lsNum);
    MeWire::write(LS_SET_IN_SPEED, lsSpd);
    MeWire::write(LS_SET_PIXEL_R, lsR);
	MeWire::write(LS_SET_PIXEL_G, lsG);
	MeWire::write(LS_SET_PIXEL_B, lsB);
	MeWire::write(LS_RUN_CTRL, LS_INDICATORS);
}
/*			Stepper		*/
MeStepperMotor::MeStepperMotor(uint8_t port):MeWire(port)
{
}

void MeStepperMotor::begin(byte microStep,long speed,long acceleration)
{
    MeWire::begin(0x04); // join i2c bus (address optional for master)
    setCurrentPosition(0);
	enable();
	setMicroStep(microStep);
	setMaxSpeed(speed);
	setAcceleration(acceleration);
}

void MeStepperMotor::setMicroStep(byte microStep)
{
	MeWire::write(STP_MS_CTRL, microStep);
}

void MeStepperMotor::reset()
{
	MeWire::write(STP_RUN_CTRL, STP_RESET_CTRL);
}

void MeStepperMotor::moveTo(long stepperMoveTo)
{
	MeWire::write(STP_MOVE_TO_L1, *((char *)(&stepperMoveTo)));
	MeWire::write(STP_MOVE_TO_L2, *((char *)(&stepperMoveTo) + 1));
	MeWire::write(STP_MOVE_TO_H1, *((char *)(&stepperMoveTo) + 2));
	MeWire::write(STP_MOVE_TO_H2, *((char *)(&stepperMoveTo) + 3));
}

void MeStepperMotor::move(long stepperMove)
{
	MeWire::write(STP_MOVE_L1, *((char *)(&stepperMove)));
	MeWire::write(STP_MOVE_L2, *((char *)(&stepperMove) + 1));
	MeWire::write(STP_MOVE_H1, *((char *)(&stepperMove) + 2));
	MeWire::write(STP_MOVE_H2, *((char *)(&stepperMove) + 3));
}

void MeStepperMotor::runSpeed()
{
	MeWire::write(STP_RUN_CTRL, STP_RUN_SPEED);
}

void MeStepperMotor::setMaxSpeed(long stepperMaxSpeed)
{
	MeWire::write(STP_MAX_SPEED_L1, *((char *)(&stepperMaxSpeed)));
	MeWire::write(STP_MAX_SPEED_L2, *((char *)(&stepperMaxSpeed) + 1));
	MeWire::write(STP_MAX_SPEED_H1, *((char *)(&stepperMaxSpeed) + 2));
	MeWire::write(STP_MAX_SPEED_H2, *((char *)(&stepperMaxSpeed) + 3));
}

void MeStepperMotor::setAcceleration(long stepperAcceleration)
{
	MeWire::write(STP_ACC_L1, *((char *)(&stepperAcceleration)));
	MeWire::write(STP_ACC_L2, *((char *)(&stepperAcceleration) + 1));
	MeWire::write(STP_ACC_H1, *((char *)(&stepperAcceleration) + 2));
	MeWire::write(STP_ACC_H2, *((char *)(&stepperAcceleration) + 3));
}

void MeStepperMotor::setSpeed(long stepperSpeed)
{
	MeWire::write(STP_SPEED_L1, *((char *)(&stepperSpeed)));
	MeWire::write(STP_SPEED_L2, *((char *)(&stepperSpeed) + 1));
	MeWire::write(STP_SPEED_H1, *((char *)(&stepperSpeed) + 2));
	MeWire::write(STP_SPEED_H2, *((char *)(&stepperSpeed) + 3));
}

long MeStepperMotor::speed()
{
	*((char *)(&stepperSpeedRead)) = MeWire::read(STP_SPEED_RL1);
	*((char *)(&stepperSpeedRead)+1) = MeWire::read(STP_SPEED_RL2);
	*((char *)(&stepperSpeedRead)+2) = MeWire::read(STP_SPEED_RH1);
	*((char *)(&stepperSpeedRead)+3) = MeWire::read(STP_SPEED_RH2);
	return stepperSpeedRead;
}

long MeStepperMotor::distanceToGo()
{
	*((char *)(&stepperDistanceToGoRead)) = MeWire::read(STP_DIS_TOGO_RL1);
	*((char *)(&stepperDistanceToGoRead)+1) = MeWire::read(STP_DIS_TOGO_RL2);
	*((char *)(&stepperDistanceToGoRead)+2) = MeWire::read(STP_DIS_TOGO_RH1);
	*((char *)(&stepperDistanceToGoRead)+3) = MeWire::read(STP_DIS_TOGO_RH2);
	return stepperDistanceToGoRead;
}

long MeStepperMotor::targetPosition()
{
	*((char *)(&stepperTargetPositionRead)) = MeWire::read(STP_TARGET_POS_RL1);
	*((char *)(&stepperTargetPositionRead)+1) = MeWire::read(STP_TARGET_POS_RL2);
	*((char *)(&stepperTargetPositionRead)+2) = MeWire::read(STP_TARGET_POS_RH1);
	*((char *)(&stepperTargetPositionRead)+3) = MeWire::read(STP_TARGET_POS_RH2);
	return stepperTargetPositionRead;
}

long MeStepperMotor::currentPosition()
{
	*((char *)(&stepperCurrentPositionRead)) = MeWire::read(STP_CURRENT_POS_RL1);
	*((char *)(&stepperCurrentPositionRead)+1) = MeWire::read(STP_CURRENT_POS_RL2);
	*((char *)(&stepperCurrentPositionRead)+2) = MeWire::read(STP_CURRENT_POS_RH1);
	*((char *)(&stepperCurrentPositionRead)+3) = MeWire::read(STP_CURRENT_POS_RH2);
	return stepperCurrentPositionRead;
}

void MeStepperMotor::setCurrentPosition(long stepperCurrentPos)
{
	MeWire::write(STP_CURRENT_POS_L1, *((char *)(&stepperCurrentPos)));
	MeWire::write(STP_CURRENT_POS_L2, *((char *)(&stepperCurrentPos) + 1));
	MeWire::write(STP_CURRENT_POS_H1, *((char *)(&stepperCurrentPos) + 2));
	MeWire::write(STP_CURRENT_POS_H2, *((char *)(&stepperCurrentPos) + 3));
}

void MeStepperMotor::enable()
{
	MeWire::write(STP_EN_CTRL, STP_ENABLE);
}

void MeStepperMotor::disable()
{
	MeWire::write(STP_EN_CTRL, STP_DISABLE);
}

void MeStepperMotor::run()
{
	MeWire::write(STP_RUN_CTRL, STP_RUN);
}

void MeStepperMotor::stop()
{
	MeWire::write(STP_RUN_CTRL, STP_STOP);
}

void MeStepperMotor::wait()
{
	MeWire::write(STP_RUN_CTRL, STP_WAIT);
}

bool MeStepperMotor::readState()
{
	if(MeWire::read(STP_RUN_STATE))
		return true;
	else
		return false;
}
MeParams::MeParams(){
	_root = createObject();
	memset(_root->child,0,sizeof(MeParamObject));
}

MeParamObject* MeParams::getParam(const char *string){
	MeParamObject *c = _root->child;
	while (c && strcasecmp(c->name, string))
		c = c->next;
	return c;
}
void MeParams::setParam(const char* name,char* n){
	double v = strtod(n,NULL);
	deleteParam(name);
	if(v==NULL){
		addItemToObject(name, createCharItem(n));
	}else{
		addItemToObject(name, createItem(v));
	}
}
double MeParams::getParamValue(const char *string){
	return getParam(string)->value;
}
char* MeParams::getParamCode(const char *string){
	return getParam(string)->code;
}
void MeParams::clear(){
	unsigned char i = 0;
	MeParamObject *c = _root->child;
	while (c)
		i++, c = c->next,deleteParam(c->name);
	
}
void MeParams::deleteParam(const char *string)
{
  deleteItemFromRoot(detachItemFromObject(string));
}
MeParamObject* MeParams::createObject(){
	MeParamObject* item = (MeParamObject*) malloc(sizeof(MeParamObject));
	if (item){
		memset(item, 0, sizeof(MeParamObject));
	}
	return item;
}
MeParamObject* MeParams::createItem(double n){
	MeParamObject* item = (MeParamObject*) malloc(sizeof(MeParamObject));
	if (item){
		memset(item, 0, sizeof(MeParamObject));
		item->value = n;
	}
	return item;
}
MeParamObject* MeParams::createCharItem(char *n){
	MeParamObject* item = (MeParamObject*) malloc(sizeof(MeParamObject));
	if (item){
		memset(item, 0, sizeof(MeParamObject));
		item->code = n;
	}
	return item;
}

void MeParams::addItemToObject(const char *string,MeParamObject *item){
  if (!item)
    return;
  if (item->name)
    free(item->name);
  item->name = strdup(string);
  MeParamObject *c = _root->child;
  if (!item)
    return;
	
  if (!c)
    {
      _root->child = item;
    }
  else
    {
      while (c && c->next)
        c = c->next;
      suffixObject(c, item);
    }
}
void MeParams::deleteItemFromRoot(MeParamObject *c)
{
  MeParamObject *next;
  while (c)
    {
      next = c->next;
      if (c->name)
        {
          free(c->name);
        }
      free(c);
      c = next;
    }
}
MeParamObject* MeParams::detachItemFromObject( const char *string)
{
  unsigned char i = 0;
  MeParamObject *c = _root->child;
  while (c && strcasecmp(c->name, string))
    i++, c = c->next;
  if (c)
    return detachItemFromArray(i);
  return 0;
}
void MeParams::deleteItemFromArray(unsigned char which)
{
  deleteItemFromRoot(detachItemFromArray(which));
}
MeParamObject* MeParams::detachItemFromArray(unsigned char which)
{
  MeParamObject *c = _root->child;
  while (c && which > 0)
    c = c->next, which--;
  if (!c)
    return 0;
	
  if (c->prev)
    c->prev->next = c->next;
  if (c->next)
    c->next->prev = c->prev;
  if (c == _root->child)
    _root->child = c->next;
  c->prev = c->next = 0;
  return c;
}

void MeParams::suffixObject(MeParamObject *prev, MeParamObject *item)
{
  prev->next = item;
  item->prev = prev;
}

#define usToTicks(_us)    (( clockCyclesPerMicrosecond()* _us) / 8)     // converts microseconds to tick (assumes prescale of 8)  // 12 Aug 2009
#define ticksToUs(_ticks) (( (unsigned)_ticks * 8)/ clockCyclesPerMicrosecond() ) // converts from ticks back to microseconds


#define TRIM_DURATION       2                               // compensation ticks to trim adjust for digitalWrite delays // 12 August 2009

//#define NBR_TIMERS        (MAX_SERVOS / SERVOS_PER_TIMER)

static servo_t servos[MAX_SERVOS];                          // static array of servo structures
static volatile int8_t Channel[_Nbr_16timers ];             // counter for the servo being pulsed for each timer (or -1 if refresh interval)

uint8_t ServoCount = 0;                                     // the total number of attached servos

// convenience macros
#define SERVO_INDEX_TO_TIMER(_servo_nbr) ((timer16_Sequence_t)(_servo_nbr / SERVOS_PER_TIMER)) // returns the timer controlling this servo
#define SERVO_INDEX_TO_CHANNEL(_servo_nbr) (_servo_nbr % SERVOS_PER_TIMER)       // returns the index of the servo on this timer
#define SERVO_INDEX(_timer,_channel)  ((_timer*SERVOS_PER_TIMER) + _channel)     // macro to access servo index by timer and channel
#define SERVO(_timer,_channel)  (servos[SERVO_INDEX(_timer,_channel)])            // macro to access servo class by timer and channel

#define SERVO_MIN() (MIN_PULSE_WIDTH - this->min * 4)  // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH - this->max * 4)  // maximum value in uS for this servo 

/************ static functions common to all instances ***********************/
static bool isTimerActive(timer16_Sequence_t timer)
{
  // returns true if any servo is active on this timer
  for(uint8_t channel=0; channel < SERVOS_PER_TIMER; channel++) {
    if(SERVO(timer,channel).Pin.isActive == true)
      return true;
  }
  return false;
}
static void finISR(timer16_Sequence_t timer)
{
    //disable use of the given timer
#if defined WIRING   // Wiring
  if(timer == _timer1) {
    #if defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
    TIMSK1 &=  ~_BV(OCIE1A) ;  // disable timer 1 output compare interrupt
    #else 
    TIMSK &=  ~_BV(OCIE1A) ;  // disable timer 1 output compare interrupt   
    #endif
    timerDetach(TIMER1OUTCOMPAREA_INT); 
  }
  else if(timer == _timer3) {     
    #if defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
    TIMSK3 &= ~_BV(OCIE3A);    // disable the timer3 output compare A interrupt
    #else
    ETIMSK &= ~_BV(OCIE3A);    // disable the timer3 output compare A interrupt
    #endif
    timerDetach(TIMER3OUTCOMPAREA_INT);
  }
#else
    //For arduino - in future: call here to a currently undefined function to reset the timer
#endif
}
static inline void handle_interrupts(timer16_Sequence_t timer, volatile uint16_t *TCNTn, volatile uint16_t* OCRnA)
{
	if( Channel[timer] < 0 ){
		*TCNTn = 0; // channel set to -1 indicated that refresh interval completed so reset the timer 
		 _isServoBusy = false;
	}else{
		if( SERVO_INDEX(timer,Channel[timer]) < ServoCount && SERVO(timer,Channel[timer]).Pin.isActive == true )  {
		  digitalWrite( SERVO(timer,Channel[timer]).Pin.nbr,LOW); // pulse this channel low if activated   
		  _isServoBusy = false;
		}
	}

  Channel[timer]++;    // increment to the next channel
  if( SERVO_INDEX(timer,Channel[timer]) < ServoCount && Channel[timer] < SERVOS_PER_TIMER) {
    *OCRnA = *TCNTn + SERVO(timer,Channel[timer]).ticks;
    if(SERVO(timer,Channel[timer]).Pin.isActive == true){     // check if activated
      digitalWrite( SERVO(timer,Channel[timer]).Pin.nbr,HIGH); // its an active channel so pulse it high   
		_isServoBusy = true;
	  
	}
	  
  }  
  else { 
  
    // finished all channels so wait for the refresh period to expire before starting over 
    if( ((unsigned)*TCNTn) + 4 < usToTicks(REFRESH_INTERVAL) )  // allow a few ticks to ensure the next OCR1A not missed
      *OCRnA = (unsigned int)usToTicks(REFRESH_INTERVAL);  
    else 
      *OCRnA = *TCNTn + 4;  // at least REFRESH_INTERVAL has elapsed
	
	
    Channel[timer] = -1; // this will get incremented at the end of the refresh period to start again at the first channel
	
  }
}

#ifndef WIRING // Wiring pre-defines signal handlers so don't define any if compiling for the Wiring platform
// Interrupt handlers for Arduino 
#if defined(_useTimer1)
ISR(TIMER1_COMPA_vect) 
{ 
  handle_interrupts(_timer1, &TCNT1, &OCR1A); 
}
#endif
#if defined(_useTimer3)
ISR(TIMER3_COMPA_vect) 
{ 
  handle_interrupts(_timer3, &TCNT3, &OCR3A); 
}
#endif

#if defined(_useTimer4)
ISR(TIMER4_COMPA_vect) 
{
  handle_interrupts(_timer4, &TCNT4, &OCR4A); 
}
#endif

#if defined(_useTimer5)
ISR(TIMER5_COMPA_vect) 
{
  handle_interrupts(_timer5, &TCNT5, &OCR5A); 
}
#endif

#elif defined WIRING
// Interrupt handlers for Wiring 
#if defined(_useTimer1)
void Timer1Service() 
{ 
  handle_interrupts(_timer1, &TCNT1, &OCR1A); 
}
#endif
#if defined(_useTimer3)
void Timer3Service() 
{ 
  handle_interrupts(_timer3, &TCNT3, &OCR3A); 
}
#endif
#endif


static void initISR(timer16_Sequence_t timer)
{  
#if defined (_useTimer1)
  if(timer == _timer1) {
    TCCR1A = 0;             // normal counting mode 
    TCCR1B = _BV(CS11);     // set prescaler of 8 
    TCNT1 = 0;              // clear the timer count 
#if defined(__AVR_ATmega8__)|| defined(__AVR_ATmega128__)
    TIFR |= _BV(OCF1A);      // clear any pending interrupts; 
    TIMSK |=  _BV(OCIE1A) ;  // enable the output compare interrupt  
#else
    // here if not ATmega8 or ATmega128
    TIFR1 |= _BV(OCF1A);     // clear any pending interrupts; 
    TIMSK1 |=  _BV(OCIE1A) ; // enable the output compare interrupt 
#endif    
#if defined(WIRING)       
    timerAttach(TIMER1OUTCOMPAREA_INT, Timer1Service); 
#endif	
  } 
#endif  


#if defined (_useTimer3)
  if(timer == _timer3) {
    TCCR3A = 0;             // normal counting mode 
    TCCR3B = _BV(CS31);     // set prescaler of 8  
    TCNT3 = 0;              // clear the timer count 
#if defined(__AVR_ATmega128__)
    TIFR |= _BV(OCF3A);     // clear any pending interrupts;   
	ETIMSK |= _BV(OCIE3A);  // enable the output compare interrupt     
#else  
    TIFR3 = _BV(OCF3A);     // clear any pending interrupts; 
    TIMSK3 =  _BV(OCIE3A) ; // enable the output compare interrupt      
#endif
#if defined(WIRING)    
    timerAttach(TIMER3OUTCOMPAREA_INT, Timer3Service);  // for Wiring platform only	
#endif  
  }
#endif

#if defined (_useTimer4)
  if(timer == _timer4) {
    TCCR4A = 0;             // normal counting mode 
    TCCR4B = _BV(CS41);     // set prescaler of 8  
    TCNT4 = 0;              // clear the timer count 
    TIFR4 = _BV(OCF4A);     // clear any pending interrupts; 
    TIMSK4 =  _BV(OCIE4A) ; // enable the output compare interrupt
  }    
#endif

#if defined (_useTimer5)
  if(timer == _timer5) {
    TCCR5A = 0;             // normal counting mode 
    TCCR5B = _BV(CS51);     // set prescaler of 8  
    TCNT5 = 0;              // clear the timer count 
    TIFR5 = _BV(OCF5A);     // clear any pending interrupts; 
    TIMSK5 =  _BV(OCIE5A) ; // enable the output compare interrupt      
  }
#endif
} 





/****************** end of static functions ******************************/

MeServo::MeServo(uint8_t port,uint8_t device):MePort(port)
{
	servoPin =( device == DEV1 ? s2 : s1);
  if( ServoCount < MAX_SERVOS) {
    this->servoIndex = ServoCount++;                    // assign a servo index to this instance
	servos[this->servoIndex].ticks = usToTicks(DEFAULT_PULSE_WIDTH);   // store default values  - 12 Aug 2009
  }
  else
    this->servoIndex = INVALID_SERVO ;  // too many servos 
}

uint8_t MeServo::begin()
{
  return this->begin(MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t MeServo::begin(int min, int max)
{
  if(this->servoIndex < MAX_SERVOS ) {
    pinMode( servoPin, OUTPUT) ;                                   // set servo pin to output
    servos[this->servoIndex].Pin.nbr = servoPin;  
    // todo min/max check: abs(min - MIN_PULSE_WIDTH) /4 < 128 
    this->min  = (MIN_PULSE_WIDTH - min)/4; //resolution of min/max is 4 uS
    this->max  = (MAX_PULSE_WIDTH - max)/4; 
    // initialize the timer if it has not already been initialized 
    timer16_Sequence_t timer = SERVO_INDEX_TO_TIMER(servoIndex);
    if(isTimerActive(timer) == false)
      initISR(timer);    
    servos[this->servoIndex].Pin.isActive = true;  // this must be set after the check for isTimerActive
  } 
  return this->servoIndex ;
}

void MeServo::detach()  
{
  servos[this->servoIndex].Pin.isActive = false;  
  timer16_Sequence_t timer = SERVO_INDEX_TO_TIMER(servoIndex);
  if(isTimerActive(timer) == false) {
    finISR(timer);
  }
}

void MeServo::write(int value)
{  
int delayTime = abs(value-this->read());
	this->begin();
  if(value < MIN_PULSE_WIDTH)
  {  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
    if(value < 0) value = 0;
    if(value > 180) value = 180;
    value = map(value, 0, 180, SERVO_MIN(),  SERVO_MAX());      
  }
  this->writeMicroseconds(value);
  delay(delayTime);
  this->detach();
}

void MeServo::writeMicroseconds(int value)
{
  // calculate and store the values for the given channel
  byte channel = this->servoIndex;
  if( (channel < MAX_SERVOS) )   // ensure channel is valid
  {  
    if( value < SERVO_MIN() )          // ensure pulse width is valid
      value = SERVO_MIN();
    else if( value > SERVO_MAX() )
      value = SERVO_MAX();   
    
  	value = value - TRIM_DURATION;
    value = usToTicks(value);  // convert to ticks after compensating for interrupt overhead - 12 Aug 2009

    uint8_t oldSREG = SREG;
    cli();
    servos[channel].ticks = value;  
    SREG = oldSREG;   
  } 
}

int MeServo::read() // return the value as degrees
{
  return  map( this->readMicroseconds()+1, SERVO_MIN(), SERVO_MAX(), 0, 180);     
}

int MeServo::readMicroseconds()
{
  unsigned int pulsewidth;
  if( this->servoIndex != INVALID_SERVO )
    pulsewidth = ticksToUs(servos[this->servoIndex].ticks)  + TRIM_DURATION ;   // 12 aug 2009
  else 
    pulsewidth  = 0;

  return pulsewidth;   
}

bool MeServo::attached()
{
  return servos[this->servoIndex].Pin.isActive ;
}