#include "L298Lib.h"
#include <Streaming.h>
#include <Arduino.h>

int speed[2];
int dir[2];
int motor;

void L298_setup(void)
{
  pinMode(M1_in1, OUTPUT);
  pinMode(M1_in2, OUTPUT);
  pinMode(M2_in3, OUTPUT);
  pinMode(M2_in4, OUTPUT);

  motor = MOTOR_M1;
}

void Control (char ch)
{
      switch (ch) {
      case 'r' :
      case 'R' :
        motor = MOTOR_M1;  
        break;
      case 'l' :
      case 'L' :
        motor = MOTOR_M2;  
        break;
      case '+' :
        speed[motor] = speed[motor] + 1;
        if (speed[motor]>10) speed[motor]=10;
        break;
      case '-' :
        speed[motor]--;
        if (speed[motor]<0) speed[motor]=0;
        break;
      case 'f' :
      case 'F' : //　前進
        dir[motor] = DIR_FORWARD;
        break;
      case 'b' :  
      case 'B' : //　倒車
        dir[motor] = DIR_BACKWARD;
        break;
      case 's' :
      case 'S' :
        speed[motor] = 0;
        break;
      default : //　停止
        speed[0] = 0;
        speed[1] = 0;
        DriveSingleMotor(0, speed[motor], dir[motor]); 
        DriveSingleMotor(1, speed[motor], dir[motor]); 
    }
    //Serial << "MOTOR ; " << motor << " , SPEED: " << speed[motor] << " , DIR: " << dir[motor] << endl;
    DriveSingleMotor(motor, speed[motor], dir[motor]);  
}

// m: 0 => M1      1 => M2
// power 0~10     dir: 0=>正向　1=>反向
void DriveSingleMotor(int m, int speed, int dir)
{
  int _speed = 0;

  //　設定速度
  if (speed>10) speed=10;
    _speed = map(speed, 1, 10, 100, 255);
  
  if (speed<=0) {
    speed=0;
    _speed=0;
  } else 
    _speed = map(speed, 1, 10, 100, 255);
  
  if (m == MOTOR_M1) {
    //　設定方向        
    if (dir ==  DIR_FORWARD) {
      digitalWrite(M1_in1, HIGH);  //右輪前進
      digitalWrite(M1_in2, LOW);
    }else {
      digitalWrite(M1_in1, LOW);  //右輪倒轉R
      digitalWrite(M1_in2, HIGH);  
    }
    analogWrite(M1_enable, _speed);  // 驅動馬達  右輪

  } else if (m == MOTOR_M2) {
    //　設定方向        
    if (dir == DIR_FORWARD) {
      digitalWrite(M2_in3, HIGH);  //左輪前進
      digitalWrite(M2_in4, LOW);
    }else {
      digitalWrite(M2_in3, LOW);  //左輪倒轉
      digitalWrite(M2_in4, HIGH);
    }
    analogWrite(M2_enable, _speed);    // 驅動馬達  左輪 

  } else {
    analogWrite(M1_enable, PWR_STOP);  //右輪
    analogWrite(M2_enable, PWR_STOP);  //左輪
  }
  //Serial << "TEST: motor " << m << " , speed " << speed << " , _speed " << _speed  << " , dir "  << dir << endl;
  //Serial << "-----------------------------------------" << endl;
}
