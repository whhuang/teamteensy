#include <TeensyThreads.h>
#include <Encoder.h>
#include <i2c_t3.h>
#include "Adafruit_VL6180X_upgrade.h"
#include <LSM6_upgrade.h>
//constants
const int L = 0;
const int R = 1;
const int BOTH = 2;
const int NONE = 3;
const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;
const int TURNING = 4;
const double MAZE_SQUARE_SIZE = 5197.0;  //number of encoder ticks in a square
int leftTurn   = 10400000;
int rightTurn = -10200000; //these might need minor tweaks
int left180 = leftTurn * 2;
int right180 = rightTurn * 2;
const int TOP_SPEED = 255;

//the magnetic encoder makes roughly 660 ticks per inch of travel

int gyro_calibrate = -380;

// variable variables
volatile int v1Range = 404; // This must be calculated in loop with the gyro to avoid SDA conflicts
int relativeDirection = 0; // does this need to be volatile?
int angularVelocity = 0;
int dt = 20;
volatile int facing = EAST;
volatile double xLocation = 0.0; //might adjust these. not sure how the coordinates will actually work
volatile double yLocation = 0.0; // these coordinates refer to the location directly between the robot's wheels, because that's the center of rotation.

// pins
const int m1_1 = 21;
const int m1_2 = 20;
const int m2_1 = 23;
const int m2_2 = 22;
const int ledPin = 13;
Encoder leftEnc(17, 16);
Encoder rightEnc(0, 1);

// sensors
Adafruit_VL6180X v1 = Adafruit_VL6180X();
Adafruit_VL6180X v2 = Adafruit_VL6180X();
Adafruit_VL6180X v3 = Adafruit_VL6180X();
LSM6 imu;



