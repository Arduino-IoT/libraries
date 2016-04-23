#ifndef pieceduino_h
#define pieceduino_h

#include "Arduino.h"

#define MAX_MESSAGE_SIZE  100
#define DEBUG_BUF_LEN 128


// WebSocket protocol constants
// First byte.
#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a
// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127

class pieceduino
{
public:
    pieceduino(HardwareSerial &uart, uint32_t baud = 115200);
    bool begin();
    bool reset();
    bool setWifiMode(int pattern);
    bool setAP(String ssid, String pwd , uint8_t channel, uint8_t ecn);
    bool setSleep(uint8_t mode);
    bool setDeepSleep(uint8_t time);
    bool connToWifi(String ssid, String pwd);
    String getVersion();
    String getIP();
    String sensingAP(String ssid);
    bool ProcessReceivedCharacter();
    bool Throw(String key, float value);
    
    bool createTCPServer(uint32_t port = 3333);
    bool createTCP(String addr, uint32_t port);
    bool createUDP(String addr, uint32_t port);
    bool enableMUX();
    bool disableMUX();
    bool smartLink();
    bool connected();
    uint32_t recv();
    void Send(uint8_t mux_id,String str, byte len) ;
    void Send(String str, byte len);
    void WebSocketConnect(String token);
    byte cipmux;
    char          MessageBuffer[MAX_MESSAGE_SIZE + 1];
    uint8_t client_id;
    
    void setCallback(void (*WebSocketInCallback)(char key, float value));
    void SaveDataToCloud(String key, String data);
    
    String WEBSOCKET_SERVER;
    String WEBSOCKET_PORT;
    String WEBSOCKET_PATH;
    
  private:
    
    //byte StringLength(char *str);
    //int FindEspRecv(char *str);
    int FindEspRecv(String str);
    //int FindEspRecv(char *str,String &recv);
    int FindEspRecv(String str,String &recv);
    bool StringFilter(String begin, String end, String &data);
    
    void ProcessMessage(char *str, byte len);
    void parseWebsocket(String frame);
    void parseSocketIo(String frame);
    void parseData(String frame);
    void WebSocketSendText(String str);
    
    
    byte          errorCheck;
    String pieceduino_cloud_token;
    unsigned long pingtimer;
    unsigned long connectedtimer;
    char          MessageCheckBuf[15];
    byte          MessageReceivingMode;
    unsigned int  MessageSize;
    unsigned int  MessageSize_return;
    byte          MessageCursor;
    bool          bConnected;
    bool          bIPD;
    
    bool bWebSocketConnect;
    
    HardwareSerial *m_puart;
    
    void (*_WebSocketInCallback)(char key, float value);
    void rx_empty();
   
};

#endif
