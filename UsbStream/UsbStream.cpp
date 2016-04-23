/* Name: main.c
 * Based on project: hid-data, example how to use HID for data transfer
 * (Uses modified descriptor and usbFunctionSetup from it.)
 * Original author: Christian Starkjohann
 * Arduino modifications by: Philip J. Lindsay
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: main.c 692 2008-11-07 15:07:40Z cs $
 */

/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */

#include "UsbStream.h"

// Ring buffer implementation nicked from HardwareSerial.cpp
// TODO: Don't nick it. :)
ring_buffer rx_buffer = { { 0 }, 0, 0 };
ring_buffer tx_buffer = { { 0 }, 0, 0 };


inline void store_char(unsigned char c, ring_buffer *the_buffer)
{
  int i = (the_buffer->head + 1) % RING_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != the_buffer->tail) {
    the_buffer->buffer[the_buffer->head] = c;
    the_buffer->head = i;
  }
}
 
UsbStreamDevice::UsbStreamDevice(ring_buffer *rx_buffer,
				 ring_buffer *tx_buffer) {
  _rx_buffer = rx_buffer;
  _tx_buffer = tx_buffer;
}

void UsbStreamDevice::begin() {
    // disable timer 0 overflow interrupt (used for millis)
    TIMSK0&=!(1<<TOIE0);

    cli();

    usbInit();
      
    usbDeviceDisconnect();
    uchar   i;
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        _delay_ms(1);
    }
    usbDeviceConnect();

    sei();
  }
    
// TODO: Deprecate update
void UsbStreamDevice::update() {
  refresh();
}

void UsbStreamDevice::refresh() {
  usbPoll();
}

int UsbStreamDevice::available() {
  /*
   */
  return (RING_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % RING_BUFFER_SIZE;
}
  
int UsbStreamDevice::read() {
  /*
   */
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer->head == _rx_buffer->tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
    _rx_buffer->tail = (_rx_buffer->tail + 1) % RING_BUFFER_SIZE;
    return c;
  }
}

void UsbStreamDevice::write(byte c) {
  /*
   */
  store_char(c, _tx_buffer);
}

// TODO: Handle this better?
int tx_available() {
  /*
   */
  return (RING_BUFFER_SIZE + tx_buffer.head - tx_buffer.tail) % RING_BUFFER_SIZE;
}

int tx_read() {
  /*
   */
  // if the head isn't ahead of the tail, we don't have any characters
  if (tx_buffer.head == tx_buffer.tail) {
    return -1;
  } else {
    unsigned char c = tx_buffer.buffer[tx_buffer.tail];
    tx_buffer.tail = (tx_buffer.tail + 1) % RING_BUFFER_SIZE;
    return c;
  } 
}




/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"{
#endif 
PROGMEM char usbHidReportDescriptor[22] = {    /* USB report descriptor */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};
/* Since we define only one feature report, we don't use report-IDs (which
 * would be the first byte of the report). The entire report consists of 1
 * opaque data bytes.
 */

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
  usbRequest_t    *rq = (usbRequest_t*)((void *)data);

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID class request */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* since we have only one report type, we can ignore the report-ID */
	    static uchar dataBuffer[1];  /* buffer must stay valid when usbFunctionSetup returns */
	    if (tx_available()) {
	      dataBuffer[0] = tx_read();
	      usbMsgPtr = dataBuffer; /* tell the driver which data to return */
	      return 1; /* tell the driver to send 1 byte */
	    } else {
	      // Drop through to return 0 (which will stall the request?)
	    }
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            /* since we have only one report type, we can ignore the report-ID */

	  // TODO: Check race issues?
	  store_char(rq->wIndex.bytes[0], &rx_buffer);

        }
    }else{
        /* ignore vendor type requests, we don't use any */
    }
    return 0;
}
#ifdef __cplusplus
} // extern "C"
#endif

UsbStreamDevice UsbStream = UsbStreamDevice(&rx_buffer, &tx_buffer);


