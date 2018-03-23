int wallLeft() {
  return distanceToSensor(v3);
}

int wallAhead() {
  return v1Range; //v1 fires every 20ms; might not be a great idea
}

int wallRight() {
  return distanceToSensor(v2);
}


int distanceToSensor(Adafruit_VL6180X sensor) {
  uint8_t range = sensor.readRange();
  uint8_t status = sensor.readRangeStatus();

  if (status == VL6180X_ERROR_NONE)
    return range;
  return 404;
}

