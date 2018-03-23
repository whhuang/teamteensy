void readIMUloop() {
  Serial.println("starting the IMU loop");
  unsigned long lastTime = 0;
  threads.delay(20);
  while (1) {
    unsigned long t = millis();
    unsigned long elapsed = t - lastTime;
    lastTime = t;
    readIMU(elapsed);
    threads.delay(20);
  }
}

void readIMU(unsigned long dt) {
  imu.read();
  // with comp filters and stuff, this function will get more complicated
  int reading = imu.g.z - gyro_calibrate;
  if (abs(reading) > 250){
    relativeDirection += (reading)*dt;
    angularVelocity = reading;
    //Serial.println(relativeDirection);
  }
  else {
    angularVelocity = 0;
  }

  // reading the distance sensor must be done here because they're both on the same SDA port
  // replace this code with distanceToSensor(v1);
  uint8_t range = v1.readRange();
  uint8_t status = v1.readRangeStatus();

  if (status == VL6180X_ERROR_NONE)
    v1Range = range;
  else
    v1Range = 404;
}
