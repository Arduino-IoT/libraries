#include "Arduino.h"
#include "pieceduino.h"

#define DEBUG_MODE 1
#define SOCKETIO_HEARTBEAT     55000
#define CONNECTED_TIMER     10000
#define ESP8266_TIMEOUT   10000

//
pieceduino::pieceduino(HardwareSerial &uart, uint32_t baud): m_puart(&uart)
{
    m_puart->begin(baud);
    rx_empty();
    
    errorCheck             = 0; //錯誤旗標.
    pingtimer            = 0; //heartbeat時間
    MessageReceivingMode = 0; //模式設定
    MessageSize          = 0; //ESP8266傳來的訊息大小
    MessageCursor        = 0;  //
    bWebSocketConnect    = false; //檢測是否現在為使用websocket
    bConnected = false;
    
    WEBSOCKET_SERVER    =     "www.pieceduino.com";
    WEBSOCKET_PORT      =     "8001";
    WEBSOCKET_PATH      =     "/socket.io/?transport=websocket";
}

//
uint32_t pieceduino::recv(){
    while (m_puart->available()) {
        if(ProcessReceivedCharacter()==true){
            return MessageSize_return;
        }
    }
    if(bWebSocketConnect == true){
        if (millis() > pingtimer + SOCKETIO_HEARTBEAT) {
            #if DEBUG_MODE
            Serial.print(F("handshaking..."));
            #endif
            MessageReceivingMode = 0;
            MessageSize = 0;
            MessageCursor = 0;
            WebSocketSendText("2");
            pingtimer = millis();
        }
    
        if (errorCheck == 1) {
            #if DEBUG_MODE
            Serial.println(F("errorCheck == 1"));
            #endif
            WebSocketConnect(pieceduino_cloud_token);  //recconect
            pingtimer = millis();
            errorCheck = 0;
        }
    }
    return 0;
}

//
void pieceduino::SaveDataToCloud(String key, String data){
    String frame;
    
    
    frame ="42[\"s_Data\",\"";
    frame +="{\\\"from\\\":\\\"Pieceduino\\\",\\\"";
    frame +=key;
    frame +="\\\":\\\"";
    frame +=data;
    frame +="\\\"}";
    frame +="\"]";
    
    Serial.println(frame);
    
    WebSocketSendText(frame);
}

//
bool pieceduino::StringFilter(String begin, String end, String &data)
{
    String data_tmp;
    data_tmp = data;

    int32_t index1 = data_tmp.indexOf(begin);
    int32_t index2 = data_tmp.indexOf(end);
    if (index1 != -1 && index2 != -1) {
        index1 += begin.length();
        data = data_tmp.substring(index1, index2);
        return true;
    }

    data = data_tmp;
    return false;
}

//
String pieceduino::getVersion(){
    String Version;
    m_puart->println(F("AT+GMR"));
    if(FindEspRecv("OK\r\n",Version)){
        StringFilter("\r\r\n", "\r\n\r\nOK", Version);
        return Version;
    }else{
        return "error";
    }
}

//
String pieceduino::getIP(){
    String ip;
    m_puart->println(F("AT+CIFSR"));
    if(FindEspRecv("OK",ip)){
        StringFilter("STAIP,", "+CIFSR:STAMAC", ip);
        return ip;
    }else{
        return "error";
    }
    //
}

//
String pieceduino::sensingAP(String ssid){
    String detail;
    m_puart->print(F("AT+CWLAP=\""));
    m_puart->print(ssid);
    m_puart->println(F("\""));
    if(FindEspRecv("OK",detail)){
        StringFilter("(", ")", detail);
        return detail;
    }else{
        return "error";
    }
    //

}

