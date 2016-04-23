///@file Makeblock.h head file of Makeblock Library V2.1
///Define the interface of Makeblock Library
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
//#include <inttypes.h>

#ifndef Makeblock_h
#define Makeblock_h
#define NC -1

#define PORT_1 0x01
#define PORT_2 0x02
#define PORT_3 0x03
#define PORT_4 0x04
#define PORT_5 0x05
#define PORT_6 0x06
#define PORT_7 0x07
#define PORT_8 0x08
#define M1     0x09
#define M2     0x0a

#define DEV1 1
#define DEV2 2

///@brief Struct of MePort_Sig
typedef struct {
  uint8_t s1;
  uint8_t s2;
}MePort_Sig;
extern MePort_Sig mePort[11];//mePort[0] is nonsense




//states of two linefinder sensors
#define	S1_IN_S2_IN 0x00 //sensor1 and sensor2 are both inside of black line
#define	S1_IN_S2_OUT 0x01 //sensor1 is inside of black line and sensor is outside of black line
#define	S1_OUT_S2_IN 0x02 //sensor1 is outside of black line and sensor is inside of black line 
#define	S1_OUT_S2_OUT 0x03 //sensor1 is outside of black line and sensor is outside of black line

//ledstrip
//address table
#define LS_RUN_STATE 0x91
#define LS_CURRENT 0x92
#define LS_GET_PIXEL_R 0x93
#define LS_GET_PIXEL_G 0x94
#define LS_GET_PIXEL_B 0x95
#define LS_GET_PIXEL_NUM 0x96

#define LS_SET_PIXEL_NUM 0x02
#define LS_SET_PIXEL_R 0x03
#define LS_SET_PIXEL_G 0x04
#define LS_SET_PIXEL_B 0x05
#define LS_SET_SPEED 0x06
#define LS_SET_COUNT 0x07
#define LS_SET_IN_SPEED 0x08

#define LS_RUN_CTRL 0x1A
#define LS_LED_COUNT                 0x1B

//data table
#define LS_NO_FLASH                        0x00
#define LS_STOP_FLASH                0x00
#define LS_AUTO_FLASH                0x01
#define LS_ONCE_FLASH                0x02
#define LS_CLOSE                                0x04
#define LS_RESET                                0x05
#define LS_COLOR_LOOP                0x06
#define LS_INDICATORS 0x07
#define LS_ALL_PIXEL                 0xFE

#define MAX_BRI                                        200
#define MIN_BRI                                        0
#define IN_SPEED                                2


//stepper
//address table
#define STP_RUN_STATE 0x91
#define STP_SPEED_RL1 0x92
#define STP_SPEED_RL2 0x93
#define STP_SPEED_RH1 0x94
#define STP_SPEED_RH2 0x95
#define STP_DIS_TOGO_RL1 0x96
#define STP_DIS_TOGO_RL2 0x97
#define STP_DIS_TOGO_RH1 0x98
#define STP_DIS_TOGO_RH2 0x99
#define STP_TARGET_POS_RL1 0x9A
#define STP_TARGET_POS_RL2 0x9B
#define STP_TARGET_POS_RH1 0x9C
#define STP_TARGET_POS_RH2 0x9D
#define STP_CURRENT_POS_RL1 0x9E
#define STP_CURRENT_POS_RL2 0x9F
#define STP_CURRENT_POS_RH1 0xA0
#define STP_CURRENT_POS_RH2 0xA1

#define STP_ACC_L1 0x01 //This is an expensive call since it requires a square root to be calculated. Don't call more ofthen than needed.
#define STP_ACC_L2 0x02
#define STP_ACC_H1 0x03
#define STP_ACC_H2 0x04
#define STP_MAX_SPEED_L1 0x05
#define STP_MAX_SPEED_L2 0x06
#define STP_MAX_SPEED_H1 0x07
#define STP_MAX_SPEED_H2 0x08
#define STP_SPEED_L1 0x09
#define STP_SPEED_L2 0x0A
#define STP_SPEED_H1 0x0B
#define STP_SPEED_H2 0x0C
#define STP_MOVE_TO_L1 0x0D
#define STP_MOVE_TO_L2 0x0E
#define STP_MOVE_TO_H1 0x0F
#define STP_MOVE_TO_H2 0x10
#define STP_MOVE_L1 0x11
#define STP_MOVE_L2 0x12
#define STP_MOVE_H1 0x13
#define STP_MOVE_H2 0x14
#define STP_CURRENT_POS_L1 0x15
#define STP_CURRENT_POS_L2 0x16
#define STP_CURRENT_POS_H1 0x17
#define STP_CURRENT_POS_H2 0x18

