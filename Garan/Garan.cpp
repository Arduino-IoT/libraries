#include "Garan.h"

#include <string.h>

#include <Arduino.h>
#include <SoftwareSerial.h>

// example or fixed commands
const uint8_t SINGLE_PLAY[]        = {0x04, 0x01, 0x00, 0x00, 0x01};  // NH NL
const uint8_t SEQUENCE_PLAY[]      = {0x04, 0x02, 0x00, 0x00, 0x02};  // NH NL
const uint8_t SINGLE_LOOP_PLAY[]   = {0x04, 0x03, 0x00, 0x00, 0x02};  // NH NL
const uint8_t SINGLE_PLAY_NAME[]   = {0x07, 0x04, 0x00, 0x31, 0x2E, 0x4D, 0x50, 0x33};  // Name must <= 12
const uint8_t SEQUENCE_PLAY_NAME[] = {0x07, 0x05, 0x00, 0x31, 0x2E, 0x4D, 0x50, 0x33};  // Name must <= 12
const uint8_t SINGLE_LOOP_NAME[]   = {0x07, 0x06, 0x00, 0x31, 0x2E, 0x4D, 0x50, 0x33};  // Name must <= 12
const uint8_t STOP[]               = {0x02, 0x07, 0x00};
const uint8_t PAUSE_PLAY[]         = {0x02, 0x08, 0x00};
const uint8_t NEXT[]               = {0x02, 0x09, 0x00};
const uint8_t PREV[]               = {0x02, 0x0A, 0x00};
const uint8_t VOLUME_UP[]          = {0x02, 0x0B, 0x00};
const uint8_t VOLUME_DOWN[]        = {0x02, 0x0C, 0x00};
const uint8_t SET_VOLUME[]         = {0x03, 0x0D, 0x00, 0x05};  // Value
const uint8_t EQ_CHANGE[]          = {0x02, 0x0E, 0x00};
const uint8_t SET_EQ[]             = {0x03, 0x0F, 0x00, 0x01};  // Value
const uint8_t STANDBY_MODE[]       = {0x02, 0x10, 0x00};
const uint8_t SET_TIME[]           = {0x09, 0x11, 0x00, 0x07, 0xDD, 0x06, 0x0D, 0x0B, 0x11, 0x01};
const uint8_t GET_PLAYING_NAME[]   = {0x02, 0x21, 0x01};
const uint8_t GET_MUSIC_NUMBERS[]  = {0x02, 0x22, 0x01};
const uint8_t GET_PLAYING_ORDER[]  = {0x02, 0x23, 0x01};
const uint8_t GET_TIME[]           = {0x02, 0x25, 0x01};
const uint8_t FEEDBACK_AT_END[]    = {0x03, 0xA0, 0x01, 0x01};
const uint8_t NO_FEEDBACK_AT_END[] = {0x03, 0xA0, 0x01, 0x00};
const uint8_t GET_VERSION[]        = {0x02, 0x80, 0x01};

Garan::Garan(SoftwareSerial &theSerial) : _serial(theSerial)
{
    _serial.begin(9600);

    _commandBuff[2] = 1;
}

bool Garan::available() {
    return _serial.available();
}

void Garan::sendCommand(uint8_t command[])  {
    uint8_t checksum = 0;

    _serial.write(0x24);
    for (uint16_t i = 0; i <= command[0]; i++) {
        _serial.write(command[i]);
        checksum ^= command[i];
    }

    _serial.write(checksum);
}

inline void Garan::buildHead(uint8_t len, uint8_t cmd)  {
    _commandBuff[0] = len;
    _commandBuff[1] = cmd;
}

void Garan::singlePlay(uint16_t number)  {
    buildHead(0x04, 0x01);
    _commandBuff[3] = (uint8_t)(number >> 8);
    _commandBuff[4] = (uint8_t)number;

    sendCommand(_commandBuff);
}

void Garan::sequencePlay(uint16_t number)  {
    buildHead(0x04, 0x02);
    _commandBuff[3] = (uint8_t)(number >> 8);
    _commandBuff[4] = (uint8_t)number;

    sendCommand(_commandBuff);
}

void Garan::singleLoopPlay(uint16_t number)  {
    buildHead(0x04, 0x02);
    _commandBuff[3] = (uint8_t)(number >> 8);
    _commandBuff[4] = (uint8_t)number;

    sendCommand(_commandBuff);
}

void Garan::singlePlayName(char *name)  {
    strcpy((char *)&_commandBuff[3], name);
    buildHead(strlen(name) + 2, 4);

    sendCommand(_commandBuff);
    _serial.begin(9600);
}

void Garan::sequencePlayName(char *name)  {
    strcpy((char *)&_commandBuff[3], name);
    buildHead(strlen(name) + 2, 5);

    sendCommand(_commandBuff);
    _serial.begin(9600);
}

void Garan::singleLoopName(char *name)  {
    strcpy((char *)&_commandBuff[3], name);
    buildHead(strlen(name) + 2, 6);

    sendCommand(_commandBuff);
    _serial.begin(9600);
}

void Garan::stop()  {
    sendCommand((uint8_t *)STOP);
}

void Garan::pausePlay()  {
    sendCommand((uint8_t *)PAUSE_PLAY);
}

void Garan::next()  {
    sendCommand((uint8_t *)NEXT);
}

void Garan::prev()  {
    sendCommand((uint8_t *)PREV);
}

void Garan::volumeUp()  {
    sendCommand((uint8_t *)VOLUME_UP);
}

void Garan::volumeDown()  {
    sendCommand((uint8_t *)VOLUME_DOWN);
}

void Garan::setVolume(uint8_t volume)  {
    buildHead(0x03, 0x0D);
    _commandBuff[3] = volume;

    sendCommand(_commandBuff);
}

void Garan::eqChange()  {
    sendCommand((uint8_t *)EQ_CHANGE);
}

void Garan::setEQ(uint8_t eq)  {
    buildHead(0x03, 0x0F);
    _commandBuff[3] = eq;

    sendCommand(_commandBuff);
}

void Garan::standbyMode()  {
    sendCommand((uint8_t *)STANDBY_MODE);
}

void Garan::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)  {
    buildHead(0x09, 0x11);
    _commandBuff[3] = (uint8_t)(year >> 8);
    _commandBuff[4] = (uint8_t)year;
    _commandBuff[5] = month;
    _commandBuff[6] = day;
    _commandBuff[7] = hour;
    _commandBuff[8] = minute;
    _commandBuff[9] = second;

    sendCommand(_commandBuff);
}

void Garan::getPlayingName()  {
    sendCommand((uint8_t *)GET_PLAYING_NAME);
}

void Garan::getMusicNumbers()  {
    sendCommand((uint8_t *)GET_MUSIC_NUMBERS);
}

void Garan::getPlayingOrder()  {
    sendCommand((uint8_t *)GET_PLAYING_ORDER);
}

void Garan::getTime()  {
    sendCommand((uint8_t *)GET_TIME);
}

void Garan::feedbackAtEnd()  {
    sendCommand((uint8_t *)FEEDBACK_AT_END);
}

void Garan::NoFeedbackAtEnd()  {
    sendCommand((uint8_t *)NO_FEEDBACK_AT_END);
}

void Garan::getVersion()  {
    sendCommand((uint8_t *)GET_VERSION);
}