//
bool pieceduino::begin(){
    String Version;
    #if DEBUG_MODE
    Serial.print(F("testing ESP8266..."));
    #endif
    m_puart->println(F("AT"));
    if(FindEspRecv("OK\r\n")){
    #if DEBUG_MODE
        Serial.println(F("[ok]"));
    #endif
        return true;
    }else{
    #if DEBUG_MODE
        Serial.println(F("[fatal error: check baud rate]"));
    #endif
        errorCheck = 1;
        return false;
    }
}

//
bool pieceduino::reset(){
    #if DEBUG_MODE
    Serial.print(F("reset ESP8266..."));
    #endif
    m_puart->println(F("\r\nAT+RST"));
    if(FindEspRecv("ready\r\n")){
    #if DEBUG_MODE
        Serial.println("[ok]");
    #endif
        return true;
    }else{
    #if DEBUG_MODE
        Serial.println(F("[error: can't reset ESP8266, check baud rate, reset arduino]"));
    #endif
        errorCheck = 1;
        return false;
    }
}

//
bool pieceduino::setWifiMode(int pattern){
    #if DEBUG_MODE
    Serial.print(F("setting ESP8266 mode..."));
    #endif
    
    switch(pattern){
        case 1:
            m_puart->println(F("AT+CWMODE=1"));//STA
        break;
        case 2:
            m_puart->println(F("AT+CWMODE=2"));//AP
        break;
        case 3:
            m_puart->println(F("AT+CWMODE=3"));//Both
        break;
    }
    
    if(FindEspRecv("OK\r\n")){
        
    #if DEBUG_MODE
        switch(pattern){
        case 1:
            Serial.println(F("[ok: station mode]"));
            break;
        case 2:
            Serial.println(F("[ok: ap mode]"));
            break;
        case 3:
            Serial.println(F("[ok: both mode]"));
            break;
        }
    #endif
        
        return true;
    }else{
        
    #if DEBUG_MODE
        Serial.println(F("[error: not station mode]"));
    #endif
        
        return false;
    }
}

//
bool pieceduino::connToWifi(String ssid, String pwd){
    #if DEBUG_MODE
    Serial.print(F("connecting to WiFi..."));
    #endif
    
    m_puart->print(F("AT+CWJAP=\""));
    m_puart->print(ssid);
    m_puart->print(F("\",\""));
    m_puart->print(pwd);
    m_puart->println(F("\""));
    
    if(FindEspRecv("OK\r\n")){
        
    #if DEBUG_MODE
        Serial.println("[ok]");
    #endif
        
        return true;
    }else{
        
    #if DEBUG_MODE
        Serial.println(F("[error: can't connect WiFi]"));
    #endif
        
        errorCheck = 1;
        return false;
    }
}

//
bool pieceduino::connected(){
    if (millis() > connectedtimer + CONNECTED_TIMER) {
        m_puart->println("AT+CIPSTATUS");
        connectedtimer = millis();
    }
    return bConnected;
}

//
bool pieceduino::createTCPServer(uint32_t port){
    #if DEBUG_MODE
    Serial.print(F("create TCP Server..."));
    #endif
    
    m_puart->print("AT+CIPSERVER=1,");
    m_puart->println(port);
    
    if(FindEspRecv("OK\r\n")){
        
    #if DEBUG_MODE
        Serial.println(F("[ok: TCP Server created]"));
    #endif
        
        return true;
    }else{
        
    #if DEBUG_MODE
        Serial.println(F("[error: can't create TCP Server]"));
    #endif
        
        return false;
    }

}

bool pieceduino::createTCP(String addr, uint32_t port)
{
    String data;
 
    m_puart->print("AT+CIPSTART=\"TCP");
    m_puart->print("\",\"");
    m_puart->print(addr);
    m_puart->print("\",");
    m_puart->println(port);
    
    if(FindEspRecv("ERROR\r\n")){
    #if DEBUG_MODE
        Serial.println(F("[error: can't create TCP]"));
    #endif
        
        return false;
    }else if(FindEspRecv("OK\r\n")){
        
    #if DEBUG_MODE
        Serial.println(F("[ok: TCP created]"));
    #endif
        bConnected = true;
        return true;
    }else{
        
    #if DEBUG_MODE
        Serial.println(F("[error: can't create TCP]"));
    #endif
        
        return false;
    }
}