#define STP_RUN_CTRL 0x1C
#define STP_EN_CTRL 0x1D
#define STP_SLEEP_CTRL 0x1F
#define STP_MS_CTRL 0x20

//data table
#define STP_RUN 0x01
#define STP_STOP 0x02
#define STP_WAIT 0x03
#define STP_RESET_CTRL 0x04
#define STP_RUN_SPEED 0x05
#define STP_TRUE 0x01
#define STP_FALSE 0x00
#define STP_ENABLE 0x01
#define STP_DISABLE 0x02
#define STP_FULL 0x01
#define STP_HALF 0x02
#define STP_QUARTER 0x04
#define STP_EIGHTH 0x08
#define STP_SIXTEENTH 0x16

//NEC Code table
#define IR_BUTTON_POWER 0x45
#define IR_BUTTON_MENU 0x47
#define IR_BUTTON_TEST 0x44
#define IR_BUTTON_PLUS 0x40
#define IR_BUTTON_RETURN 0x43
#define IR_BUTTON_PREVIOUS 0x07
#define IR_BUTTON_PLAY 0x15
#define IR_BUTTON_NEXT 0x09
#define IR_BUTTON_MINUS 0x19
#define IR_BUTTON_CLR 0x0D
#define IR_BUTTON_0 0x16
#define IR_BUTTON_1 0x0C
#define IR_BUTTON_2 0x18
#define IR_BUTTON_3 0x5E
#define IR_BUTTON_4 0x08
#define IR_BUTTON_5 0x1C
#define IR_BUTTON_6 0x5A
#define IR_BUTTON_7 0x42
#define IR_BUTTON_8 0x52
#define IR_BUTTON_9 0x4A

static bool _isServoBusy = false;
///@brief Struct of MeParamObject
typedef struct MeParamObject {
        char *name; 
        struct MeParamObject *next, *prev; 
        struct MeParamObject *child; 
        union {
				char *code;
                double value;
        };
} MeParamObject;

///@brief class of MeParams
class MeParams
{
	public:
		///@brief initialize
		MeParams();
		///@brief get the param object with name.
		///@param string param name
		///@return MeParamObject.
		MeParamObject* getParam(const char *string);
		///@brief get double value of the param with name.
		///@param string param name.
		///@return double value.
		double getParamValue(const char *string);
		///@brief get string value of the param with name.
		///@param string param name
		///@return string value.
		char* getParamCode(const char *string);
		void setParam(const char* name, char * n);
		void clear();
	protected:
		MeParamObject* _root;
		void suffixObject(MeParamObject *prev, MeParamObject *item);
		void addItemToObject(const char *string,MeParamObject *item);
		void deleteParam(const char *string);
		void deleteItemFromRoot(MeParamObject *c);
		MeParamObject* detachItemFromObject( const char *string);
		void deleteItemFromArray(unsigned char which);
		MeParamObject* detachItemFromArray(unsigned char which);
		MeParamObject* createObject();
		MeParamObject* createItem(double n);
		MeParamObject* createCharItem(char* n);
};
///@brief class of MePort,it contains two pin.
class MePort
{
	public:
		///@brief initialize the Port
		///@param port port number of device
		MePort(uint8_t port);
		///@return the level of pin 1 of port 
		///@retval true on HIGH.
		///@retval false on LOW.
		bool read1();
		///@return the level of pin 2 of port 
		///@retval true on HIGH.
		///@retval false on LOW.
		bool read2();
	protected:
		uint8_t s1;
		uint8_t s2;
};

///@brief class of MeDigital
class MeDigital:public MePort
{
	public:
		///@brief initialize
		///@param port port number of device
		MeDigital(uint8_t port);
};

