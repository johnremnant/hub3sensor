#ifndef SENSOR_BMP_H
#define SENSOR_BMP_H
#include <Arduino.h>

bool initBME280();
bool readBME280(float& temperature, float& humidity);

#endif

