//#include <LIS3MDL.h>
//#include <LSM6.h>
#include <TeensyThreads.h>
//#include <Adafruit_VL6180X.h>
#include <Encoder.h>
//constants
const int L = 0;
const int R = 1;
const float kP = 0.2;
const float kI = 0.01;
const float kD = 1;
const float twist_kP = 1;
const int TOP_SPEED = 255;
//the magnetic encoder makes roughly 660 ticks per inch of travel

// pins
const int m1_1 = 21;
const int m1_2 = 20;
const int m2_1 = 23;
const int m2_2 = 22;
const int ledPin = 13;
Encoder leftEnc(17, 16);
Encoder rightEnc(33, 34);

void setup() {
  Serial.begin(9600);
  pinMode(m1_1, OUTPUT);
  pinMode(m1_2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  //threads.addThread(motorThread);
}

void goStraight (int dist) {
  leftEnc.write(0);
  rightEnc.write(0);
  int lastError = 0;
  int error; // how far the robot is from its destination
  int integral = 0;
  int twistError; // left encoder - right encoder
  const int integralMax = 10000;
  while (1) {
    int leftPos = leftEnc.read();
    int rightPos = rightEnc.read();
    error = dist - (leftPos + rightPos ) / 2; // average left and right. maybe change this
    twistError = leftPos - rightPos;
    int d = error - lastError; // change in position
    integral += error;
    integral = maximum(integral, integralMax);
    int pwr = maximum(error*kP + integral*kI, TOP_SPEED);
    int twistCorrect = twist_kP*twistError;
    Serial.print("error: ");
    Serial.print(error);
    Serial.print("derivative: ");
    Serial.print(d);
    Serial.print("Integral: ");
    Serial.print(integral);
    Serial.print("Left power: ");
    Serial.print(pwr - twistCorrect);
    Serial.print("Right power: ");
    Serial.println(pwr+twistCorrect);
    leftMotor(pwr - twistCorrect);
    rightMotor(pwr + twistCorrect);
    
    delay(25);
    if (nearZero(error, 13) && nearZero(d, 10) && nearZero(leftPos - rightPos, 13)) { //error of 13 = 0.02inches
      leftMotor(0);
      rightMotor(0);
      return;
    }
    lastError = error;
  }
}

long oldPosition  = -999;
void drive (int dist) {
  int driving = threads.addThread(goStraight, dist);
  threads.wait(driving, 0); // 0 can be replacaed with time limit
}

void loop() {
  long newPosition = leftEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    //Serial.println(newPosition);
  }
  drive(10000);
  delay(500);
  drive(-10000);
  delay(500);
  
}
    
