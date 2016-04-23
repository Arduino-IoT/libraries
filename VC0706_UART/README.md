#Camera Shield VC0706
  
This is a library for the Adafruit TTL JPEG Camera (VC0706 chipset)

Pick one up today in the adafruit shop!
------> http://www.adafruit.com/products/397

These displays use Serial to communicate, 2 pins are required to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution


To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Adafruit_VC0706. Check that the Adafruit_VC0706 folder contains Adafruit_VC0706.cpp and Adafruit_VC0706.h

Place the Adafruit_VC0706 library folder your <arduinosketchfolder>/libraries/ folder. You may need to create the libraries subfolder if its your first library. Restart the IDE.

### Introduction
It is a first camera shield compatible with arduino which comes from Seeed, we use **VC0706 + OV7725** to achieve it. VC0706 is a high performance camera processor with enhanced image processing functions and it embedded the hard-wired JPEG codec. 

### Feature
+ Suppotr 640*480 pixel picture taken
+ High speed SPI/UART Interface
+ Images can saved to SD card directly

### Interface

***constructor***
    
    //we support both hardware serial and software serial
    #if USE_SOFTWARE_SERIAL
        VC0706(SoftwareSerial *serial){
    #else
	    VC0706(HardwareSerial *serial){
    #endif    

***Init the Camera Shield***

    void begin(VC0706_BaudRate baudRate);

***Take Picture***
    
    boolean takePicture(void);//snap
    void getPicture(uint16_t length);//send cmd to read picture 
	uint8_t* readPicture(uint8_t length);//get data
   

***Other Functions***

    char* getVersion(void);
    uint32_t getFrameLength(void);
    ……

### Getting Started
####use Hardware Serial or Software Serial for communication
we use Serial(Software Serial or Hardware serial) to communicate with Camera Shield, you can generate your applicance like below:
```c
//use software serial
SoftwareSerial cameraconnection(2,3);//Rx, Tx
VC0706 cam = VC0706(&cameraconnection);
```
or if you want to use the hardware serial, you can do it like below:
```c
//use hardware serial
VC0706 cam = VC0706(&Serial1);
```
we use the software serial by default. If hareware serial is your choise, you need to modify the code in VC0706_UART head file. Change
```c
#define USE_SOFTWARE_SERIAL			1
```
to 
```c
#define USE_SOFTWARE_SERIAL			0
```
####Use UART or SPI for data transmission
Usually, you can take a picture by the following steps:
```c
cam.takePicture();
uint16_t jpglen = cam.getFrameLength();
cam.getPicture(jpglen);
buffer = cam.readPicture(bytesToRead);   
```
and we allow you to read data via two mode: 
```
1. UART —— slower mode. Software Serial only support 9600/19200 baud rate. Hardware Serial can be 115200/38400/19200/9600. It will take about 30s to get a picture if you choose the uart mode and at 19200 baud rate. 
2. SPI —— faster mode. It will only take about 1s or 2s to get the whole picture back.
```
You can choose your mode in VC0706_UART header file. We use the SPI mode by default.
```c
#define TRANSFER_BY_SPI				1
``` 
change it to 
```c
#define TRANSFER_BY_SPI				0
```
to use the UART mode.<br><br>
Please take the example sketches in examples folder as reference, have fun!

----
This software is based on Adafruit's VC0706 Serial Camera library and modified by lawliet zou (![](http://www.seeedstudio.com/wiki/images/f/f8/Email-lawliet.zou.jpg)) for seeed studio.  
it is licensed under [The BSD License](http://www.freebsd.org/copyright/freebsd-license.html). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by [forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>

[![Analytics](https://ga-beacon.appspot.com/UA-46589105-3/Camera_Shield_VC0706)](https://github.com/igrigorik/ga-beacon)
