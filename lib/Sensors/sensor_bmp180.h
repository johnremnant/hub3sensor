#ifndef SENSOR_BMP180_H
#define SENSOR_BMP180_H

#include <Arduino.h>

bool initBMP180();
bool readBMP180(float& temperature, float& humidity);

#endif
