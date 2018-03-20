#include <SimpleTimer.h>
#include <TeensyThreads.h>
#include <Encoder.h>

//constants
const int L = 0;
const int R = 1;
const float kP = 0.2;
const float kI = 0.01;
const float kD = 1;
const float twist_kP = 1;
const int TOP_SPEED = 255;
int gyro_calibrate = -380;
int direction = 0;
int dt = 20;
unsigned long lastTime = 0;
int leftTurn   = 10500000;
int rightTurn = -10300000;
int left180 = leftTurn * 2;
int right180 = rightTurn * 2;

//drivePID variables
int driveDist;
int lastError = 0;
int integral = 0;
const int integralMax = 10000;
volatile boolean driving = false;

SimpleTimer timer;

// pins
const int m1_1 = 21;
const int m1_2 = 20;
const int m2_1 = 23;
const int m2_2 = 22;
const int ledPin = 13;
Encoder leftEnc(17, 16);
Encoder rightEnc(0, 1);

// sensors
//LSM6 imu;

void setup() {
  Serial.begin(9600);
  pinMode(m1_1, OUTPUT);
  pinMode(m1_2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  //timer.setTimeout(2000, backAndForth);
  Serial.println("setup");
  int backAndForthThread = threads.addThread(backAndForth);
}

void backAndForth() {
 Serial.println("SFDSFSF");
  while(1){
    Serial.println("back and forth");
    goStraight(10000);
    while(driving) {};
    
  }
  //while (driving) {}
  //Serial.println("done");
}
void drivePID() {
  int leftPos = leftEnc.read();
  int rightPos = rightEnc.read();
  int error = driveDist - (leftPos + rightPos ) / 2; // average left and right. maybe change this
  int twistError = leftPos - rightPos;
  int d = error - lastError; // change in position
  integral += error;
  integral = maximum(integral, integralMax);
  int pwr = maximum(error*kP + integral*kI, TOP_SPEED);
  int twistCorrect = twist_kP*twistError;
  /*Serial.print("error: ");
  Serial.print(error);
  Serial.print("derivative: ");
  Serial.print(d);
  Serial.print("Integral: ");
  Serial.print(integral);
  Serial.print("Left power: ");
  Serial.print(pwr - twistCorrect);
  Serial.print("Right power: ");
  Serial.println(pwr+twistCorrect);*/
  leftMotor(pwr - twistCorrect);
  rightMotor(pwr + twistCorrect);
  
  if (nearZero(error, 13) && nearZero(d, 10) && nearZero(leftPos - rightPos, 13)) { //error of 13 = 0.02inches
    leftMotor(0);
    rightMotor(0);
    driving = false;
    Serial.println("done driving");
    return;
  }
  lastError = error;
}


void drivePIDlooper() { // runs drivePID() every 25 ms, better than using delays.
  drivePID();
  unsigned long lastDriveTime = millis();
  while (1) {
    unsigned long time = millis();
    if (time - lastDriveTime >= 25){
      lastDriveTime = time;
      drivePID();
    }
    if (!driving) {
      return;
    }
  }
}

void goStraight(int dist) {
  Serial.print("going straight ");
  Serial.println(dist);
  driveDist = dist;
  leftEnc.write(0);
  rightEnc.write(0);
  lastError = 0;
  integral = 0;
  driving = true;
  int driver = timer.setInterval(25, drivePID);
  //int driver = threads.addThread(drivePIDlooper);
  //timer.disable(driver);
  //threads.wait(driver, 0);
  Serial.println("done");
}

void loop() {
  timer.run();
}
