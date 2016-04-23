#include "SoftwareSerial.h"
#include <VC0706_UART.h>
#include <SD.h>
#include <SPI.h>
#define SS_SD  10

//use software serial
SoftwareSerial cameraconnection(2,3);//Rx, Tx
VC0706 cam = VC0706(&cameraconnection);
//use hardware serial
//VC0706 cam = VC0706(&Serial1);

void setup() 
{
    Serial.begin(9600);
    Serial.println("VC0706 Camera Snapshot Test ...");
    
    if (!SD.begin(SS_SD)) {
        Serial.println("SD Card init failed...");
        return;
    }  
    if(true == cameraInit()){
        snapShot();
    }else{
        Serial.println("camera init error...");
    }
}

void loop() 
{
    //nothing to do
}

bool cameraInit()
{
    cam.begin(BaudRate_19200);
    char *reply = cam.getVersion();
    if (reply == 0) {
        Serial.println("Failed to get version");
        return false;
    } else {
        Serial.println("version:");
        Serial.println("-----------------");
        Serial.println(reply);
        Serial.println("-----------------");
        return true;
    }
}

void snapShot()
{
    Serial.println("Snap in 3 secs...");
    delay(3000);
    if (! cam.takePicture()){ 
        Serial.println("Failed to snap!");
    }else { 
        Serial.println("Picture taken!");
    }
    // Create an image with the name IMAGExx.JPG
    char filename[13];
    strcpy(filename, "IMAGE00.JPG");
    for (int i = 0; i < 100; i++) {
        filename[5] = '0' + i/10;
        filename[6] = '0' + i%10;
        // create if does not exist, do not open existing, write, sync after write
        if (! SD.exists(filename)) {
            break;
        }
    }
    // Open the file for writing
    File imgFile = SD.open(filename, FILE_WRITE);
    Serial.print("create ");
    Serial.println(filename);
    uint16_t jpglen = cam.getFrameLength();
    Serial.print("wait to fetch ");
    Serial.print(jpglen, DEC);
    Serial.println(" byte image ...");
    int32_t time = millis();
    cam.getPicture(jpglen);
    uint8_t *buffer;
    while(jpglen != 0){
         uint8_t bytesToRead = min(32, jpglen);
         buffer = cam.readPicture(bytesToRead);     
         imgFile.write(buffer, bytesToRead);
         //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
         jpglen -= bytesToRead;   
    } 
    imgFile.close();
    time = millis() - time;
    Serial.println("Done!");
    Serial.print("Took "); Serial.print(time); Serial.println(" ms");
    cam.resumeVideo();    
}

