#ifndef SENSOR_DS18B20_H
#define SENSOR_DS18B20_H

#include <Arduino.h>

// Initialize DS18B20 sensor, returns true if successful
bool initDS18B20();

// Read temperature and humidity (humidity always 0 for DS18B20)
// Returns true if reading successful
bool readDS18B20(float& temperature, float& humidity);

#endif // SENSOR_DS18B20_H