void setup() {
  Serial.begin(9600);
  pinMode(m1_1, OUTPUT);
  pinMode(m1_2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  //threads.addThread(motorThread);

  /*
  // this absolutely MUST be commented out for it to work unless it's connected to USB
  while (!Serial) {
    delay(1);
  }
  */
  
  Serial.println("Adafruit VL6180x");
  if (! v1.begin(WIRE_0)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("v1");
  if (! v2.begin(WIRE_1)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  if (! v3.begin(WIRE_2)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
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
  // make a temp x or y coordinate that doesn't change. Real coordinate = temp + leftEnc.read(); on every loop
  const float kP = 0.2;
  const float kI = 0.01;
  const float kD = 1;
  const float twist_kP = 1;
  int tempCoordinate;
  if (facing == NORTH || facing == SOUTH) {
    tempCoordinate = yLocation;
  }
  else if (facing == EAST || facing == WEST) {
    tempCoordinate = xLocation;
  }
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

void goStraightUntilWall() {
  Serial.println("go until wall");
  const float twist_kP = 1;
  const float wall_wall_kP = 10;
  const float wall_wall_kD = 30;
  int tempCoordinate;
  if (facing == NORTH || facing == SOUTH) {
    tempCoordinate = yLocation;
  }
  else if (facing == EAST || facing == WEST) {
    tempCoordinate = xLocation;
  }
  leftEnc.write(0);
  rightEnc.write(0);
  if (wallAhead() <= 90)
    return;
  int lastMode = NONE;
  int lastError = 0;
  while (wallAhead() > 90) {
    /* If there is a wall on the left and right, center the robot between them.
     * If there is just a wall on the left, put the robot 50mm from that wall.
     * If just on the right, 50mm from that wall.
     * Else, use encoders to keep tracking straight.
     */
    int leftWallDist = wallLeft();
    int rightWallDist = wallRight();
    int leftWallTarget = 50; //desired distance from each wall
    int rightWallTarget = 50;
    int leftPower = 0;
    int rightPower = 0;
    
    if (leftWallDist < 100) {
      if (rightWallDist < 100) {
        //2 walls
        int wallError = leftWallDist - rightWallDist;
        int d = 0;
        if (lastMode == BOTH) {
          d = wallError - lastError;
        }
        leftPower = 255 - d*wall_wall_kD - wallError*wall_wall_kP;
        rightPower = 255 + d*wall_wall_kD + wallError*wall_wall_kP;
        //leftPower = 255 - maximum(wallError*wall_wall_kP, 100);
        //rightPower = 255 + maximum(wallError*wall_wall_kP, 100);
        lastMode = BOTH;
        lastError = wallError;
      }
      else {
        // just left wall
        int wallError = leftWallDist - leftWallTarget;
        int d = 0;
        if (lastMode == L) {
          //Serial.print("calculating d");
          d = wallError - lastError;
        }
        //leftPower = 255 - maximum(d*wall_wall_kD, 50) - maximum(wallError*wall_wall_kP, 80);
        //rightPower = 255 + maximum(d*wall_wall_kD, 50) + maximum(wallError*wall_wall_kP, 80);
        leftPower = 255 - maximum(wallError*wall_wall_kP, 100);
        rightPower = 255 + maximum(wallError*wall_wall_kP, 100);
        lastMode = L;
        lastError = wallError;
      }
      rightEnc.write(leftEnc.read()); // sync up the encoders because it's not reading them
    }
    else if (rightWallDist < 100) {
      // just right wall
      int wallError = rightWallTarget - rightWallDist;
      int d = 0;
      if (lastMode == R) {
        d = wallError - lastError;
      }
      leftPower = 255 - d*wall_wall_kD - wallError*wall_wall_kP;
      rightPower = 255 + d*wall_wall_kD - wallError*wall_wall_kP;
      //leftPower = 255 - maximum(wallError*wall_wall_kP, 100);
      //rightPower = 255 + maximum(wallError*wall_wall_kP, 100);

      lastMode = R;
      lastError = wallError;
      rightEnc.write(leftEnc.read()); // sync up the encoders because it's not reading them
    }
    else {
      //no walls
      int leftPos = leftEnc.read();
      int rightPos = rightEnc.read();
      int twistError = leftPos - rightPos;
      leftPower = 255 - twistError*twist_kP;
      rightPower = 255 + twistError*twist_kP;
    }

    /*Serial.print("Left: "); Serial.print(leftPower);
    Serial.print(" Right: "); Serial.println(rightPower);*/
    leftMotor(leftPower);
    rightMotor(rightPower); // only one of these will take effect because 255 is max power
    updateLocation(tempCoordinate, leftEnc.read());
    threads.delay(25);
  }

  // wallAhead() detected a wall
  leftMotor(-100);
  rightMotor(-100);
  threads.delay(50);
  leftMotor(0);
  rightMotor(0);
  return;
    
}

void driveUntilNode(Node node/*not sure how to import the node*/) {
  //drive forward in a straight line until reaching the specified node. do so by looking at the coordinates and then confirming that the robot is centered at the destination node because relying purely on coordinates wouldn't work
}

void roudCoordinates () {
  //round xLocation and yLocation to nearest integer
}

boolean atTheCenter() {
  //returns true if the robot is at one of the center coordinates, which I think are (7,7),(7,8),(8,7),and (8,8)
}
void makeTurn(int dist) {
  //ISSUE: sometimes it doesn't quite complete the turn. it gets close and can't finish
  // positive is CCW
  int wasFacing = facing;
  facing = TURNING;
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
    if (nearZero(error,leftTurn/20) && nearZero(d, 2500)) {// close to destination but the motor power isn't enough to move the robot
      Serial.print("Booost");
      pwr*=1.3;
    }
    /*Serial.print("Error: "); Serial.print(error);
    Serial.print(" Power: "); Serial.print(pwr);
    Serial.print(" Speed: "); Serial.println(d);*/
    rightMotor(pwr);
    leftMotor(-pwr);
    lastError = error;
    
    if (nearZero(error, leftTurn / 60) && nearZero(angularVelocity, 1000)){ // about 1.5 degree tolerance
      leftMotor(0);
      rightMotor(0);
      facing = (((wasFacing + dist/rightTurn) % 4)+ 4) % 4;
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
void driveUntilWall() {
  Serial.println("driveuntilwall");
  int driving = threads.addThread(goStraightUntilWall);
  
  while(threads.getState(driving) == threads.RUNNING) {
    //any code that you want to run while it's driving can go right here.
    Serial.print(facing);Serial.print(" ");Serial.print(xLocation);Serial.print(" ");Serial.println(yLocation);
    threads.delay(100);
  }
  threads.wait(driving, 0); // 0 can be replacaed with time limit
  //the threads.wait() line isnt necessary if you have the while loop
}

void turn (int amount) {
  Serial.print("Initiating Turn "); Serial.println(amount);
  int turning = threads.addThread(makeTurn, amount);
  /*while(threads.getState(turning) != threads.ENDED) {
    Serial.print(facing);Serial.print(xLocation);Serial.println(yLocation);
  }*/
  threads.wait(turning, 0); // time limit?
}

void loop() {
  /*long newPosition = leftEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    //Serial.println(newPosition);
  }*/
  //makeTurn(leftTurn);

  //back and forth
  /*threads.delay(500);
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
  turn(right180);*/
  

  /*threads.delay(2000);
  goStraightUntilWall();
  */
  /*uint8_t range = v1.readRange();
  uint8_t status = v1.readRangeStatus();
  uint8_t range2 = v2.readRange();
  uint8_t status2 = v2.readRangeStatus();
  uint8_t range3 = v3.readRange();
  uint8_t status3 = v3.readRangeStatus();
  */

  threads.delay(500);
  driveUntilWall();
  threads.delay(500);
  turn(rightTurn);


}
    