bool pieceduino::createUDP(String addr, uint32_t port)
{
    String data;
    
    m_puart->print("AT+CIPSTART=\"UDP");
    m_puart->print("\",\"");
    m_puart->print(addr);
    m_puart->print("\",");
    m_puart->print(port);
    m_puart->print("[,(");
    m_puart->print(port);
    m_puart->println("),(0)");
    
    if(FindEspRecv("ERROR\r\n")){
#if DEBUG_MODE
        Serial.println(F("[error: can't create UDP]"));
#endif
        
        return false;
    }else if(FindEspRecv("OK\r\n")){
        
#if DEBUG_MODE
        Serial.println(F("[ok: UDP created]"));
#endif
        bConnected = true;
        return true;
    }else{
        
#if DEBUG_MODE
        Serial.println(F("[error: can't create UDP]"));
#endif
        
        return false;
    }
}

//
bool pieceduino::smartLink(){
    m_puart->print("AT+CWSTARTSMART=0");
    
    if(FindEspRecv("OK\r\n")){
        
    #if DEBUG_MODE
        Serial.println(F("[ok: smartLink]"));
    #endif
        
        return true;
    }else{
        
    #if DEBUG_MODE
        Serial.println(F("[error: can't smartLink]"));
    #endif
        
        return false;
    }
}

//
bool pieceduino::disableMUX(){
#if DEBUG_MODE
    Serial.print(F("disable MUX..."));
#endif
    
    m_puart->print("AT+CIPMUX=");
    m_puart->println(0);
    
    if(FindEspRecv("OK\r\n")){
        
#if DEBUG_MODE
        Serial.println(F("[ok: single]"));
#endif
        
        cipmux = 0;
        return true;
    }else{
        
#if DEBUG_MODE
        Serial.println(F("[error: can't single]"));
#endif
        
        return false;
    }
}

//
bool pieceduino::enableMUX(){
    #if DEBUG_MODE
    Serial.print(F("enable MUX..."));
    #endif
    
    m_puart->print("AT+CIPMUX=");
    m_puart->println(1);
    
    if(FindEspRecv("OK\r\n")){
        
    #if DEBUG_MODE
        Serial.println(F("[ok: multiple]"));
    #endif
        
        cipmux = 1;
        return true;
    }else{
        
    #if DEBUG_MODE
        Serial.println(F("[error: can't multiple]"));
    #endif
        
        return false;
    }
}

//
/*
 參數說明
 <sleep mode>
 0 關閉休眠模式
 1 light-sleep 模式
 2 modem-sleep 模式
 */
/*
1： Modem-Sleep 用于需要 CPU 一直 处于工作状态 如 PWM 或 I2S 应用等。在保持 WiFi 连接时，如果没有数据传输，可根据 802.11 标准（如 U-APSD），关闭 WiFi Modem 电路来省电。例如，在 DTIM3时，每 sleep 300mS，醒来 3mS 接收 AP 的 Beacon 包等，则整体平均电流约 15mA。

2： Light-Sleep 用于 CPU 可暂停的应用，如 WiFi 开关。在保持 WiFi 连接时，如果没有数据传输，可根据 802.11 标准（如 U-APSD），关闭 WiFi Modem 电路并 暂停 CPU 来省电。例如，在 DTIM3 时，每 sleep 300mS，醒来 3mS 接收 AP 的 Beacon 包等，则整体平均电流约 0.9mA。

3： Deep-Sleep 不需一直保持 WiFi 连接，很长时间才发送一次 数据包的 应用，如每 100 秒测量一次温度的传感器。例如，每 300S 醒来后需 0.3~1s 连上 AP 发送数据,则整体平均电流可远小于 1mA。
 */
