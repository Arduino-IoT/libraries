
unsigned char cmd_buf[10];
unsigned char i;

void ArduinoMP3Shield_SendCMD(unsigned char *cmd_buf, unsigned len)
{
    unsigned i;
    for(i=0; i<len; i++){
        Serial.write(cmd_buf[i]);
    }
}

void setup(void)
{
    /** wait until arduino mp3 shield get ready */
    delay(1000);

    Serial.begin(9600);
    
    /** set volume */
    cmd_buf[0] = 0x7E;          // START
    cmd_buf[1] = 0x03;          // Length
    cmd_buf[2] = 0xA7;          // Command
    cmd_buf[3] = 0x0F;          // new volume
    cmd_buf[4] = 0x7E;          // END
    ArduinoMP3Shield_SendCMD(cmd_buf, 5);
    
    /** set play mode repeat all */
    cmd_buf[0] = 0x7E;          // START
    cmd_buf[1] = 0x03;          // Length
    cmd_buf[2] = 0xA9;          // Command SET MODE
    cmd_buf[3] = 0x02;          // set mode
    cmd_buf[4] = 0x7E;          // END
    ArduinoMP3Shield_SendCMD(cmd_buf, 5);
    
    /** select SD card first music and play */
    cmd_buf[0] = 0x7E;          // START
    cmd_buf[1] = 0x04;          // Length
    cmd_buf[2] = 0xA0;          // Command
    cmd_buf[3] = 0x00;          // file number high byte
    cmd_buf[4] = 0x01;          // file number low byte
    cmd_buf[5] = 0x7E;          // END
    ArduinoMP3Shield_SendCMD(cmd_buf, 6);
}

void loop(void)
{
    
}

