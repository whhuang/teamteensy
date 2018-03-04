#include <Encoder.h>
//#include <Wire.h>
//#include <LIS3MDL.h> magnetometer
#include <LSM6.h>
Encoder leftEnc(17, 16);
Encoder rightEnc(0, 1);

void setup() {
  Serial.begin(9600);
}
long oldPositionLeft  = -999;
long oldPositionRight = -999;
void loop() {
  // put your main code here, to run repeatedly:
  long newPositionLeft = leftEnc.read();
  if (newPositionLeft != oldPositionLeft) {
    oldPositionLeft = newPositionLeft;
    Serial.print("Left: ");
    Serial.println(newPositionLeft);
  }
  long newPositionRight = rightEnc.read();
  if (newPositionRight != oldPositionRight) {
    oldPositionRight = newPositionRight;
    Serial.print("Right: ");
    Serial.println(newPositionRight);
  }
  
}