bool pieceduino::setSleep(uint8_t mode){
    m_puart->print("AT+SLEEP=");
    m_puart->println(mode);
    
    if(FindEspRecv("OK")){
#if DEBUG_MODE
        Serial.print(F("[ok: Sleep Setting]"));
#endif
        return true;
    }else{
        return false;
    }
    return false;
}

//深度睡眠
/*
 參數說明
 <sleep time>
 單位:豪妙
 ESP8266會在設定時間後自動喚醒
 */
bool pieceduino::setDeepSleep(uint8_t time){
    m_puart->print("AT+GSLP=");
    m_puart->println(time);
    
    if(FindEspRecv("OK")){
#if DEBUG_MODE
        Serial.print(F("[ok: DeepSleep Setting]"));
#endif
        return true;
    }else{
        return false;
    }
    return false;
}

//
bool pieceduino::setAP(String ssid, String pwd , uint8_t channel, uint8_t ecn){
    String cmd;
    
#if DEBUG_MODE
    Serial.print(F("Setting AP Mode Detail..."));
#endif
    m_puart->print(F("AT+CWSAP_DEF=\""));
    m_puart->print(ssid);
    m_puart->print(F("\",\""));
    m_puart->print(pwd);
    m_puart->print(F("\","));
    m_puart->print(channel);
    m_puart->print(F(","));
    m_puart->println(ecn);
    
    if(FindEspRecv("OK")){
#if DEBUG_MODE
        Serial.println(F("[ok: AP Setting]"));
#endif
        return true;
    }else{
        return false;
    }
    return false;
}

//
void pieceduino::WebSocketConnect(String token){
    
    pieceduino_cloud_token = token;
    
    #if DEBUG_MODE
    Serial.print(F("opening TCP connection..."));
    #endif
    
    m_puart->print(F("AT+CIPSTART=\"TCP\",\""));
    m_puart->print(WEBSOCKET_SERVER);
    m_puart->print(F("\","));
    m_puart->println(WEBSOCKET_PORT);
    
    if(FindEspRecv("OK\r\n")){
    #if DEBUG_MODE
        Serial.println("[ok]");
    #endif
    }else{
    #if DEBUG_MODE
        Serial.println(F("[error: can't open TCP connection]"));
    #endif
        errorCheck = 1;
        return;
    }
    
    char key_base64[] = "B3aOqgUw9RtO7WoAAxIupQ==";
    #if DEBUG_MODE
    Serial.print(F("opening Websocket connection..."));
    #endif
    m_puart->print(F("AT+CIPSEND="));
    /*
    m_puart->println(StringLength(WEBSOCKET_PATH)+pieceduino_cloud_token.length()+StringLength(WEBSOCKET_SERVER) + StringLength(WEBSOCKET_PORT) + 139+28);
     */
    
    m_puart->println(WEBSOCKET_PATH.length()+pieceduino_cloud_token.length()+WEBSOCKET_SERVER.length() + WEBSOCKET_PORT.length() + 139+28);
    
    if(FindEspRecv("> ")){
    }else{
    #if DEBUG_MODE
        Serial.println(F("[error: can't send tcp]"));
    #endif
        errorCheck = 1;
        return;
    }
    
    m_puart->print(F("GET "));
    m_puart->print(WEBSOCKET_PATH);
    m_puart->print(F(" HTTP/1.1\r\nHost: "));
    m_puart->print(WEBSOCKET_SERVER);
    m_puart->print(F(":"));
    m_puart->print(WEBSOCKET_PORT);
    m_puart->print(F("\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n"));
    m_puart->print(F("Sec-WebSocket-Key: "));
    m_puart->print(key_base64);
    m_puart->print(F("\r\nSec-WebSocket-Version: 13\r\n"));
    m_puart->print(F("Cookie:v=1;i=Pieceduino;t="));  //29
    m_puart->print(pieceduino_cloud_token);
    m_puart->print(F("\r\n")); //2
    m_puart->print(F("\r\n"));
    
    int i=0;
    String s =  "";
    unsigned long timer;
    timer = millis();
    
    if(FindEspRecv("HTTP")){
    while(1){
        
        if(m_puart->available()){
            char c = (char)m_puart->read();
            //Serial.print(c);
            if(c != '\r' && c != '\n' && c != '\0'){
            s += c;
            }
        }
        if (millis() - timer > 3000){
            break;
        }
    }
    }
    
    //Serial.println(s);
    //Serial.println(s.length());
    //Serial.println(s.indexOf("connected"));
    
    if(s.indexOf("connected")>0){  //s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
    #if DEBUG_MODE
        Serial.println("[ok]");
    #endif
        bWebSocketConnect = true;
    }else{
    #if DEBUG_MODE
        Serial.println(F("[error: can't open Websocket. check token.]"));
    #endif
        //error but ignore
    }  
    errorCheck = 0;  //ignore err when connecting
}

