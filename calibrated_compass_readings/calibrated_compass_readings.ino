/*
The sensor outputs provided by the library are the raw 16-bit values
obtained by concatenating the 8-bit high and low magnetometer data registers.
They can be converted to units of gauss using the
conversion factors specified in the datasheet for your particular
device and full scale setting (gain).

Example: An LIS3MDL gives a magnetometer X axis reading of 1292 with its
default full scale setting of +/- 4 gauss. The GN specification
in the LIS3MDL datasheet (page 8) states a conversion factor of 6842
LSB/gauss (where LSB means least significant bit) at this FS setting, so the raw
reading of 1292 corresponds to 1292 / 6842 = 0.1888 gauss.
*/

#include <Wire.h>
#include <LIS3MDL.h>

LIS3MDL mag;

char report[80];

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!mag.init())
  {
    Serial.println("Failed to detect and initialize magnetometer!");
    while (1);
  }

  mag.enableDefault();
}

void loop()
{
  mag.read();
  int x = -1*(mag.m.x - 651);
  int y = (mag.m.y +1436)*1.04;
  //float heading = atan(float(x)/y)*180/3.141592654;
  float heading = atan2(float(x), float(y))*180/3.14159265;
  if (heading < 0) heading+=360;
  /*snprintf(report, sizeof(report), "M: %6d %6d %6f",
    x, y, heading);*/
  Serial.println(heading);
  /*snprintf(report, sizeof(report), "M: %6d %6d %6d",
    mag.m.x - 651, (mag.m.y +1436)*1.04, mag.m.z-3705, atan((mag.m.x-651)/(mag.m.y+1436)/1.04));*/
    
  //Serial.println(report);

  delay(100);
}
