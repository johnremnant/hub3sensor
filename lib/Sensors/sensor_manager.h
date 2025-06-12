#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "sensor_types.h"

// Include all sensor headers
#include "sensor_dht.h"
#include "sensor_bme280.h"
#include "sensor_bmp280.h"
#include "sensor_bmp180.h"
#include "sensor_ds18b20.h"
#ifdef SENSOR_WITTY
#include "sensor_witty.h"
#endif
#include "sensor_mlx90614.h"
#include "sensor_ldr.h"


// Active sensor init + read
void initSensor(SensorType type);
bool readSensor(SensorType type, float& temperature, float& humidity);


// Optional utility
float readLDRSensor(bool ldrEnabled);

#endif
