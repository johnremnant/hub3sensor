#include "sensor_dht.h"
#include <DHT.h>

#define DHTPIN  4       // GPIO where your DHT is connected
#define DHTTYPE DHT22  // DHT22 or DHT11

DHT* dht = nullptr;
bool dhtInitialized = false;

bool initDHT() {
  if (!dhtInitialized) {
    dht = new DHT(DHTPIN, DHTTYPE);
    dht->begin();
    dhtInitialized = true;
  }
  return dhtInitialized;
}

bool readDHT(float& temperature, float& humidity) {
  if (!dhtInitialized && !initDHT()) {
    return false;
  }

  delay(800); // Allow DHT22 to stabilize

  humidity = dht->readHumidity();
  temperature = dht->readTemperature();

  if (isnan(temperature) || isnan(humidity)) {
    return false;
  }

  return true;
}
