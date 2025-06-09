#include "sensor_bme280.h"
#include <Wire.h>
#include <BME280I2C.h>

BME280I2C bme;  // Default address is 0x76 or 0x77 depending on sensor wiring
bool bmeInitialized = false;

bool initBME280() {
  if (!bmeInitialized) {
    Wire.begin();  // Only needed once, safe to call repeatedly
    if (!bme.begin()) {
      return false;
    }
    bmeInitialized = true;
  }
  return true;
}

bool readBME280(float& temperature, float& humidity) {
  if (!bmeInitialized && !initBME280()) {
    return false;
  }

  float pressure = 0;
  bme.read(pressure, temperature, humidity);
  return true;  // Always returns true if sensor initialized
}
