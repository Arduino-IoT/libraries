/* 
 * maple-util.c
 * 
 * various utility routines useful for debugging
 *
 */
#include <string.h>

#include "libmaple_types.h"
#include "gpio.h"
#include "timers.h"
#include "usb.h"
#include "wirish.h"
#include "maple-util.h"

#define USB_TIMEOUT 50

const char *hex = "0123456789ABCDEF";

void serialUsbInit() {
    setupUSB();
}

void serialUsbDisable(void) {
    disableUSB();
}

void serialUsbWrite(uint8 ch) {
    if(!(usbIsConnected() && usbIsConfigured())) {
        return;
    }
    uint16 status = 0;
    uint32 start = millis();
    while(status == 0 && (millis() - start <= USB_TIMEOUT)) {
        status = usbSendBytes(&ch, 1);
    }
}

void serialUsbWriteStr(const char *str) {
    if(!(usbIsConnected() && usbIsConfigured())) {
        return;
    }
    uint32 len = strlen(str);
    uint16 status = 0;
    uint16 oldstatus = 0;
    uint32 start = millis();
    while(status < len && (millis() - start < USB_TIMEOUT)) {
        status += usbSendBytes((uint8*)str+status, len-status);
        if(oldstatus != status) 
            start = millis();
        oldstatus = status;
    }
}

void serialUsbWriteBuf(void *buf, uint32 size) {
    if(!(usbIsConnected() && usbIsConfigured())) {
        return;
    }
    if (!buf) {
        return;
    }
    uint16 status = 0;
    uint16 oldstatus = 0;
    uint32 start = millis();
    while(status < size && (millis() - start < USB_TIMEOUT)) {
        status += usbSendBytes((uint8*)buf+status, size-status);
        if(oldstatus != status) 
            start = millis();
        oldstatus = status;
    }
}

uint32 serialUsbReadBuf(void *buf, uint32 len) {
   if (!buf) {
      return 0;
   }
   return usbReceiveBytes((uint8*)buf, len);
}

uint8 serialUsbRead(void) {
   uint8 ch;
   usbReceiveBytes(&ch, 1);
   return ch;
}

uint8 serialUsbIsConnected(void) {
   return (usbIsConnected() && usbIsConfigured());
}

void serialUsbPrintNewline(void) {
  serialUsbWrite('\r');
  serialUsbWrite('\n');
}

void serialUsbPrintln(const char c[]) {
  serialUsbWriteStr(c);
  serialUsbPrintNewline();
}

void serialUsbPrintlnChar(char c) {
  serialUsbWrite(c);
  serialUsbPrintNewline();
}

char input;
int toggle = 0;

void serialUsbInput() {
    serialUsbPrintln("(waiting for input...)");
    toggle = 0;
    while(!usbBytesAvailable()) {
      toggle ^= 1;
      digitalWrite(LED_PIN, toggle);
      delay(100);
    }
    input = serialUsbRead();
    serialUsbPrintlnChar(input);
    toggle = 0;
    digitalWrite(LED_PIN, toggle);
}

void serialUsbPrintlnWaitForInput(const char c[]) {
   serialUsbPrintln(c);
   serialUsbInput();
}

void blinky() {
  pinMode(LED_PIN, OUTPUT);     
  digitalWrite(LED_PIN, HIGH);   // set the LED on
  delay(500);               // wait for a second
  digitalWrite(LED_PIN, LOW);    
  delay(500);               
  digitalWrite(LED_PIN, HIGH);   
  delay(500);               
  digitalWrite(LED_PIN, LOW);    
  delay(500);               
  digitalWrite(LED_PIN, HIGH);   
  delay(500);               
  digitalWrite(LED_PIN, LOW);    
  delay(500);               
}

void blinkyConn() {
  pinMode(LED_CONN_PIN, OUTPUT);     
  digitalWrite(LED_CONN_PIN, HIGH);   // set the LED on
  delay(500);               // wait for a second
  digitalWrite(LED_CONN_PIN, LOW);    
  delay(500);               
  digitalWrite(LED_CONN_PIN, HIGH);   
  delay(500);               
  digitalWrite(LED_CONN_PIN, LOW);    
  delay(500);               
  digitalWrite(LED_CONN_PIN, HIGH);   
  delay(500);               
  digitalWrite(LED_CONN_PIN, LOW);    
  delay(500);               
}

void serialUsbPrintHex(uint8 byte) {
  char upper, lower;
  upper = byte >> 4;
  lower = (byte &= 0x0F);
  serialUsbWrite(hex[upper]);
  serialUsbWrite(hex[lower]);
}
