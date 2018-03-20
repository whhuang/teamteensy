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
  
  Serial.print("Initializing VL6180x...");
  if (! vl.begin(WIRE_0)) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("Done");

  Serial.print("Initializing IMU...");
  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
  
  Serial.println("Done");

}

void calibrateGyro() {
  int interval = 20;
  int duration = 1000;
  int calib;
  for (int i = 0; i < duration/interval; i++) {
    imu.read();
    calib += imu.g.z;
    delay(20);
  }
  calibrate_offset = calib/(duration/interval);
  Serial.print("calibration offset: ");
  Serial.println(calibrate_offset);
}

void loop() {
  calibrateGyro();
  
  delay(dt);
  
}
