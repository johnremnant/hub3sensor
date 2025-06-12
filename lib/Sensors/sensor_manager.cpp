#include "sensor_manager.h"
#include "sensor_types.h"
#ifdef SENSOR_WITTY
#include "sensor_witty.h"
#endif


// Track the currently active sensor
SensorType activeSensor = SENSOR_NONE;

void initSensor(SensorType type) {
  activeSensor = type;

  switch (type) {
    case SENSOR_DHT:        initDHT(); break;
    case SENSOR_BME280:     initBME280(); break;
    case SENSOR_BMP280:     initBMP280(); break;
    case SENSOR_BMP180:     initBMP180(); break;
    case SENSOR_DS18B20:    initDS18B20(); break;
    #ifdef SENSOR_WITTY
    case SENSOR_WITTY:      initWitty(); break;
    #endif
    case SENSOR_MLX90614:   initMLX90614(); break;
    case SENSOR_NONE:
    default: break;
  }
}

bool readSensor(SensorType type, float& temperature, float& humidity)  {
  switch (type) {
    case SENSOR_DHT:
      return readDHT(temperature, humidity);
    case SENSOR_BME280:
      return readBME280(temperature, humidity);
    case SENSOR_BMP280:
      humidity = 0;
      return readBMP280(temperature, humidity);
    case SENSOR_BMP180:
      humidity = 0;
      return readBMP180(temperature, humidity);
    case SENSOR_DS18B20:
      humidity = 0;
      return readDS18B20(temperature, humidity);
    case SENSOR_MLX90614:
      humidity = 0;
      return readMLX90614(temperature, humidity);
    
    case SENSOR_NONE:
    default:
    Serial.println(F("No sensor read"));
      temperature = 0;
      humidity = 0;
      return false;
  }
}

// Optional: LDR read if it’s separate from main sensor flow
float readLDRSensor(bool ldrEnabled) {
  if (ldrEnabled) {
    return readLDR();  // Assumes readLDR() returns a float
  } else {
    Serial.println(F("LDR disabled in config"));
    return 0.0;  // Or -1.0 or NAN if you want to signal "disabled"
  }
}
