// Import libraries
#include "DHT11.h"
#include "SimpleMap.h"
#include "acs712.h"
#include "timer.h"
#include "led.h"
#include "MemoryFree.h"
#include "Watchdog.h"
#include "OneButton.h"
#include "LowPower.h"
#include "MeshNet.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "RF24Layer2.h"

// debug console
//#define DEBUG

// Declare output functions
static int serial_putchar(char c, FILE *) {
  Serial.write(c);
  return 0;
};
FILE serial_out = {0};

// Avoid spurious warnings
#if ! defined( NATIVE ) && defined( ARDUINO )
#undef PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))
#undef PSTR
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))
#endif

// DEVICE TYPE
const uint32_t deviceType = 001;
// DEVICE UNIQUE ID
uint32_t deviceUniqueId = 100001;
/** LAYER 2 DEPENDENT CODE **/
// Declare SPI bus pins
#define CE_PIN  9
#define CS_PIN  10
// Set up nRF24L01 radio
RF24 radio(CE_PIN, CS_PIN);
const uint8_t RF24_INTERFACE = 0;
// The number of network interfaces that this device has
const int NUM_INTERFACES = 1;
// Pass a layer3 packet to the layer2
int sendPacket(unsigned char* message, uint8_t len, uint8_t interface, uint8_t macAddress)
{  
    // Here should be called the layer2 function corresponding to interface
    if(interface == RF24_INTERFACE){
      rf24sendPacket(message, len, macAddress);
      return 1;
    }
    return 0;
}
void onCommandReceived(uint8_t command, void* data, uint8_t dataLen)
{
  #ifdef DEBUG
    printf_P(PSTR("onCommandReceived: %d, %d\n\r"), command, data);
  #endif
}

#define NAME  "iPower"

// Declare DHT11 sensor digital pin
#define DHT11PIN  3
// Declare state map keys
#define HUMIDITY  "humidity"
#define COMPUTER_TEMP  "comp. temp" // temperature inside

// Set up LED digital pins
Led led(5, 6); // (green, red) 

// Declare relays digital pins
#define RELAY1PIN  8
#define RELAY2PIN  7
// Declare state map keys
#define RELAY_1  "relay_1"
#define RELAY_2  "relay_2"

// Declare push button
OneButton button(4, true);

// Declare ACS712 sensor analog pin
#define ACS712PIN  A0
// Declare state map key
#define POWER  "power"
// Declare object
acs712 ACS712;

// Declare state map
struct comparator {
  bool operator()(const char* &str1, const char* &str2) {
    return strcmp(str1, str2) == 0;
  }
};
SimpleMap<const char*, int, 8, comparator> states;

// Declare delay managers in ms
timer_t timer(30000);

// Declare critical state
const int warm_temp = 40;
const int warm_humid = 80;
const int warm_power = 2000;

