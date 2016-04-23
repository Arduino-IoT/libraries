// 引用 Keypad library
#include <Keypad.h>
#include "UsbKeypad.h"

// 3x4 Keypad
const byte ROWS = 4; // 4 Rows
const byte COLS = 3; // 3 Columns

// 定義 Keypad 的按鍵
char keys[ROWS][COLS] = {
  {KEY_1, KEY_2, KEY_3},
  {KEY_4, KEY_5, KEY_6},
  {KEY_7, KEY_8, KEY_9},
  {KEY_ENTER , KEY_0, KEY_ENTER}      // '*' and '#' as "enter" keystroke
};

// 定義 Keypad 連到 Arduino 的接腳
byte rowPins[ROWS] = {9, 8, 7, 6};   // 連到 Keypad 的 4 個 Rows: Row0, Row1, Row2, Row3 
byte colPins[COLS] = {12, 11, 10};   // 連到 Keypad 的 3 個 Columns: Column0, Column1, Column2

// 建立 Keypad 物件
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  keypad.setDebounceTime(100);     // debounce time 預設是 50 mS，改成 100 mS 比較不太靈敏
}
  
void loop(){
  // 讓 V-USB driver 處理工作，至少每 50 ms 要跑一次
  UsbKeypad.update();
  
  char key = keypad.getKey();
  
  // 檢查 Keypad 有沒被按下
  // 有的話，key 會是非 NO_KEY 的值, NO_KEY 代表沒有按鍵被按下
  if (key != NO_KEY){
    // 送出對應的數字鍵 keystroke 給電腦
    UsbKeypad.sendKeyStroke(key);
  }
}