//
int pieceduino::FindEspRecv(String str) {
    unsigned long timeofstart = millis();
    //byte len = StringLength(str);
    byte len = str.length();
    char buf[len];
    byte mode = 0;
    while (1) {
        if (m_puart->available()) {
            char a = m_puart->read();
            //Serial.print(a);
            byte i;
            for (i = 0; i < len; i++) {
                buf[i] = buf[i + 1];
            }
            buf[len - 1] = a;
           
            mode = 1;
            for (i = 0; i < len; i++) {
                if (buf[i] != str[i]) {
                    mode = 0;
                    break;
                }
            }
            if (mode == 1) {
                return 1;
            }
        }
       
        if ((millis() - timeofstart) > ESP8266_TIMEOUT) {
            errorCheck = 1;
            pingtimer = millis();
            return 0;
        }
    }
}

//
int pieceduino::FindEspRecv(String str,String &recv) {
    unsigned long timeofstart = millis();
    //byte len = StringLength(str);
    byte len = str.length();
    char buf[len];
    byte mode = 0;
    String data_tmp;
    while (1) {
        if (m_puart->available()) {
            char a = m_puart->read();
            //Serial.print(a);
            data_tmp += a;
            byte i;
            for (i = 0; i < len; i++) {
                buf[i] = buf[i + 1];
            }
            buf[len - 1] = a;
            
            mode = 1;
            for (i = 0; i < len; i++) {
                if (buf[i] != str[i]) {
                    mode = 0;
                    break;
                }
            }
            if (mode == 1) {
                recv =data_tmp;
                return 1;
                
            }
        }
        
        if ((millis() - timeofstart) > ESP8266_TIMEOUT) {
            errorCheck = 1;
            pingtimer = millis();
            return 0;
        }
    }
}

//
/*
byte pieceduino::StringLength(char *str) {
    byte len = 0;
    while (1) {
        if (str[len] == 0) {
            break;
        } else {
            len++;
        }
    }
    return len;
}
*/
 
