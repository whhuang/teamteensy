#include <i2c_t3.h>
#include "Adafruit_VL6180X_upgrade.h"
#include <LSM6_upgrade.h>

Adafruit_VL6180X vl = Adafruit_VL6180X();
Adafruit_VL6180X v2 = Adafruit_VL6180X();
//Adafruit_VL6180X v3 = Adafruit_VL6180X();

LSM6 imu;
int calibrate_offset;
int direction = 0;
int dt = 20;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);

  // wait for serial port to open on native usb devices
  while (!Serial) {
    delay(1);
  }
  
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

  calibrateGyro();
}

void calibrateGyro() {
  /*
  int calib10;
  for (int i = 0; i < 50; i++) {
    imu.read();
    calib10 += imu.g.z;
    delay(20);
  }
  calibrate_offset = calib10/50;*/
  // instead of the above, it's better to run the calibration utility and take the result from that.
  calibrate_offset = -380;
  Serial.print("calibration offset: ");
  Serial.println(calibrate_offset);
}

void loop() {
  
  unsigned long currentTime = millis();
  //float lux = vl.readLux(VL6180X_ALS_GAIN_5);

  //Serial.print("Lux: "); Serial.print(lux);
  
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();
  uint8_t range2 = v2.readRange();
  uint8_t status2 = v2.readRangeStatus();
  //uint8_t range3 = v3.readRange();
  //uint8_t status3 = v3.readRangeStatus();

  imu.read();
  /*
  if (status == VL6180X_ERROR_NONE) {
    Serial.print("  Range1: "); Serial.print(range);
  }
  else {
    Serial.print("  Range1: far");
  }
  if (status2 == VL6180X_ERROR_NONE) {
    Serial.print("  Range2: "); Serial.print(range2);
  }
  else {
    Serial.print("  Range2: far");
  }*/
  /*if (status3 == VL6180X_ERROR_NONE) {
    Serial.print("  Range3: "); Serial.println(range3);
  }
  else {
    Serial.println("  Range3: far");
  }*/
  //Serial.print("Gyro Z: ");

  if (Serial.available() > 0) {
    //calibrate_offset += direction;
    direction = 0;
    Serial.read();
    Serial.println(0);
  }
  int reading = imu.g.z - calibrate_offset;
  if (abs(reading) > 250){
    unsigned long elapsed = currentTime - lastTime;
    direction += (reading)*elapsed;
    Serial.println(direction);
  }
  
  // Some error occurred, print it out!
  /*
  if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
    Serial.println("System error");
  }
  else if (status == VL6180X_ERROR_ECEFAIL) {
    Serial.println("ECE failure");
  }
  else if (status == VL6180X_ERROR_NOCONVERGE) {
    Serial.println("No convergence");
  }
  else if (status == VL6180X_ERROR_RANGEIGNORE) {
    Serial.println("Ignoring range");
  }
  else if (status == VL6180X_ERROR_SNR) {
    Serial.println("Signal/Noise error");
  }
  else if (status == VL6180X_ERROR_RAWUFLOW) {
    Serial.println("Raw reading underflow");
  }
  else if (status == VL6180X_ERROR_RAWOFLOW) {
    Serial.println("Raw reading overflow");
  }
  else if (status == VL6180X_ERROR_RANGEUFLOW) {
    Serial.println("Range reading underflow");
  }
  else if (status == VL6180X_ERROR_RANGEOFLOW) {
    Serial.println("Range reading overflow");
  }*/
  lastTime = currentTime;
  delay(dt);
  
}
