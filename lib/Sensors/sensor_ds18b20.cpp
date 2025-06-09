#include <OneWire.h>
#include <DallasTemperature.h>
#include "sensor_types.h"

// Define your OneWire bus pin (change as needed)
#define ONE_WIRE_BUS 4  // Example GPIO4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

bool dallasInitialized = false;

bool initDS18B20() {
  sensors.begin();
  dallasInitialized = true;
  return true;
}

bool readDS18B20(float& temperature, float& humidity) {
  if (!dallasInitialized && !initDS18B20()) {
    return false;
  }

  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);  // read first sensor on the bus
  humidity = 0;  // DS18B20 does not measure humidity

  if (temperature == DEVICE_DISCONNECTED_C) {
    temperature = 0;
    return false;
  }

  return true;
}
