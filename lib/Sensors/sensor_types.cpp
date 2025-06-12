#include "sensor_types.h"

const char* sensorTypeName(SensorType type) {
  switch (type) {
    case SENSOR_NONE:     return "None";
    case SENSOR_DHT:      return "DHT22";
    case SENSOR_BME280:   return "BME280";
    case SENSOR_BMP280:   return "BMP280";
    case SENSOR_BMP180:   return "BMP180";
    case SENSOR_DS18B20:  return "DS18B20";
    case SENSOR_WITTY:    return "WITTY";
    case SENSOR_MLX90614: return "MLX90614";
    case SENSOR_LDR:      return "LDR (Analog Mux)";
    default:              return "Unknown";
  }
}