// Receive messeage
bool pieceduino::ProcessReceivedCharacter() {
    char a = m_puart->read();
    //Serial.print(a);

    MessageCheckBuf[0] = MessageCheckBuf[1];
    MessageCheckBuf[1] = MessageCheckBuf[2];
    MessageCheckBuf[2] = MessageCheckBuf[3];
    MessageCheckBuf[3] = MessageCheckBuf[4];
    MessageCheckBuf[4] = a;
    
    
    if ((MessageReceivingMode == 0)
        && (MessageCheckBuf[0] == '+')
        && (MessageCheckBuf[1] == 'I')
        && (MessageCheckBuf[2] == 'P')
        && (MessageCheckBuf[3] == 'D')
        && (MessageCheckBuf[4] == ',') ) {
        if(cipmux == 1){
            MessageReceivingMode = 1;
        }else if(cipmux == 0){
            MessageReceivingMode = 2;
        }
        return false;
    }else if ((MessageReceivingMode == 0)//檢測連線狀態
              && (MessageCheckBuf[0] == 'A')
              && (MessageCheckBuf[1] == 'T')
              && (MessageCheckBuf[2] == 'U')
              && (MessageCheckBuf[3] == 'S')
              && (MessageCheckBuf[4] == ':')) {
        //Serial.print("(");
        delay(1);
        char b = m_puart->read();
        if(b != '3'){
            bConnected = 0;
            //Serial.println(bConnected);
        }else{
            bConnected = 1;
            //Serial.println(bConnected);
        }
        while (1) {
            char b = m_puart->read();
            //Serial.print(b);
            if(b == ','){
                //Serial.print(")");
                return false;
            }
            if(bConnected == 0){
                return false;
            }
            delay(1);
        }
        
        return false;
    }
    
    //抓到Client id (mux有開才有)
    if (MessageReceivingMode == 1) {
        if (a == ',') {
            MessageReceivingMode = 2;
            return false;
        }
        String cid = (String)a;
        client_id = cid.toInt();
        //Serial.print("cid: ");
        //Serial.println(cid);
    }
    
    //抓到長度
    if (MessageReceivingMode == 2) {
        if ((a >= '0') && (a <= '9')) {
            MessageSize = MessageSize * 10 + a - 48;
        }
        if (a == ':') {
            MessageReceivingMode = 3;
            MessageSize_return = MessageSize;
        }

        return false;
    }
    
    //抓到訊息
    if (MessageReceivingMode == 3) {
        
        MessageBuffer[MessageCursor] = a;
        MessageCursor++;
        
        //終了処理
        if((MessageCursor == MessageSize) || (MessageCursor == MAX_MESSAGE_SIZE)){
            MessageBuffer[MessageCursor] = '\0';
            
            if(bWebSocketConnect == true){
                ProcessMessage(MessageBuffer, MessageCursor);
            }
            
            MessageReceivingMode = 0;
            MessageSize = 0;
            MessageCursor = 0;
            return true;
        }
    }
    return false;
}

//
void pieceduino::ProcessMessage(char *str, byte len) {
    //str to String obj
    String message;
    message.reserve(len);
    byte l=0;
    for (l=0;l<len;l++){
        message+=str[l];
    }
    
    int cursor=1;
    byte from;
    byte to;
    while(cursor>0){
        from = cursor-1;
        cursor = message.indexOf(B10000001,cursor);
        if(cursor==-1){
            //One websocket frame
            to=len; //to the end of message
        }else{
            //more than one websocket frame
            to=cursor;
            cursor++;
        }
        parseWebsocket(message.substring(from,to));
    }
}
void  pieceduino::parseWebsocket(String frame){
    if (((byte)frame.charAt(0)==B10000001)
        &&((((byte)frame.charAt(1))&B10000000) == B00000000)){
        parseSocketIo(frame.substring(2));
    }else{
    }
}

//
void pieceduino::parseSocketIo(String frame){
    if(frame.charAt(0)=='3'){
#if DEBUG_MODE
        Serial.println(F("[ok]"));
#endif
    }else if((frame.charAt(0)=='4')&&(frame.charAt(1)=='2')){
        frame.remove(0,2);
        parseData(frame);
    }else{
    }
}

//
void pieceduino::parseData(String frame){
    
    if(frame.charAt(2)=='c'){ //is f (float) frame?
#if DEBUG_MODE
        /*
        String debugMsg = "Catch key: ";
        debugMsg +=frame.charAt(17);
        debugMsg +=" value: ";
        debugMsg +=frame.substring(33,frame.length()-5).toFloat();
        Serial.println(frame);
         */
#endif
        //call Catch function
        _WebSocketInCallback(frame.charAt(17),
              frame.substring(33,frame.length()-5).toFloat());
    }else{
        
    }
}