//
// Setup
//
void setup()
{
  // Configure output
  Serial.begin(9600);
  fdev_setup_stream(&serial_out, serial_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = stderr = &serial_out;

  #ifdef DEBUG
    printf_P(PSTR("Free memory: %d bytes.\n\r"), freeMemory());
  #endif
  // prevent continiously restart
  delay(500);
  // restart if memory lower 512 bytes
  softResetMem(512);
  // restart after freezing for 8 sec
  softResetTimeout();

  // initialize radio
  rf24init();

  // Configure button
  button.attachClick( buttonClick );
  button.attachLongPressStart( buttonLongPress );

  // initialize pin
  pinMode(ACS712PIN, INPUT);
  //Quiscent output voltage - the average voltage ACS712 shows with no load (0 A)
  ACS712.determineVQ(ACS712PIN);

  // always Power On after reset 
  // its temproary for hydroponics
  delay(1000);
  buttonLongPress();
}

//
// Loop
//
void loop()
{
  // watchdog
  heartbeat();
  // enable warning led if power on 
  if(states[RELAY_1] || states[RELAY_2])
    led.set(LED_RED);
  else
    led.set(LED_OFF);

  if(timer) {
    #ifdef DEBUG
      printf_P(PSTR("Free memory: %d bytes.\n\r"), freeMemory());
    #endif
    // reading sensors
    read_DHT11();

    states[POWER] = ACS712.readCurrent(ACS712PIN);
    #ifdef DEBUG
      printf_P(PSTR("ACS712: Info: Sensor value: power: %d.\n\r"), 
        states[POWER]);
    #endif

    // checking for critical state
    if(states[COMPUTER_TEMP]>=warm_temp ||
      states[HUMIDITY]>=warm_humid || 
  	  states[POWER]>=warm_power) 
    {      
      printf_P(PSTR("WARNING: Device sensors found critical value!"));
  	  printf_P(PSTR(" Device power will be shut down!\n\r"));
  	  // power off
      relayOff(RELAY_1);
      relayOff(RELAY_2);
      printf_P(PSTR("WARNING: Temperature: %d, Humidity: %d, Power: %d\n\r"),
      states[COMPUTER_TEMP], states[HUMIDITY], states[POWER]);   	  
      //led.set_blink(LED_RED, 100);
      // long sleep
      printf_P(PSTR("SLEEP: Info: Go to long sleep.\n\r"));
      Serial.flush();
      LowPower.powerDown(SLEEP_8S, 8, ADC_OFF, BOD_OFF); 
  	}

    // send data to base
    sendCommand( 1, (void*) &states, sizeof(states) );
    //sendCommand(1, (void*) &NAME, sizeof(NAME));
    //sendCommand(2, (void*) &states[COMPUTER_TEMP], sizeof(states[COMPUTER_TEMP]));
    //sendCommand(3, (void*) &states[HUMIDITY], sizeof(states[HUMIDITY]));
    //sendCommand(4, (void*) &states[POWER], sizeof(states[POWER]));
    //sendCommand(5, (void*) &states[RELAY_1], sizeof(states[RELAY_1]));
    //sendCommand(6, (void*) &states[RELAY_2], sizeof(states[RELAY_2]));
  }
  // update network
  rf24receive();

  // sleeping
  //#ifdef DEBUG
  //  printf_P(PSTR("SLEEP: Info: Go to Sleep.\n\r"));
  //#endif
  // Power down the radio.  Note that the radio will get powered back up
  // on the next write() call.
  //radio.powerDown();
  //Serial.flush();
  // Enter power down state for X*X sec with ADC and BOD module disabled
  //LowPower.powerDown(SLEEP_30MS, 1, ADC_OFF, BOD_OFF);
  //#ifdef DEBUG
  //  printf_P(PSTR("SLEEP: Info: WakeUp\n\r"));
  //#endif
  // update led
  led.update();
  // update push button
  button.tick();
}

/****************************************************************************/

void relayOn(const char* relay) {
  //if(states[relay]) {
    // relay is already on
  //  return;
  //}
  bool status = relays(relay, 0); // 0 is ON
  if(status) {
    #ifdef DEBUG
      printf_P(PSTR("RELAY: Info: '%s' is enabled.\n\r"), relay);
    #endif
    states[relay] = true;
  }
}

void relayOff(const char* relay) {
  if(states[relay] == false) {
    // relay is already off
    return;
  }
  bool status = relays(relay, 1); // 1 is OFF
  if(status) {
    #ifdef DEBUG
      printf_P(PSTR("RELAY: Info: '%s' is disabled.\n\r"), relay);
    #endif
    states[relay] = false;
  }
}

bool relays(const char* relay, uint8_t state) {
  if(strcmp(relay, RELAY_1) == 0) {
    pinMode(RELAY1PIN, OUTPUT);
    digitalWrite(RELAY1PIN, state);
    return true;
  } 
  if(strcmp(relay, RELAY_2) == 0) {
    pinMode(RELAY2PIN, OUTPUT);
    digitalWrite(RELAY2PIN, state);
    return true;
  } 
  printf_P(PSTR("RELAY: Error: '%s' is unknown!\n\r"), relay);
  return false;
}

/****************************************************************************/

void buttonClick() {
  #ifdef DEBUG
    printf_P(PSTR("BUTTON: Info: Short click.\n\r"));
  #endif
  relayOff(RELAY_1);
  relayOff(RELAY_2);
}

void buttonLongPress() {
  #ifdef DEBUG
    printf_P(PSTR("BUTTON: Info: Long press.\n\r"));
  #endif
  relayOn(RELAY_1);
  relayOn(RELAY_2);
  delay(500);
}

/****************************************************************************/

bool read_DHT11() {
  dht11 DHT11;
  if( DHT11.read(DHT11PIN) == DHTLIB_OK ) {
    states[HUMIDITY] = DHT11.humidity;
    states[COMPUTER_TEMP] = DHT11.temperature;
    #ifdef DEBUG
      printf_P(PSTR("DHT11: Info: Air humidity: %d, temperature: %dC.\n\r"), 
        states[HUMIDITY], states[COMPUTER_TEMP]);
    #endif
    return true;
  }
  printf_P(PSTR("DHT11: Error: Communication failed!\n\r"));
  return false;
}

/****************************************************************************/
