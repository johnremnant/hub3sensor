#include "sensor_bmp280.h"
#include <Wire.h>
#include <BME280I2C.h>

BME280I2C bmp;  // Set address as needed: 0x76 or 0x77
bool bmpInitialized = false;

bool initBMP280() {
  if (!bmpInitialized) {
    Serial.println(F("Initializing BMP280 sensor..."));
    Wire.begin();
    if (!bmp.begin()) {
      Serial.println(F("BMP280 sensor init failed!"));
      return false;
    }
    Serial.println(F("BMP280 sensor initialized successfully."));
    bmpInitialized = true;
  }
  return true;
}

bool readBMP280(float& temperature, float& humidity) {
  if (!bmpInitialized && !initBMP280()) {
    Serial.println(F("BMP280 not initialized, read failed."));
    return false;
  }

  float pres, temp, hum;
  bmp.read(pres, temp, hum);  // humidity will be 0 on BMP280
  temperature = temp;  // read first sensor on the bus
  humidity = 0;  // DS18B20 does not measure humidity
   Serial.print(F("BMP280 temperature: "));
  Serial.println(temperature);
  return true;
}
