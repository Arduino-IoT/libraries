/*
  Garan.h - Library for controlling Garan Audio Module
  Released into the public domain.
*/
#ifndef Garan_h
#define Garan_h

#include <SoftwareSerial.h>

class Garan
{
    private:
        SoftwareSerial &_serial;
        unsigned char _commandBuff[13];
        void sendCommand(uint8_t command[]);
        inline void buildHead(uint8_t len, uint8_t cmd);
    public:
        Garan(SoftwareSerial &theSerial);
        bool available();
        void singlePlay(uint16_t number);
        void sequencePlay(uint16_t number);
        void singleLoopPlay(uint16_t number);
        void singlePlayName(char *name);
        void sequencePlayName(char *name);
        void singleLoopName(char *name);
        void stop();
        void pausePlay();
        void next();
        void prev();
        void volumeUp();
        void volumeDown();
        void setVolume(uint8_t volume);
        void eqChange();
        void setEQ(uint8_t eq);
        void standbyMode();
        void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
        void getPlayingName();
        void getMusicNumbers();
        void getPlayingOrder();
        void getTime();
        void feedbackAtEnd();
        void NoFeedbackAtEnd();
        void getVersion();
};

#endif  // Garan_h