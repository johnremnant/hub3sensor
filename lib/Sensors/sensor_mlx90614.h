#ifndef SENSOR_MLX90614_H
#define SENSOR_MLX90614_H

#include <Arduino.h>

// Initialize MLX90614 sensor
void initMLX90614();

// Read object temperature from MLX90614
// Temperature is returned in `temperature`, humidity = 0
bool readMLX90614(float& temperature, float& humidity);

#endif // SENSOR_MLX90614_H
