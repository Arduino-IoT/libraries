/*
 * Based on Obdev's AVRUSB code and under the same license.
 */
#ifndef __UsbKeypad_h__
#define __UsbKeypad_h__

#include "WProgram.h"
#include "usbdrv.h"
#include "hid_keys.h"

#define BUFFER_SIZE 4       // Minimum of 2: 1 for modifiers + 1 for keystroke 

static uchar    idleRate;           // in 4 ms units 

/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and but we do allow
 * simultaneous key presses. 
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */
PROGMEM char usbHidReportDescriptor[35] = { /* USB report descriptor */
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop) 
  0x09, 0x06,                    // USAGE (Keyboard) 
  0xa1, 0x01,                    // COLLECTION (Application) 
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard) 
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl) 
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI) 
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0) 
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1) 
  0x75, 0x01,                    //   REPORT_SIZE (1) 
  0x95, 0x08,                    //   REPORT_COUNT (8) 
  0x81, 0x02,                    //   INPUT (Data,Var,Abs) 
  0x95, BUFFER_SIZE-1,           //   REPORT_COUNT (simultaneous keystrokes) 
  0x75, 0x08,                    //   REPORT_SIZE (8) 
  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101) 
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated)) 
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application) 
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs) 
  0xc0                           // END_COLLECTION 
};


class UsbKeypadDevice {
 public:
  UsbKeypadDevice () {
    usbInit();
  }
    
  void update() {
    usbPoll();
  }
    
  void sendKeyStroke(byte keyStroke) {
    sendKeyStroke(keyStroke, 0);
  }

  void sendKeyStroke(byte keyStroke, byte modifiers) {
      
    while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke
      //       so we know the previous keystroke was
      //       sent.
    }
      
    memset(reportBuffer, 0, sizeof(reportBuffer));

    reportBuffer[0] = modifiers;
    reportBuffer[1] = keyStroke;
        
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

    while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke
      //       so we know the previous keystroke was
      //       sent.
    }
      
    // This stops endlessly repeating keystrokes:
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
      
  uchar    reportBuffer[4];    // buffer for HID reports [ 1 modifier byte + (len-1) key strokes]

};

UsbKeypadDevice UsbKeypad = UsbKeypadDevice();

// USB_PUBLIC uchar usbFunctionSetup
uchar usbFunctionSetup(uchar data[8]) 
{
    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    /* The following requests are never used. But since they are required by
    * the specification, we implement them in this example.
    */
    usbMsgPtr = UsbKeypad.reportBuffer; //
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
      if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
          /* we only have one report type, so don't look at wValue */
          UsbKeypad.reportBuffer[0] = 0; // nothing pressed
          UsbKeypad.reportBuffer[1] = 0;
          return sizeof(UsbKeypad.reportBuffer);    
      }
    } else {
      /* no vendor specific requests implemented */
    }
    return 0;
}


#endif // __UsbKeypad_h__