///@brief class of MeAnalog
class MeAnalog:public MePort
{
	public:
		///@brief initialize
		///@param port port number of device
		MeAnalog(uint8_t port);
		///@return the analog signal of pin 1 of port between 0 to 1023
		int read1();
		///@return the analog signal of pin 2 of port between 0 to 1023
		int read2();
};
///@brief class of MeOutput
class MeOutput:public MePort
{
	public:
		///@brief initialize
		///@param port port number of device
		MeOutput(uint8_t port);
		///@brief set the analog value of pin 1 of port
		///@param value between 0 to 1023
		void write1(int value);
		///@brief set the analog value of pin 2 of port
		///@param value between 0 to 1023
		void write2(int value);
};
///@brief class of MeSerial
class MeSerial:public MePort
{
	public:
		///@brief initialize
		///@param port port number of device
		MeSerial(uint8_t port);
		///@brief Sets the data rate in bits per second (baud) for serial data transmission.
		///@param baudrate use one of these rates: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200. 
		void begin(long baudrate);
		///@brief Writes binary data to the serial port. This data is sent as a byte or series of bytes.
		///@param byte a value to send as a single byte 
		size_t write(uint8_t byte);
		///@brief the first byte of incoming serial data available (or -1 if no data is available) - int
		int read();
		///@brief Get the number of bytes (characters) available for reading from the serial port. This is data that's already arrived and stored in the serial receive buffer (which holds 64 bytes).
		int available();
		///@brief Get the param string available for reading from the serial port.
		bool paramAvailable();
		///@brief Get the param object with the param available
		///@return MeParams, the param string example:"motor_speed=100&servo_angle=45.4"
		MeParams getParams();
		int getParamValue(char* str);
		char* getParamCode(char*str);
		bool listen();
		bool isListening();
	protected:
		MeParams _params;
		void findParamName(char* str,int len);
		void findParamValue(char* str,int len,char*name);
		char _cmds[64];
		int _index;
		bool _hard;
		SoftwareSerial swSerial;
};
///@brief class of MeWire
class MeWire:public MePort
{
	public:
		///@brief initialize
		///@param port port number of device
		MeWire(uint8_t port);
		///@brief Initiate the Wire library and join the I2C bus as a master or slave. This should normally be called only once.
		///@param address the 7-bit slave address (optional); if not specified, join the bus as a master.
		void begin(int slaveAddress);
		///@brief send one byte data request for read one byte from slave address.
		byte read(byte dataAddress);
		///@brief send one byte data request for write one byte to slave address.
        void write(byte dataAddress, byte data);
	protected:
		int _slaveAddress;
};


///@brief Class for LineFinder Module
class MeLineFinder:public MeDigital
{
	public:
		///@brief initialize
		MeLineFinder(uint8_t port);
		///@brief state of all sensors
		///@return state of sensors
		int readSensors();
		///@brief state of left sensors
		int readSensor1();
		///@brief state of right sensors
		int readSensor2();
};
///@brief Class for Limit Switch Module
class MeLimitSwitch:public MeDigital
{
	public:
		MeLimitSwitch(uint8_t port);
		bool touched();		
};
///@brief Class for Ultrasonic Sensor Module
class MeUltrasonicSensor:public MeOutput
{
	public :
		MeUltrasonicSensor(uint8_t port);
		long distanceCm();
		long distanceInch();
		long measure();
};
///@brief Class for DC Motor Module
class MeDCMotor:public MeOutput
{
	public:
		MeDCMotor(uint8_t port);
		void run(int speed);
		void stop();
};
///@brief Class for Camera Shutter Module
class MeShutter:public MeOutput
{
	public:
		MeShutter(uint8_t port);
		void shotOn();
		void shotOff();
		void focusOn();
		void focusOff();
};
///@brief Class for Bluetooth Module
class MeBluetooth:public MeSerial
{
	public:
		MeBluetooth(uint8_t port);
};
///@brief Class for Infrared Receiver Module
class MeInfraredReceiver:public MeSerial
{
	public:
		MeInfraredReceiver(uint8_t port);
		void begin();
		///@brief check press state of button
		bool buttonState();
};
///@brief class for Led Strip Module
class MeLedStrip :public MeWire
{
	public:
        ///@brief initialize,portNum can ONLY be PORT_1 or PORT_2
        MeLedStrip(uint8_t port);
        
        ///@brief start ledStrip Driver and set the led quantity. (value: 1-60)
        void begin(int ledCount);
        
        ///@brief Automatic cycle refresh each LED. After perform this function, the led refresh automatically.You can set the reflash time. Max value = 255.
        void autoFlash(int flashSpeed = 0);
        