//
void pieceduino::setCallback(void (*WebSocketInCallback)(char key, float value))
{
    _WebSocketInCallback = WebSocketInCallback;
}

//
void pieceduino::WebSocketSendText(String str) {
    
    uint8_t len = str.length();
    uint8_t header_len;
    
    if (len > 125) {
        header_len = 8;
    }else{
        header_len = 6;
    }
    
    char Frame[header_len + str.length() + 1];
    
    if (len > 125) {
        Frame[0] = WS_OPCODE_TEXT | WS_FIN; //FIN(1),RSV(000),opcode(0001:TXT)
        Frame[1] = (WS_SIZE16 | WS_MASK);
        Frame[2] = ((uint8_t) (len >> 8));
        Frame[3] = ((uint8_t) (len & 0xFF));
        Frame[4] = random(0, 256); //MASK
        Frame[5] = random(0, 256); //MASK
        Frame[6] = random(0, 256); //MASK
        Frame[7] = random(0, 256); //MASK
    } else {
        Frame[0] = WS_OPCODE_TEXT | WS_FIN; //FIN(1),RSV(000),opcode(0001:TXT)
        Frame[1] = (len | WS_MASK);
        Frame[2] = random(0, 256); //MASK
        Frame[3] = random(0, 256); //MASK
        Frame[4] = random(0, 256); //MASK
        Frame[5] = random(0, 256); //MASK
    }
    
    
    byte i = 0;

    for (i = 0; i < str.length(); i++) {
        Frame[header_len + i] = str[i] ^ Frame[i % 4 + 2];
    }
    Frame[header_len + str.length()] = B00000000;
    Send(Frame, header_len + str.length());
    
}

//
bool pieceduino::Throw(String key, float value)
{
    if(false){
        
        Serial.print(F("Throw key: "));
        Serial.print(key);
        Serial.print(F(" value: "));
        Serial.println(value);
    }
    String frame;
    
    frame ="42[\"c_Data\",\"";
    frame +="{\\\"from\\\":\\\"Pieceduino\\\",\\\"key\\\":\\\"";
    frame +=key;
    frame +="\\\",\\\"value\\\":";
    frame +=value;
    frame +=",\\\"token\\\":\\\"";
    frame +=pieceduino_cloud_token;
    frame +="\\\"}";
    frame +="\"]";
    #if DEBUG_MODE
    //Serial.println(frame);
    #endif
    WebSocketSendText(frame);
 
    return true;
}

//
void pieceduino::Send(String str, byte len) {
    m_puart->print(F("AT+CIPSEND="));
    m_puart->println(len);
    if(FindEspRecv("> ")){//wait for prompt
    }else{
#if DEBUG_MODE
        Serial.println(F("error: can't send tcp"));
#endif
        errorCheck = 1;
        return;
    }

    int i;
    for (i = 0; i < len; i++) {

        m_puart->print(str[i]);
        if ((i % 64) == 63) {
            delay(20);
        }
    }
    FindEspRecv("SEND OK\r\n");
}

//
void pieceduino::Send(uint8_t mux_id, String str, byte len) {
    m_puart->print("AT+CIPSEND=");
    m_puart->print(mux_id);
    m_puart->print(",");
    m_puart->println(len);

    if(FindEspRecv("> ")){//wait for prompt
    }else{
#if DEBUG_MODE
        Serial.println(F("error: can't send tcp"));
#endif
        errorCheck = 1;
        return;
    }
    
    m_puart->print(str);
    /*
    int i;
    for (i = 0; i < len; i++) {
        
        Serial1.print(str[i]);
        if ((i % 64) == 63) {
            delay(20);
        }
    }
     */
    FindEspRecv("SEND OK\r\n");
}

//
void pieceduino::rx_empty(void)
{
    while(m_puart->available() > 0) {
        m_puart->read();
    }
}
