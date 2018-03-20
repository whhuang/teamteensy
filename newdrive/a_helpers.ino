bool nearZero(int val, int tol) {
  if (-tol < val && val < tol) return true;
  return false;
}

int maximum(int val, int limit) {
  if (val > limit) return limit;
  if (val < -limit) return -limit;
  return val;
}
void leftMotor(int pwr) {
  if (pwr > TOP_SPEED) pwr = TOP_SPEED;
  else if (pwr < -TOP_SPEED) pwr = -TOP_SPEED;
  if (pwr == 0) {
    analogWrite(m1_1, 0);
    analogWrite(m1_2, 0);
  }
  else if (pwr > 0) {
    analogWrite(m1_1, pwr);
    analogWrite(m1_2, 0);
  }
  else {
    analogWrite(m1_2, -pwr);
    analogWrite(m1_1, 0);
  }
}

void rightMotor(int pwr) {
  if (pwr > TOP_SPEED) pwr = TOP_SPEED;
  else if (pwr < -TOP_SPEED) pwr = -TOP_SPEED;
  if (pwr == 0) {
    analogWrite(m2_1, 0);
    analogWrite(m2_2, 0);
  }
  else if (pwr > 0) {
    analogWrite(m2_1, pwr);
    analogWrite(m2_2, 0);
  }
  else {
    analogWrite(m2_2, -pwr);
    analogWrite(m2_1, 0);
  }
}
