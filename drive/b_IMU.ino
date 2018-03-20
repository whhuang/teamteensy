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
}