        ///@brief Refresh once LED, Entirely by the loop function to refresh the LED Strip. When you need to write multiple leds,use the LS_ONCE_FLASH mode to refresh all LED when writing the last LED, in front of the led use LS_NO_FLASH mode,it can speed up the refresh.
        void onceFlash();
        
        ///@brief Stop all led of strip. But it doesn't close the leds. All LED to keep the last state.
        void stopFlash();
        
        ///@brief Stop and reset all led of strip.
        void reset();
        
        bool readState();
        
        ///@brief Write each LED color and refresh mode.
        ///@param lsNum LED Number
		///@param lsR Red brightness
		///@param lsG Green brightness
		///@param lsB Blue brightness
		///@param lsMode LS_AUTO_FLASH : Automatic cycle refresh each LED. After perform this function, the led refresh automatically.This mode is not commonly used.LS_ONCE_FLASH : Please refer to the instructions of onceFlash().
        void setPixelColor(byte lsNum = LS_ALL_PIXEL, byte lsR = MIN_BRI,byte lsG = MIN_BRI, byte lsB = MIN_BRI, byte lsMode = LS_ONCE_FLASH);
        
		//long getPixelColor(byte n);

        ///@brief Color gradient LED Scroller function.Automatically refresh after initialization, you can use stopFlash() to stop flash, but it doesn't close leds.use the reset() to stop and reset led.
        void color_loop();
        
        ///@brief This is an indicator function that is used to display range quickly.
        ///@param lsNum LED Number
		///@param lsR Red brightness
		///@param lsG Green brightness
		///@param lsB Blue brightness
		///@param lsSpd Indicators flash speed
        void indicators(byte lsNum, byte lsR, byte lsG, byte lsB, byte lsSpd = IN_SPEED);
};
///@brief Class for Stepper Motor Driver
class MeStepperMotor:public MeWire
{
	public:
        ///@brief initialize,portNum can ONLY be PORT_1 or PORT_2
        MeStepperMotor(uint8_t port);
        
        ///@brief start stepper driver.
        void begin(byte microStep = STP_SIXTEENTH,long speed = 10000,long acceleration = 5000);
        
        ///@brief set micro step. 
		///@param microStep STP_FULL, STP_HALF, STP_QUARTER, STP_EIGHTH, STP_SIXTEENTH
        void setMicroStep(byte microStep);

        ///@brief stop stepper and reset current position to zero.
        void reset();
    
		///@brief Set the target position. The run() function will try to move the motor from the current position to the target position set by the most recent call to this function. Caution: moveTo() also recalculates the speed for the next step. If you are trying to use constant speed movements, you should call setSpeed() after calling moveTo().
		///@param stepperMoveTo absolute The desired absolute position. Negative is anticlockwise from the 0 position.
		void moveTo(long stepperMoveTo);

		///@brief Set the target position relative to the current position
		///@param stepperMove relative The desired position relative to the current position.Negative is anticlockwise from the current position.
		void move(long stepperMove);

		///@brief Poll the motor and step it if a step is due, implmenting a constant speed as set by the most recent call to setSpeed(). You must call this as frequently as possible, but at least once per step interval,
		void runSpeed();

		///@brief Sets the maximum permitted speed. the run() function will accelerate up to the speed set by this function.
		///@param stepperMaxSpeed speed The desired maximum speed in steps per second. Must be > 0. Caution: Speeds that exceed the maximum speed supported by the processor may Result in non-linear accelerations and decelerations.
		void setMaxSpeed(long stepperMaxSpeed);

		///@brief Sets the acceleration and deceleration parameter.
		///@param stepperAcceleration acceleration The desired acceleration in steps per second per second. Must be > 0.0. This is an expensive call since it requires a square root to be calculated. Dont call more ofthen than needed
		void setAcceleration(long stepperAcceleration);

		///@brief Sets the desired constant speed for use with runSpeed().
		///@param stepperSpeed speed The desired constant speed in steps per second. Positive is clockwise. Speeds of more than 1000 steps per second are unreliable. Very slow speeds may be set (eg 0.00027777 for once per hour, approximately. Speed accuracy depends on the Arduino crystal. Jitter depends on how frequently you call the runSpeed() function.
		void setSpeed(long stepperSpeed);

