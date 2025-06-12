#ifdef WITTY_BOARD

#include "sensor_witty.h"
#include "pins.h"

void initWitty() {
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(OUTPUT_CTRL, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(sensorPin, INPUT); // DS18B20 or similar
}

bool readWitty(float& value) {
  int raw = analogRead(MUX_Z);
  value = (raw / 1023.0) * 100.0;

  // Example LED color mapping
  digitalWrite(RGB_RED_PIN, value < 30 ? HIGH : LOW);
  digitalWrite(RGB_GREEN_PIN, (value >= 30 && value <= 70) ? HIGH : LOW);
  digitalWrite(RGB_BLUE_PIN, value > 70 ? HIGH : LOW);

  return true;
}

#endif // WITTY_BOARD
