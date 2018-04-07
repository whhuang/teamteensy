#include <TeensyThreads.h>
#include <Encoder.h>
#include <i2c_t3.h>
#include "Adafruit_VL6180X_upgrade.h"
//#include "Adafruit_VL6180X.h"
#include <LSM6_upgrade.h>
//#include <LSM6.h>
#include <math.h>
#include <stdlib.h>
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
//const double MAZE_SQUARE_SIZE = 5197.0;  //number of encoder ticks in a square
const double MAZE_SQUARE_SIZE = 4600.0;  //number of encoder ticks in a square
int leftTurn   = 10200000;//10400000;
int rightTurn = -9700000; //-10000000 //these might need minor tweaks
int left180 = leftTurn * 2;
int right180 = rightTurn * 2;
const int TOP_SPEED = 255;

//the magnetic encoder makes roughly 660 ticks per inch of travel

int gyro_calibrate = -380;

// variable variables
volatile int v1Range = 404; // This must be calculated in loop with the gyro to avoid SDA conflicts
volatile int v2Range = 404;
volatile int v3Range = 404;
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
const int v3Shutdown = 10;
const int v2Shutdown = 9;
Encoder leftEnc(17, 16);
Encoder rightEnc(0, 1);

// sensors
Adafruit_VL6180X v1 = Adafruit_VL6180X();
Adafruit_VL6180X v2 = Adafruit_VL6180X();
Adafruit_VL6180X v3 = Adafruit_VL6180X();
LSM6 imu;

const int LEFTB = 1;
const int RIGHTB = 2;
const int FRONTB = 4;

