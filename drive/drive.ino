#include <TeensyThreads.h>
#include <Encoder.h>
#include <i2c_t3.h>
#include "Adafruit_VL6180X_upgrade.h"
#include <LSM6_upgrade.h>
//constants
const int L = 0;
const int R = 1;

const int TOP_SPEED = 255;
//the magnetic encoder makes roughly 660 ticks per inch of travel

int gyro_calibrate = -380;
int relativeDirection = 0; // does this need to be volatile?
int angularVelocity = 0;
int dt = 20;
//int leftTurn   = 10500000;
//int rightTurn = -10300000;
int leftTurn   = 10400000;
int rightTurn = -10200000; //these might need minor tweaks
int left180 = leftTurn * 2;
int right180 = rightTurn * 2;

// pins
const int m1_1 = 21;
const int m1_2 = 20;
const int m2_1 = 23;
const int m2_2 = 22;
const int ledPin = 13;
Encoder leftEnc(17, 16);
Encoder rightEnc(0, 1);

// sensors
Adafruit_VL6180X vl = Adafruit_VL6180X();
Adafruit_VL6180X v2 = Adafruit_VL6180X();
//Adafruit_VL6180X v3 = Adafruit_VL6180X();
LSM6 imu;


void setup() {
  Serial.begin(9600);
  pinMode(m1_1, OUTPUT);
  pinMode(m1_2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  //threads.addThread(motorThread);

  /*while (!Serial) {
    delay(1);
  }*/
  
  Serial.println("Adafruit VL6180x");
  if (! vl.begin(WIRE_0)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("v1");
  if (! v2.begin(WIRE_1)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  /*if (! v3.begin(WIRE_2)) {
    Serial.println("Failed to find sensor");
    while (1);
  }*/
  Serial.println("distance sensors working");
  
  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
  
  Serial.println("all sensors found");
  threads.addThread(readIMUloop);
}



void goStraight (int dist) {
  const float kP = 0.2;
  const float kI = 0.01;
  const float kD = 1;
  const float twist_kP = 1;
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
    
    threads.delay(25);
    if (nearZero(error, 13) && nearZero(d, 10) && nearZero(leftPos - rightPos, 13)) { //error of 13 = 0.02inches
      leftMotor(0);
      rightMotor(0);
      return;
    }
    lastError = error;
  }
}

void makeTurn(int dist) {
  // positive is CCW
  relativeDirection = 0; // reset direction to zero before starting
  int lastError = 0;
  int error;
  int integral;
  // Ziegler-Nichols PI values: kP = 0.000054; kI = 0.0000037;
  const float kP = 0.000054;
  const float kI = 0.0000037;
  const float kD = 0;
  const int integralMax = 1000000;
  
  while (1) {
    error = dist - relativeDirection;
    int d = angularVelocity;
    //int d = error - lastError;
    integral += error;
    integral = maximum(integral, integralMax);
    
    int pwr = maximum(error*kP + integral*kI + d*kD, TOP_SPEED);
    if (nearZero(error,leftTurn/30) && nearZero(d, 2000)) {// close to destination but the motor power isnt' enough to move the robot
      Serial.print("Booost");
      pwr*=1.3;
    }
    Serial.print("Error: "); Serial.print(error);
    Serial.print(" Power: "); Serial.print(pwr);
    Serial.print(" Speed: "); Serial.println(d);
    rightMotor(pwr);
    leftMotor(-pwr);
    lastError = error;
    
    if (nearZero(error, leftTurn / 60) && nearZero(angularVelocity, 1000)){ // about 1.5 degree tolerance
      leftMotor(0);
      rightMotor(0);
      return;
    }
    threads.delay(25);
  }
  
}

long oldPosition  = -999;
void drive (int dist) {
  int driving = threads.addThread(goStraight, dist);
  //any code that you want to run while it's driving can go right here.
  threads.wait(driving, 0); // 0 can be replacaed with time limit
}

void turn (int amount) {
  Serial.print("Initiating Turn "); Serial.println(amount);
  int turning = threads.addThread(makeTurn, amount);
  threads.wait(turning, 0); // time limit?
}

void loop() {
  long newPosition = leftEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    //Serial.println(newPosition);
  }
  //makeTurn(leftTurn);

  //back and forth
  threads.delay(500);
  drive(10000);
  threads.delay(500);
  turn(left180);
  threads.delay(500);
  drive(10000);
  threads.delay(500);
  turn(left180);
  threads.delay(500);
  drive(10000);
  threads.delay(500);
  turn(right180);
  threads.delay(500);
  drive(10000);
  threads.delay(500);
  turn(right180);
  
  //makeTurn(rightTurn);
  //turn(rightTurn);
  /*drive(10000);
  threads.delay(500);
  drive(-10000);
  threads.delay(500);
  */
  
  /*turn(leftTurn);
  delay(1000);
  turn(rightTurn);
  delay(1000);*/
}
    
