#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

#include <Arduino.h>

bool initDHT();
bool readDHT(float& temperature, float& humidity);

#endif
