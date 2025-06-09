// sensor_bmp280.h
#ifndef SENSOR_BMP280_H
#define SENSOR_BMP280_H

#include <Arduino.h>

bool initBMP280();                      // Initialization function
bool readBMP280(float& temperature, float& humidity);    // Reading function (no humidity)

#endif