		///@brief The most recently set speed
		///@return the most recent speed in steps per second
		long speed();

		///@brief The distance from the current position to the target position.
		///@return the distance from the current position to the target position in steps.Positive is clockwise from the current position.
		long distanceToGo();

		///@brief The most recently set target position.
		///@return the target position in steps. Positive is clockwise from the 0 position.
		long targetPosition();

		///@brief The currently motor position.
		///@return the current motor position in steps. Positive is clockwise from the 0 position.
		long currentPosition();

		///@brief Resets the current position of the motor, so that wherever the motor happens to be right now is considered to be the new 0 position. Useful for setting a zero position on a stepper after an initial hardware positioning move.Has the side effect of setting the current motor speed to 0.
		///@param stepperCurrentPos position The position in steps of wherever the motor happens to be right now.
		void setCurrentPosition(long stepperCurrentPos);

        ///@brief enable stepper driver, Keep the micro step current position.
        void enable();
        
        ///@brief disable stepper driver, release the stepper.
        void disable();

        ///@brief output pulse
        void run();

        ///@brief Sets a new target position that causes the stepper to stop as quickly as possible, using to the current speed and acceleration parameters.
        void stop();

        ///@brief stop all dispose, keep the user setting data.
        void wait();

        bool readState();
        
	private:
        long stepperSpeedRead;
        long stepperDistanceToGoRead;
        long stepperTargetPositionRead;
        long stepperCurrentPositionRead;
};
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define _useTimer5
#define _useTimer1 
#define _useTimer3
#define _useTimer4 
typedef enum { _timer5, _timer1, _timer3, _timer4, _Nbr_16timers } timer16_Sequence_t ;

#elif defined(__AVR_ATmega32U4__)  
#define _useTimer1 
typedef enum { _timer1, _Nbr_16timers } timer16_Sequence_t ;

#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define _useTimer3
#define _useTimer1
typedef enum { _timer3, _timer1, _Nbr_16timers } timer16_Sequence_t ;

#elif defined(__AVR_ATmega128__) ||defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
#define _useTimer3
#define _useTimer1
typedef enum { _timer3, _timer1, _Nbr_16timers } timer16_Sequence_t ;

#else  // everything else
#define _useTimer1
typedef enum { _timer1, _Nbr_16timers } timer16_Sequence_t ;                  
#endif

#define Servo_VERSION           2      // software version of this library

#define MIN_PULSE_WIDTH       500     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2500     // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH  1500     // default pulse width when servo is attached
#define REFRESH_INTERVAL    20000     // minumim time to refresh servos in microseconds 

#define SERVOS_PER_TIMER       12     // the maximum number of servos controlled by one timer 
#define MAX_SERVOS   (_Nbr_16timers  * SERVOS_PER_TIMER)

#define INVALID_SERVO         255     // flag indicating an invalid servo index

typedef struct  {
  uint8_t nbr        :6 ;             // a pin number from 0 to 63
  uint8_t isActive   :1 ;             // true if this channel is enabled, pin not pulsed if false 
} ServoPin_t   ;  

typedef struct {
  ServoPin_t Pin;
  unsigned int ticks;
} servo_t;

///@brief Class for Servo Module
class MeServo:public MePort
{
public:
	MeServo(uint8_t port,uint8_t device);
	///@brief attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure
	uint8_t begin();    
	///@brief as above but also sets min and max values for writes.
	uint8_t begin(int min, int max);  
	void detach();
	///@brief if value is < 200 its treated as an angle, otherwise as pulse width in microseconds 
	void write(int value);        
	///@brief Write pulse width in microseconds 
	void writeMicroseconds(int value); 
	///@brief current pulse width as an angle between 0 and 180 degrees
	///@return angle between 0 and 180 degrees
	int read();    
	///@brief current pulse width in microseconds for this servo (was read_us() in first release)
	///@return microseconds
	int readMicroseconds();
	///@brief true if this servo is attached, otherwise false 
	bool attached(); 
private:
	///@brief index into the channel data for this servo
   uint8_t servoIndex;
   ///@brief minimum is this value times 4 added to MIN_PULSE_WIDTH
   int8_t min; 
   ///@brief maximum is this value times 4 added to MAX_PULSE_WIDTH
   int8_t max;
   int servoPin;
};
#endif