#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <Arduino.h>

enum SensorType : uint8_t {
  SENSOR_NONE = 0,
  SENSOR_DHT,
  SENSOR_BME280,
  SENSOR_BMP280,
  SENSOR_BMP180,
  SENSOR_DS18B20,
  SENSOR_WITTY,
  SENSOR_MLX90614,
  SENSOR_LDR, 
   };

// Function declaration
const char* sensorTypeName(SensorType type);

#endif
