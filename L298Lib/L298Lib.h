#include <arduino.h>

const int M1_enable = 6;
const int M2_enable = 5;

const int M1_in1 = 12;
const int M1_in2 = 11;
const int M2_in3 = 10;
const int M2_in4 =  9;

const int MOTOR_M1=0;
const int MOTOR_M2=1;
const int DIR_FORWARD= 0;
const int DIR_BACKWARD=1;
const int PWR_STOP = 0;

void L298_setup(void);
void Control (char ch);
void DriveSingleMotor(int m, int speed, int dir);

