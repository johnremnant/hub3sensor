#include "sensor_bmp180.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp180;
bool bmp180Initialized = false;

bool initBMP180() {
  if (!bmp180Initialized) {
    Wire.begin();
    if (!bmp180.begin()) {
      return false;
    }
    bmp180Initialized = true;
  }
  return true;
}

bool readBMP180(float& temperature, float& humidity) {
  if (!bmp180Initialized && !initBMP180()) {
    return false;
  }

  temperature = bmp180.readTemperature();
  humidity = 0;
  return true;
}