void setup() {
  Serial.begin(9600);
  pinMode(m1_1, OUTPUT);
  pinMode(m1_2, OUTPUT);
  pinMode(m2_1, OUTPUT);
  pinMode(m2_2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  //threads.addThread(motorThread);

  
  // this absolutely MUST be commented out for it to work unless it's connected to USB
  /*
  while (!Serial) {
    delay(1);
  }
  */
  
  
  /**** old sensor initilizations
  Serial.println("Adafruit VL6180x");
  
   if (! v1.begin(WIRE_0)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  
  Serial.println("v1");
  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
  Serial.println("IMU found");
  

  // changing one of distance sensor addresses
  pinMode(V2_RESET_PIN, OUTPUT);
  digitalWrite(V2_RESET_PIN, LOW);
  delay(10);
  if (! v3.begin(WIRE_2)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("v3");
  v3.setAddress(0b0111001);
  delay(10);
  digitalWrite(V2_RESET_PIN, HIGH);
  
  if (! v2.begin(WIRE_2)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("v2");
  
  
  
  
  Serial.println("distance sensors working");*/
  pinMode(v3Shutdown, OUTPUT);
  pinMode(v2Shutdown, OUTPUT);
  digitalWrite(v3Shutdown, LOW);
  digitalWrite(v2Shutdown, LOW);
  delay(10);
  Serial.println("Starting to initialize sensors");
  if (! v1.begin(WIRE_0)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  v1.setAddress(0b0111001);
  Serial.println("v1");
  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
  Serial.println("IMU found");
  

  // changing one of distance sensor addresses
  //pinMode(V2_RESET_PIN, OUTPUT);
  //digitalWrite(V2_RESET_PIN, LOW);
  digitalWrite(v3Shutdown, HIGH);
  delay(10);
  if (! v3.begin(WIRE_0)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("v3");
  v3.setAddress(0b0111101);
  delay(10);
  //digitalWrite(V2_RESET_PIN, HIGH);
  digitalWrite(v2Shutdown, HIGH);
  if (! v2.begin(WIRE_0)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("v2");
  
  Serial.println("all sensors found");
  
  threads.addThread(readIMUloop);
  
}

int goStraightUntilCorridor() {
  Serial.println("go straight until corridor");
  int wallAheadCutoff = 22;
  
  const float twist_kP = 1;
  const float wall_wall_kP = 10;
  const float wall_wall_kD = 30;
  const int MAX_ENCODER_DIFF = 100;
  int tempCoordinate;
  double previousTile;
  if (facing == NORTH || facing == SOUTH) {
    tempCoordinate = yLocation;
  }
  else if (facing == EAST || facing == WEST) {
    tempCoordinate = xLocation;
  }
  previousTile = 0; // increment each time the robot goes another tile forward 
  leftEnc.write(0);
  rightEnc.write(0);

  int lastMode = NONE;
  int lastError = 0;
  //while (wallAhead() > wallAheadCutoff) {
  while(1) {  

     
    /* If there is a wall on the left and right, center the robot between them.
     * If there is just a wall on the left, put the robot 50mm from that wall.
     * If just on the right, 50mm from that wall.
     * Else, use encoders to keep tracking straight.
     */
    int leftWallDist = wallLeft();
    int rightWallDist = wallRight();
    int frontWallDist = wallAhead();
    int leftWallTarget = 50; //desired distance from each wall
    int rightWallTarget = 50;
    int leftPower = 0;
    int rightPower = 0;
    int leftEncReading = leftEnc.read();
    int rightEncReading = rightEnc.read();

    /*
     * Check if left or right wall is missing at each tile
     */
    //first check if there's a wall ahead because then it needs to brake;
    if (frontWallDist <= wallAheadCutoff) {
      brake();
      roundCoordinates(); // probably useful
      return (leftWallDist <= 100) * LEFTB + (rightWallDist <= 100) * RIGHTB + (frontWallDist <= wallAheadCutoff) * FRONTB;
    }
    if (leftEncReading / MAZE_SQUARE_SIZE - previousTile >= 1) {
      
      int result = (leftWallDist <= 100) * LEFTB + (rightWallDist <= 100) * RIGHTB + (frontWallDist <= wallAheadCutoff) * FRONTB;
      if (result > 0) return result;
      previousTile += 1;
    }
    // else, robot is between tiles or (there are walls on each side && no wall ahead)
    
    if (leftWallDist < 100) {
      if (rightWallDist < 100) {
        //2 walls
        int wallError = leftWallDist - rightWallDist;
        int d = 0;
        if (lastMode == BOTH) {
          d = wallError - lastError;
        }
        int correction = d*wall_wall_kD + wallError*wall_wall_kP;
        int encoder_difference = rightEncReading - leftEncReading;
        if (encoder_difference > MAX_ENCODER_DIFF && correction > 0 ||
          encoder_difference < MAX_ENCODER_DIFF && correction < 0 ) correction = 0; //encoders are too far apart, don't overcorrect. maybe use negative feedback here instead of zero. 
        leftPower = 255 - correction;
        rightPower = 255 + correction;
        //leftPower = 255 - d*wall_wall_kD - wallError*wall_wall_kP;
        //rightPower = 255 + d*wall_wall_kD + wallError*wall_wall_kP;these sorta worked
        
        
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
        int correction = d*wall_wall_kD + wallError*wall_wall_kP;
        int encoder_difference = rightEncReading - leftEncReading;
        if (encoder_difference > MAX_ENCODER_DIFF && correction > 0 ||
          encoder_difference < MAX_ENCODER_DIFF && correction < 0 ) correction = 0; //encoders are too far apart, don't overcorrect. maybe use negative feedback here instead of zero. 
        leftPower = 255 - correction;
        rightPower = 255 + correction;
        lastMode = L;
        lastError = wallError;
      }
      //rightEnc.write(leftEnc.read()); // sync up the encoders because it's not reading them
    }
    else if (rightWallDist < 100) {
      // just right wall
      int wallError = rightWallTarget - rightWallDist;
      int d = 0;
      if (lastMode == R) {
        d = wallError - lastError;
      }
      //leftPower = 255 - d*wall_wall_kD - wallError*wall_wall_kP;
      //rightPower = 255 + d*wall_wall_kD - wallError*wall_wall_kP;

      int correction = d*wall_wall_kD + wallError*wall_wall_kP;
      int encoder_difference = rightEncReading - leftEncReading;
      if (encoder_difference > MAX_ENCODER_DIFF && correction > 0 ||
        encoder_difference < MAX_ENCODER_DIFF && correction < 0 ) correction = 0; //encoders are too far apart, don't overcorrect. maybe use negative feedback here instead of zero. 
      leftPower = 255 - correction;
      rightPower = 255 + correction;
        
      //leftPower = 255 - maximum(wallError*wall_wall_kP, 100);
      //rightPower = 255 + maximum(wallError*wall_wall_kP, 100);

      lastMode = R;
      lastError = wallError;
      //rightEnc.write(leftEnc.read()); // sync up the encoders because it's not reading them
    }
    else {
      //no walls
      if (lastMode != NONE) {
        rightEnc.write(leftEnc.read()); // assume that the robot got straighened out by the walls
      }
      int leftPos = leftEnc.read();
      int rightPos = rightEnc.read();
      int twistError = leftPos - rightPos;
      leftPower = 255 - twistError*twist_kP;
      rightPower = 255 + twistError*twist_kP;
      lastMode = NONE;
    }

    /*Serial.print("Left: "); Serial.print(leftPower);
    Serial.print(" Right: "); Serial.print(rightPower);*/
    
    leftMotor(leftPower);
    rightMotor(rightPower); // only one of these will take effect because 255 is max power
    updateLocation(tempCoordinate, leftEnc.read());
    threads.delay(25);
  }
  
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
    Serial.println(leftPos);
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


//this is not done yet. use drive straight for now
/*
void goStraightToNode(int dist, Node node) { // I know it's bad to directly copy code from another function and then change it a little to make it work but I'm rushing here.
  // int dist is the number of encoder ticks, not the number of squares.
  Serial.println("go distance without stopping");
  if (facing == NORTH && node.north = NULL || facing == EAST && node.east = NULL || facing == SOUTH && node.south = NULL || facing == WEST && node.west = NULL) {
    // if the destination node has a wall at the end of it, just use goStraightUntilWall();
    goStraightUntilWall();
    return;
  }
  boolean goalLeftWall;
  boolean goalRightWall;

  if (facing==NORTH) {
    goalLeftWall = node.west == NULL;
    goalRightWall = node.east == NULL;
  }
  else if (facing==EAST) {
    goalLeftWall = node.north == NULL;
    goalRightWall = node.south == NULL;    
  }
  else if (facing==SOUTH) {
    goalLeftWall = node.east == NULL;
    goalRightWall = node.west == NULL;
  }
  else if (facing==WEST) {
    goalLeftWall = node.south == NULL;
    goalRightWall = node.north == NULL;
  }
  else {
    // the robot is turning or something
    Serial.println("Error: tried goStraightToNode() while turning");
    return;
  }
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
  int lastMode = NONE;
  int lastError = 0;
  int distanceError = dist;
  boolean didntReachNodeYet = true;
  int nodeIdentification = 0
  if (facing == NORTH) {
    Node* prevNode = node.south; // is this valid? idk how pointers work.
    //assuming NORTH is in the negative y direction...
    if (prevNode != NULL && prevNode.y == node.y + 1) {
      if ((node.west == NULL) != (prevNode.west == NULL) {
        // the left wall changes so we can use that to find the new node.
        
      }
    }
  }*//*
  while (didntReachNodeYet) {*/
    /* If there is a wall on the left and right, center the robot between them.
     * If there is just a wall on the left, put the robot 50mm from that wall.
     * If just on the right, 50mm from that wall.
     * Else, use encoders to keep tracking straight.
     *//*
    if (nodeIdentification == 0) {// no means of distinguishing the destination node from the previous square
      didntReachNodeYet = distanceError > 0;
    }
    int leftWallDist = wallLeft();
    int rightWallDist = wallRight();
    int leftWallTarget = 50; //desired distance from each wall, I dind't measure so it's probably off. Keep in mind the sensors themselved might be inaccurate although precise
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
      if (lastMode != NONE) {
        rightEnc.write(leftEnc.read()); // assume that the robot got straighened out by the walls
      }
      int leftPos = leftEnc.read();
      int rightPos = rightEnc.read();
      int twistError = leftPos - rightPos;
      leftPower = 255 - twistError*twist_kP;
      rightPower = 255 + twistError*twist_kP;
      lastMode = NONE;
    }*/

    /*Serial.print("Left: "); Serial.print(leftPower);
    Serial.print(" Right: "); Serial.println(rightPower);*//*
    leftMotor(leftPower);
    rightMotor(rightPower); // only one of these will take effect because 255 is max power
    int leftPos = leftEnc.read();
    distanceError = dist - leftPos;
    updateLocation(tempCoordinate, leftPos);
    threads.delay(25);
  }

  // identify difference between the target node and the previous node if it's adjacent. otherwise just use encoders to get there.
  
  brake();
  return;
    
}*/

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
  if (wallAhead() <= 55)
    return;
  int lastMode = NONE;
  int lastError = 0;
  while (wallAhead() > 55) {
    /* If there is a wall on the left and right, center the robot between them.
     * If there is just a wall on the left, put the robot 50mm from that wall.
     * If just on the right, 50mm from that wall.
     * Else, use encoders to keep tracking straight.
     */
    int leftPower = 0;
    int rightPower = 0;
    /*
    int leftWallDist = wallLeft();
    int rightWallDist = wallRight();
    int leftWallTarget = 50; //desired distance from each wall
    int rightWallTarget = 50;

    
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
    */
      //no walls
//      if (lastMode != NONE) {
//        rightEnc.write(leftEnc.read()); // assume that the robot got straighened out by the walls
//      }
      int leftPos = leftEnc.read();
      int rightPos = rightEnc.read();
      int twistError = leftPos - rightPos;
      leftPower = 255 - twistError*twist_kP;
      rightPower = 255 + twistError*twist_kP;
////      lastMode = NONE;
//    

    /*Serial.print("Left: "); Serial.print(leftPower);
    Serial.print(" Right: "); Serial.println(rightPower);*/
    leftMotor(leftPower);
    rightMotor(rightPower); // only one of these will take effect because 255 is max power
    updateLocation(tempCoordinate, leftEnc.read());
    threads.delay(25);
  }

  // wallAhead() detected a wall
  brake();
  return;
    
}

void brake() {
  leftMotor(-100);
  rightMotor(-100);
  threads.delay(50);
  leftMotor(0);
  rightMotor(0);
}
/*
void driveUntilNode(Node node) {
  //drive forward in a straight line until reaching the specified node. do so by looking at the coordinates and then confirming that the robot is centered at the destination node because relying purely on coordinates wouldn't work
  //are Node.x and Node.y public?
  roundCoordinates();
  int distance;
  // confirm that you're at the same x or y coordinate of the destination node
  if (xLocation == node.x) {
    
    distance = node.y - yLocation;
    if (distance > 0) {
      turnTo(SOUTH);
    }
    else if (distance < 0) {
      turnTo(NORTH);
      distance = abs(distance);
    }
    
  }
  else if (yLocation == node.y) {
    distance = node.x - xLocation;
    if (distance > 0) {
      turnTo(EAST);
    }
    else if (distance < 0) {
      turnTo(WEST);
      distance = abs(distance);
    }
  }
  else {
    // then you screwed up
    distance = 0;
    Serial.println("invalid driveUntilNode call. destination not in line with robot");
  }
  distance = (int) (distance * MAZE_SQUARE_SIZE);
  goStraight(distance); // simple solution; would be inaccurate
  //goStraightToNode(distance, node); // go almost the full distance then look for walls for a more accurate destination
}*/

void roundCoordinates () {
  //round xLocation and yLocation to nearest integer
  xLocation = round(xLocation);
  yLocation = round(yLocation);
}

boolean atTheCenter() {
  //returns true if the robot is at one of the center coordinates, which I think are (7,7),(7,8),(8,7),and (8,8)
  if (xLocation == 7 && yLocation == 7) return 1;
  else if (xLocation == 7 && yLocation == 8) return 1;
  else if (xLocation == 8 && yLocation == 7) return 1;
  else if (xLocation == 8 && yLocation == 8) return 1;
  else return 0;
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
    if (nearZero(error,leftTurn/20) && nearZero(d, 3500)) {// close to destination but the motor power isn't enough to move the robot
      Serial.print("Booost");
      pwr*=1.5;
    }
    /*Serial.print("Error: "); Serial.print(error);
    Serial.print(" Power: "); Serial.print(pwr);
    Serial.print(" Speed: "); Serial.println(d);*/
    rightMotor(pwr);
    leftMotor(-pwr);
    lastError = error;
    
    if (nearZero(error, leftTurn / 50) && nearZero(angularVelocity, 1000)){ // about 1.5 degree tolerance would be leftTurn / 60
      leftMotor(0);
      rightMotor(0);
      facing = (((wasFacing + dist/rightTurn) % 4)+ 4) % 4; // why the extra +4 %4? if you were facing north and you turned left, you'd get ((-1 % 4) + 4) % 4 = 3. Without the last 2 operations, you'd get -1. There's probably a better way still.
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

void turnTo(int newDir) {
  int oldDir = facing;
  if (newDir == oldDir) return;
  
  // numTurns1 and numTurns2 incorporate the correct signs (ex. for N->E, numTurns1 = -1, numTurns2 = 3)
  int numTurns1 = oldDir - newDir;
  int numTurns2 = -(numTurns1/abs(numTurns1))*(4 - abs(numTurns1));
  
  if (abs(numTurns1) <= abs(numTurns2))
  {
    // sorry, needed to include these cases since |leftTurn| != |rightTurn|
    if (numTurns1 < 0)      turn(abs(numTurns1)*rightTurn);
    else if (numTurns1 > 0) turn(numTurns1*leftTurn);
  }
  else if (abs(numTurns2) < abs(numTurns1))
  {
    if (numTurns2 < 0)      turn(abs(numTurns2)*rightTurn);
    else if (numTurns2 > 0) turn(numTurns2*leftTurn);
  }
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

  /*
  threads.delay(500);
  driveUntilWall();
  threads.delay(500);
  turn(rightTurn);
  Serial.printf("(x, y) = (%f, %f) \n", xLocation, yLocation);
  */

  threads.delay(500);
//  int randNumber;
//  // front wall is present and left wall is open
//  int ret = goStraightUntilCorridor();
//  if (FRONTB & ret && LEFTB & ret == 0) {
//     brake();
//     turn(leftTurn);
//  } else if (FRONTB & ret && RIGHTB & ret == 0) {
//     brake();
//     turn(rightTurn);
//     
//  // otherwise if front wall or left wall not present
//  // favor going straight   
//  } else if (RIGHTB & ret == 0 && FRONTB & ret == 0) {
//    randNumber = random(10);
//    if (randNumber > 3) {goStraightUntilCorridor();}
//    else {
//      brake();
//      turn(leftTurn);
//    }
//  } else if (LEFTB & ret == 0) {
//    randNumber = random(10);
//    if (randNumber > 3) {goStraightUntilCorridor();}
//    else {
//      brake();
//      turn(rightTurn);
//    }
//  }

goStraightUntilWall();
threads.delay(100);
if (wallRight() > 100) {
  turn(rightTurn);

} else if (wallLeft() > 100) {
  turn(leftTurn);  

} else {
  turn(left180);
  }
    //brake();
  
  //goStraight(MAZE_SQUARE_SIZE*2);
  
  //while(1);


}
    
