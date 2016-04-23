/*
 * maple-util.h
 *
 */

#ifndef MAPLE_UTIL_H_
#define MAPLE_UTIL_H_

#ifdef __cplusplus
extern "C"{
#endif

#define LED_PIN 13
#define LED_CONN_PIN 9

void serialUsbInit();
void serialUsbDisable(void);
void serialUsbWriteChar(uint8 ch);
void serialUsbWriteStr(const char *str);
void serialUsbWriteBuf(void *buf, uint32 size);
uint32 serialUsbReadBuf(void *buf, uint32 len);
uint8 serialUsbRead(void);
uint8 serialUsbIsConnected(void);
void serialUsbWriteStr(const char *str);
void serialUsbPrintNewline(void);
void serialUsbPrintChar(char c);
void serialUsbPrintln(const char c[]);
void serialUsbInput();
void serialUsbPrintlnWaitForInput(const char c[]);
void blinky();
void blinkyConn();
void serialUsbPrintHex(uint8 byte);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MAPLE_UTIL_H_ */
