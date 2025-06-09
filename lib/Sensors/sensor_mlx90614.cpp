#include "sensor_mlx90614.h"
#include <Adafruit_MLX90614.h>

static Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void initMLX90614() {
  mlx.begin();
}

bool readMLX90614(float& temperature, float& humidity) {
  if (!mlx.readAmbientTempC()) {  // This can be adjusted if error checking is needed
    temperature = 0;
    humidity = 0;
    return false;
  }

  temperature = mlx.readObjectTempC(); // Object temperature
  humidity = 0; // Not applicable
  return true;
}
