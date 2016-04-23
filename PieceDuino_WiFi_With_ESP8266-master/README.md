# PieceDuino_WiFi_With_ESP8266
這一個專為PieceDuino所寫的ESP8266 Library</br>
但也相容於所有透過Arduino的ESP8266模組

bool begin();</br>
bool reset();</br>
bool setWifiMode(int pattern);</br>
bool setAP(String ssid, String pwd , uint8_t channel, uint8_t ecn);</br>
bool connToWifi(String ssid, String pwd);</br>
String getVersion();</br>
String getIP();</br>
bool Throw(char key, float value);</br>
bool createTCPServer(uint32_t port = 3333);</br>
bool createTCP(String addr, uint32_t port);</br>
bool enableMUX();</br>
bool disableMUX();</br>
uint32_t recv();</br>
void Send(uint8_t mux_id,char *str, byte len) ;</br>
void Send(char *str, byte len);</br>
void WebSocketConnect(String token);</br>
void SaveDataToCloud(String key, String data);</br>
